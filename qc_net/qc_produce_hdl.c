#include "qc_produce_hdl.h"
#include "qc_socket.h"
#include "qc_qsystem.h"



QcProduceHdl* qc_producehdl_create(QcQSystem *qSystem, const char* qname, QcErr *err)
{
	QcProduceHdl *produceHdl = (QcProduceHdl*)malloc(sizeof(QcProduceHdl));
	qc_assert(produceHdl);

	produceHdl->qSystem = qSystem;
	strcpy(produceHdl->qname, qname);

	return produceHdl;
}


void qc_producehdl_destory(QcProduceHdl *produceHdl)
{
	qc_free(produceHdl);
}


int qc_producehdl_put(QcProduceHdl *produceHdl, char *buff, int bufflen, int msg_priorty, int msec, QcErr *err)
{
	int ret;
	QcQueue *queue = qc_qsys_queue_get(produceHdl->qSystem, produceHdl->qname, err);
	if (!queue)
		return -1;

	//ret = qc_queue_msgput(queue, message, msec, err);
	//if (0 != ret)
	//	return -1;
	return 0;
}

