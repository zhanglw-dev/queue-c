#include "qc_queue_svc.h"
#include "qc_protocol.h"


struct __ListenParam {
	QcQueueSvc *queueSvc;
	QcSocket* socket;
};


struct __WorkParam {
	QcQueueSvc *queueSvc;
	QcSocket* socket;
};


typedef struct __ListenParam ListenParam;
typedef struct __WorkParam WorkParam;



void* process_thread_routine(void *param)
{
	WorkParam *workParam = param;
	QcSocket *socket = workParam->socket;

	QcProduceHdl *produceHdl = workParam->queueSvc->produceHdl;
	QcConsumeHdl *consumeHdl = workParam->queueSvc->consumeHdl;

	int ret;
	QcErr err;
	while (1) {
		//ret = qc_tcp_recvall(socket, char *recvbuf, int len)
		//
		//parse head
		//read body
		int client_type;
		if (client_type == QC_TYPE_PRODUCER) {
			//ret = qc_producehdl_put(QcProduceHdl *produceHdl, const char* qname, QcMessage *message, int msec, &err);
		}
		else if (client_type == QC_TYPE_CONSUMER) {
			//QcMessage* qc_consumehdl_get(QcConsumeHdl *consumeHdl, const char* qname, int msec, QcErr *err);
		}

		//Reply to socket
		
	}
}


void* listen_thread_routine(void *param)
{
	ListenParam *listenParam = param;

	while (1) {
		QcSocket* socket = qc_tcp_accept(listenParam->socket);
		qc_assert(socket);

		WorkParam *workParam = (WorkParam*)malloc(sizeof(WorkParam));
		workParam->queueSvc = listenParam->queueSvc;
		workParam->socket = socket;
		QcThread* thread = qc_thread_create(process_thread_routine, (void*)workParam);

		qc_list_inserttail(listenParam->queueSvc->workThreadList, thread);
	}

	//
}


QcQueueSvc* qc_queuesvc_create(const char* ip, int port, QcQSystem *qSystem, QcErr *err)
{
	QcQueueSvc *queueSvc = (QcQueueSvc*)malloc(sizeof(QcQueueSvc));
	qc_assert(queueSvc);
	memset(queueSvc, 0, sizeof(QcQueueSvc));

	QcProduceHdl *produceHdl = qc_producehdl_create(qSystem, err);
	QcConsumeHdl *consumeHdl = qc_consumehdl_create(qSystem, err);

	QcList *procList = qc_list_create(1);

	strcpy(queueSvc->svc_ip, ip);
	queueSvc->svc_port = port;
	queueSvc->consumeHdl = consumeHdl;
	queueSvc->produceHdl = produceHdl;
	queueSvc->workThreadList = procList;
	queueSvc->listenThread = NULL;

	return queueSvc;
}


void qc_queuesvc_destory(QcQueueSvc *queueSvc)
{
	qc_staticlist_release(queueSvc->workThreadList);
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

	queueSvc->listenThread = qc_thread_create(listen_thread_routine, (void*)listenParam);
	return 0;
}


void qc_queuesvc_stop(QcQueueSvc *queueSvc)
{
	//
}
