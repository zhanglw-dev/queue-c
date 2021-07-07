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

#include "qc_qmanager.h"


struct __QcQueManager {
	QcHashTbl *queueTable;
};



QcQueManager* qc_qmng_create()
{
	QcQueManager *qManager = (QcQueManager*)malloc(sizeof(QcQueManager));
	qc_assert(qManager);

	QcHashTbl* queueTable = qc_hashtbl_create(1000, 1, qc_hashstring, qc_strcompare, NULL);
	qc_assert(queueTable);

	qManager->queueTable = queueTable;

	return qManager;
}


void qc_qmng_destory(QcQueManager *qManager)
{
	qc_hashtbl_enumbegin(qManager->queueTable);

	QcQueue *queue = (QcQueue*)qc_hashtbl_enumnext(qManager->queueTable);
	while (queue) {
		qc_queue_destroy(queue);
		queue = (QcQueue*)qc_hashtbl_enumnext(qManager->queueTable);
	}

	qc_hashtbl_destroy(qManager->queueTable);
	qc_free(qManager);
}


int qc_qmng_addqueue(QcQueManager *qManager, const char* qname, QcQueue* queue, QcErr *err)
{
	int ret;
	ret = qc_hashtbl_insert(qManager->queueTable, (void*)qname, queue);
	if (ret != 0){
		qc_seterr(err, QC_ERR_RUNTIME, "qmanager add queue(%s) failed.", qname);
		return -1;
	}
	return 0;
}


int qc_qmng_delqueue(QcQueManager *qManager, const char *qname, QcErr *err)
{
	int ret;
	ret = qc_hashtbl_delete(qManager->queueTable, (void*)qname);
	if (ret != 0){
		qc_seterr(err, QC_ERR_RUNTIME, "qmanager delete queue(%s) failed.", qname);
		return -1;
	}
	return 0;
}


QcQueue* qc_qmng_getqueue(QcQueManager *qManager, const char *qname, QcErr *err)
{
	QcQueue *queue = qc_hashtbl_find(qManager->queueTable, (void*)qname);
	if(NULL == queue){
		qc_seterr(err, QC_ERR_RUNTIME, "qmanager can't find queue(%s).", qname);
	}
	return queue;
}
