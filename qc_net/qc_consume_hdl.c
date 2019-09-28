#include "qc_consume_hdl.h"
#include "qc_socket.h"
#include "qc_qsystem.h"



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


int qc_consumehdl_get(QcConsumeHdl *consumeHdl, char **buff, int *bufflen, int msec, QcErr *err)
{
	int ret;
	QcQueue *queue = qc_qsys_queue_get(consumeHdl->qSystem, consumeHdl->qname, err);
	if (!queue)
		return NULL;

	QcMessage* message = qc_queue_msgget(queue, msec, err);
	return 0;
}

