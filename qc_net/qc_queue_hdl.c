#include "qc_queue_hdl.h"
#include "qc_socket.h"
#include "qc_qsystem.h"



QcProduceHdl* qc_producehdl_create(QcQSystem *qSystem, QcErr *err)
{
	QcProduceHdl *produceHdl = (QcProduceHdl*)malloc(sizeof(QcProduceHdl));
	qc_assert(produceHdl);

	produceHdl->qSystem = qSystem;

	return produceHdl;
}


void qc_producehdl_destory(QcProduceHdl *produceHdl)
{
	qc_free(produceHdl);
}


int qc_producehdl_put(QcProduceHdl *produceHdl, const char* qname, QcMessage *message, int msec, QcErr *err)
{
	int ret;
	QcQueue *queue = qc_qsys_queue_get(produceHdl->qSystem, qname, err);
	if (!queue)
		return -1;

	ret = qc_queue_msgput(queue, message, msec, err);
	if (0 != ret)
		return -1;
	return 0;
}


QcConsumeHdl* qc_consumehdl_create(QcQSystem *qSystem, QcErr *err)
{
	QcConsumeHdl *consumeHdl = (QcConsumeHdl*)malloc(sizeof(QcConsumeHdl));
	qc_assert(consumeHdl);

	consumeHdl->qSystem = qSystem;

	return consumeHdl;
}


void qc_consumehdl_destory(QcConsumeHdl *consumeHdl)
{
	qc_free(consumeHdl);
}


QcMessage* qc_consumehdl_get(QcConsumeHdl *consumeHdl, const char* qname, int msec, QcErr *err)
{
	int ret;
	QcQueue *queue = qc_qsys_queue_get(consumeHdl->qSystem, qname, err);
	if (!queue)
		return NULL;

	QcMessage* message = qc_queue_msgget(queue, msec, err);
	return message;
}

