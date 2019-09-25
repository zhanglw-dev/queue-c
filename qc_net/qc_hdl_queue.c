#include "qc_hdl_queue.h"
#include "qc_socket.h"
#include "qc_qsystem.h"



QcProduceHdl* qc_producehdl_create(const char* ip, int port, const char* queue_name, QcErr *err)
{
	QcSocket* socket = qc_socket_create(AF_INET, SOCK_STREAM, 0);
	qc_assert(socket);

	QcProduceHdl *produceHdl = (QcProduceHdl*)malloc(sizeof(QcProduceHdl));
	qc_assert(produceHdl);

	produceHdl->socket = socket;

	QcQSystem *qSystem = getQSystem();
	qc_assert(qSystem);

	QcQueue *queue = qc_qsys_queue_get(qSystem, queue_name, err);
	if (!queue)
		return NULL;

	produceHdl->queue = queue;

	return produceHdl;
}


void qc_producehdl_destory(QcProduceHdl *produceHdl)
{
	qc_socket_close(produceHdl->socket);
	qc_free(produceHdl);
}


int qc_producehdl_put(QcProduceHdl *produceHdl, QcMessage *message, int msec, QcErr *err)
{
	int ret;
	ret = qc_queue_msgput(produceHdl->queue, message, msec, err);
	if (0 != ret)
		return -1;
	return 0;
}


QcConsumeHdl* qc_consumehdl_create(const char* ip, int port, const char* queue_name, QcErr *err)
{
	QcSocket* socket = qc_socket_create(AF_INET, SOCK_STREAM, 0);
	qc_assert(socket);

	QcConsumeHdl *consumeHdl = (QcConsumeHdl*)malloc(sizeof(QcConsumeHdl));
	qc_assert(consumeHdl);

	consumeHdl->socket = socket;

	QcQSystem *qSystem = getQSystem();
	qc_assert(qSystem);

	QcQueue *queue = qc_qsys_queue_get(qSystem, queue_name, err);
	if (!queue)
		return NULL;

	consumeHdl->queue = queue;

	return consumeHdl;
}


void qc_consumehdl_destory(QcConsumeHdl *consumeHdl)
{
	qc_socket_close(consumeHdl->socket);
	qc_free(consumeHdl);
}


QcMessage* qc_consumehdl_get(QcConsumeHdl *consumeHdl, int msec, QcErr *err)
{
	QcMessage* message = qc_queue_msgget(consumeHdl->queue, msec, err);
	return message;
}

