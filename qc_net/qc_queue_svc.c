#include "qc_queue_svc.h"



struct __ListenParam {
	QcQueueSvc *queueSvc;
	QcSocket* socket;
};


struct __ProcParam {
	QcQueueSvc *queueSvc;
	QcSocket* socket;
};


typedef struct __ListenParam ListenParam;
typedef struct __ProcParam ProcParam;



void* process_thread_routine(void *param)
{
	ProcParam *procParam = param;

	while (1) {
		//
	}
}


void* listen_thread_routine(void *param)
{
	ListenParam *listenParam = param;

	while (1) {
		QcSocket* socket = qc_tcp_accept(listenParam->socket);
		qc_assert(socket);

		ProcParam *procParam = (ProcParam*)malloc(sizeof(ProcParam));
		procParam->queueSvc = listenParam->queueSvc;
		procParam->socket = socket;
		QcThread* thread = qc_thread_create(process_thread_routine, (void*)procParam);

		qc_list_inserttail(listenParam->queueSvc->procList, thread);
	}

	//
}


QcQueueSvc* qc_queuesvc_create(const char* ip, int port, QcQSystem *qSystem, QcErr *err)
{
	QcQueueSvc *queueSvc = (QcQueueSvc*)malloc(sizeof(QcQueueSvc));
	qc_assert(queueSvc);
	memset(queueSvc, 0, sizeof(QcQueueSvc));

	QcSocket *socket = qc_socket_create(AF_INET, SOCK_STREAM, 0);
	
	QcProduceHdl *produceHdl = qc_producehdl_create(qSystem, err);
	QcConsumeHdl *consumeHdl = qc_consumehdl_create(qSystem, err);

	QcList *procList = qc_list_create(1);

	strcpy(queueSvc->svc_ip, ip);
	queueSvc->svc_port = port;
	queueSvc->consumeHdl = consumeHdl;
	queueSvc->produceHdl = produceHdl;
	queueSvc->procList = procList;
	queueSvc->listen_thread = NULL;

	return queueSvc;
}


void qc_queuesvc_destory(QcQueueSvc *queueSvc)
{
	qc_free(queueSvc);
}


int qc_queuesvc_start(QcQueueSvc *queueSvc, QcErr *err)
{
	int ret;
	QcSocket* socket = qc_socket_create(AF_INET, SOCK_STREAM, 0);

	ListenParam *listenParam = (ListenParam*)malloc(sizeof(ListenParam));

	ret = qc_tcp_bind(socket, queueSvc->svc_ip, queueSvc->svc_port);
	if (0 != ret)
		return -1;

	listenParam->socket = socket;
	listenParam->queueSvc = queueSvc;

	queueSvc->listen_thread = qc_thread_create(listen_thread_routine, (void*)listenParam);
	return 0;
}


void qc_queuesvc_stop(QcQueueSvc *queueSvc)
{
	//
}
