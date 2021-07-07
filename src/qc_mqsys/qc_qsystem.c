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

#include "qc_qsystem.h"
#include "qc_qmanager.h"
#include "qc_queue.h"



struct __QcQSystem {
	QcQueManager *qManager;
};



QcQSystem* qc_qsys_create()
{
	QcQueManager *qManager = qc_qmng_create();
	if (!qManager)
		return NULL;

	QcQSystem *qSystem = (QcQSystem*)malloc(sizeof(QcQSystem));
	qc_assert(qSystem);

	qSystem->qManager = qManager;

	return qSystem;
}


void qc_qsys_destory(QcQSystem *qSys)
{
	qc_qmng_destory(qSys->qManager);
	qc_free(qSys);
}


int qc_qsys_addqueue(QcQSystem *qSys, const char *qname, QcQueue *queue, QcErr *err)
{
	int ret;
	ret = qc_qmng_addqueue(qSys->qManager, qname, queue, err);
	if (0 != ret)
		return -1;
	return 0;
}


int qc_qsys_delqueue(QcQSystem *qSys, const char *qname, QcErr *err)
{
	int ret;
	ret = qc_qmng_delqueue(qSys->qManager, qname, err);
	if (0 != ret)
		return -1;
	return 0;
}


QcQueue* qc_qsys_queue_get(QcQSystem *qSys, const char *qname, QcErr *err)
{
	QcQueue *queue;
	queue = qc_qmng_getqueue(qSys->qManager, qname, err);
	return queue;
}


int qc_qsys_putmsg(QcQSystem *qSys, const char *qname, QcMessage *message, int msec, QcErr *err)
{
	qc_assert(qname);
	QcQueue *queue;
	queue = qc_qmng_getqueue(qSys->qManager, qname, err);
	if (NULL == queue) {
		return QC_ERR_QUEUE_NOTEXIST;
	}

	int ret = qc_queue_msgput(queue, message, msec, err);
	return ret;
}


QcMessage* qc_qsys_getmsg(QcQSystem *qSys, const char *qname, int msec, QcErr *err)
{
	qc_assert(qname);
	QcQueue *queue;
	queue = qc_qmng_getqueue(qSys->qManager, qname, err);
	if (NULL == queue) {
		return NULL;
	}

	QcMessage* message = qc_queue_msgget(queue, msec, err);
	return message;
}
