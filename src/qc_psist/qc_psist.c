/*
 * BSD 3-Clause License
 * 
 * Copyright (c) 2019, zhanglw (zhanglw366@163.com)
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "qc_psist.h"
#include "qc_psist_file.h"
#include "qc_utils.h"



struct __QcPsist {
	QcPsistFile *qtbl;
	QcNumPool *persistIdPool;
};



QcPsist* qc_psist_open(int msgbuff_size, int msgcount_limit, const char *descript, QcErr *err)
{
	qc_assert(descript);

	QcPsist *queQdb;
	qc_malloc(queQdb, sizeof(QcPsist));

	char* filedesc = "file://";
	if (strstr(descript, filedesc) == 0) {
		qc_seterr(err, QC_ERR_BADPARAM, "psist descript unlegal");
		return NULL;
	}
	char* filepath = (char*)descript + strlen(filedesc);

	QcPsistFile* qtbl = qc_psist_file_open(msgbuff_size, msgcount_limit, filepath, err);
	if (!qtbl) {
		qc_free(queQdb);
		return NULL;
	}

	QcNumPool *persistIdPool;
	qc_malloc(persistIdPool, sizeof(QcNumPool));
	qc_numpool_init(persistIdPool, msgcount_limit);

	queQdb->qtbl = qtbl;
	queQdb->persistIdPool = persistIdPool;

	return queQdb;
}


void qc_psist_close(QcPsist *queQdb)
{
	qc_psist_file_close(queQdb->qtbl);
	qc_numpool_release(queQdb->persistIdPool);
	qc_free(queQdb);
}


int qc_psist_append(QcPsist *qTbl, QcMessage *message, QcErr *err)
{
	int ret;

	Qc_MsgRecord *msgRecord = (Qc_MsgRecord*)message;
	msgRecord->persist_id = qc_numpool_get(qTbl->persistIdPool);
	if (msgRecord->persist_id < 0)
		return -1;

	ret = qc_psist_file_append(qTbl->qtbl, msgRecord, err);
	if (0 != ret)
		return -1;

	return 0;
}


int qc_psist_remove(QcPsist *qTbl, QcMessage *message, QcErr *err)
{
	int ret;

	Qc_MsgRecord *msgRecord = (Qc_MsgRecord*)message;
	ret = qc_psist_file_remove(qTbl->qtbl, (Qc_MsgRecord*)message, err);
	if (0 != ret)
		return -1;

	qc_numpool_put(qTbl->persistIdPool, msgRecord->persist_id);

	return 0;
}


int qc_psist_loadqueue(QcPsist *qTbl, QcQueue *queue, QcErr *err)
{
	int ret;
	QcPsistFile *qtbl = qTbl->qtbl;

	if (0 != qc_psist_file_fetch_ready(qtbl, err))
		return -1;

	Qc_MsgRecord *msgRecord = NULL;

	while (1) {
		qc_malloc(msgRecord, sizeof(Qc_MsgRecord));  //Qc_MsgRecord === QcMessage :-|
		ret = qc_psist_file_do_fetch(qtbl, msgRecord, err);
		if (0 == ret) {
			ret = qc_queue_msgput(queue, (QcMessage*)msgRecord, 0, err);
			if (ret < 0)
				goto failed;
		}
		else if (1 == ret) { //completed!
			qc_free(msgRecord);
			break;
		}
		else {
			goto failed;
		}
	}
	return 0;

failed:
	qc_free(msgRecord);
	return -1;
}
