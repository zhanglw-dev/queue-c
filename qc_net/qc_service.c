#include "qc_service.h"
#include "qc_message.h"
#include "qc_error.h"
#include "qc_socket.h"
#include "qc_queue.h"
#include "qc_qsystem.h"
#include "qc_protocol.h"


struct __QcQueueSvc {
	char svc_ip[256];
	int svc_port;

	QcThread *listenThread;
	QcList *workThreadList;

	QcQSystem *qSystem;
};


typedef struct {
	QcQueueSvc *queueSvc;
	QcSocket* socket;
}ListenParam;


typedef struct {
	QcQueueSvc *queueSvc;
	QcSocket* socket;
}WorkParam;


typedef struct {
	QcSocket *socket;
	char qname[32];
	QcQSystem *qSystem;
}QcProducerHdl;


typedef struct {
	QcSocket *socket;
	char qname[32];
	QcQSystem *qSystem;
}QcConsumerHdl;



int qc_proc_producer(QcProducerHdl *producerHdl, QcPrtclHead *prtclHead, char *prtcl_body, QcErr *err)
{
	int ret;
	char *head_buff;
	char *body_buff;

	QcPrtclRegister* prtclRegsiter = (QcPrtclRegister*)prtcl_body;
	qc_prtcl_register_ntoh(prtclRegsiter);
	strcpy(producerHdl->qname, prtclRegsiter->qname);

	if (prtclHead->subtype != QC_TYPE_REGISTER) {
		goto failed;
	}

	QcSocket *socket = producerHdl->socket;
	prtclHead->type = QC_TYPE_REPLY;
	ret = qc_tcp_send(socket, (char*)prtclHead, sizeof(QcPrtclHead));
	if (ret != sizeof(QcPrtclHead))
		goto failed;

	QcPrtclReply *prtclReply = (QcPrtclReply*)malloc(sizeof(QcPrtclReply));
	prtclReply->result = 0;
	prtclReply->msg_len = 0;
	qc_prtcl_reply_hton(prtclReply);
	ret = qc_tcp_send(socket, (char*)prtclReply, sizeof(QcPrtclReply));
	if (ret != sizeof(QcPrtclReply))
		goto failed;

	int head_len = sizeof(QcPrtclHead);
	head_buff = (char*)malloc(sizeof(QcPrtclHead));

	while (1) {
		ret = qc_tcp_recvall(socket, head_buff, head_len);
		if (ret <= 0)
			goto failed;

		QcPrtclHead *prtclHead = (QcPrtclHead*)head_buff;
		qc_prtcl_head_ntoh(prtclHead);
		int body_len = prtclHead->body_len;

		body_buff = (char*)malloc(body_len);
		body_len = prtclHead->body_len;
		ret = qc_tcp_recvall(socket, body_buff, body_len);
		if (ret <= 0)
			goto failed;

		if (prtclHead->type != QC_TYPE_PRODUCER)
			goto failed;

		if (prtclHead->subtype == QC_TYPE_MSGPUT) {
			QcPrtclProduce* prtclProduce = (QcPrtclProduce*)body_buff;
			qc_prtcl_produce_ntoh(prtclProduce);

			unsigned short msg_prioriy = prtclProduce->msg_prioriy;
			int wait_msec = prtclProduce->wait_msec;
			unsigned int msg_len = prtclProduce->msg_len;

			QcQueue *queue = qc_qsys_queue_get(producerHdl->qSystem, producerHdl->qname, err);
			if (!queue)
				goto failed;

			char *buff = body_buff + sizeof(QcPrtclProduce);
			QcMessage *message = qc_message_create(buff, msg_len, 1);

			ret = qc_queue_msgput(queue, message, wait_msec, err);
			if (0 != ret)
				goto failed;

			prtclHead->type = QC_TYPE_REPLY;
			ret = qc_tcp_send(socket, (char*)prtclHead, sizeof(QcPrtclHead));
			if (ret != sizeof(QcPrtclHead))
				goto failed;

			prtclReply->result = 0;
			prtclReply->msg_len = msg_len;
			qc_prtcl_reply_hton(prtclReply);
			ret = qc_tcp_send(socket, (char*)prtclReply, sizeof(QcPrtclReply));
			if (ret != sizeof(QcPrtclReply))
				goto failed;

			//ret = qc_tcp_send(socket, msg_buff, msg_len);
			//if (ret != msg_len)
			//	goto failed;
		}
	}

	return 0;

failed:
	return -1;
}


int qc_proc_consumer(QcConsumerHdl *consumerHdl, QcPrtclHead *prtclHead, char *prtcl_body, QcErr *err)
{
	int ret;
	char *head_buff;
	char *body_buff;

	QcPrtclRegister* prtclRegsiter = (QcPrtclRegister*)prtcl_body;
	qc_prtcl_register_ntoh(prtclRegsiter);
	strcpy(consumerHdl->qname, prtclRegsiter->qname);

	if (prtclHead->subtype != QC_TYPE_REGISTER) {
		goto failed;
	}

	QcSocket *socket = consumerHdl->socket;
	prtclHead->type = QC_TYPE_REPLY;
	ret = qc_tcp_send(socket, (char*)prtclHead, sizeof(QcPrtclHead));
	if (ret != sizeof(QcPrtclHead))
		goto failed;

	QcPrtclReply *prtclReply = (QcPrtclReply*)malloc(sizeof(QcPrtclReply));
	prtclReply->result = 0;
	prtclReply->msg_len = 0;
	qc_prtcl_reply_hton(prtclReply);
	ret = qc_tcp_send(socket, (char*)prtclReply, sizeof(QcPrtclReply));
	if (ret != sizeof(QcPrtclReply))
		goto failed;

	int head_len = sizeof(QcPrtclHead);
	head_buff = (char*)malloc(sizeof(QcPrtclHead));

	while (1) {
		ret = qc_tcp_recvall(socket, head_buff, head_len);
		if (ret <= 0)
			goto failed;

		QcPrtclHead *prtclHead = (QcPrtclHead*)head_buff;
		qc_prtcl_head_ntoh(prtclHead);
		int body_len = prtclHead->body_len;

		body_buff = (char*)malloc(body_len);
		body_len = prtclHead->body_len;
		ret = qc_tcp_recvall(socket, body_buff, body_len);
		if (ret <= 0)
			goto failed;

		if (prtclHead->type != QC_TYPE_CONSUMER)
			goto failed;

		if (prtclHead->subtype == QC_TYPE_MSGGET) {
			QcPrtclConsume* prtclConsume = (QcPrtclConsume*)body_buff;
			qc_prtcl_consume_ntoh(prtclConsume);

			int wait_msec = prtclConsume->wait_msec;

			QcQueue *queue = qc_qsys_queue_get(consumerHdl->qSystem, consumerHdl->qname, err);
			if (!queue)
				goto failed;

			char *buff = body_buff + sizeof(QcPrtclConsume);

			QcMessage* message = qc_queue_msgget(queue, prtclConsume->wait_msec, err);
			if (!message)
				return -1;
			int msg_len = qc_message_bufflen(message);

			prtclHead->type = QC_TYPE_REPLY;
			ret = qc_tcp_send(socket, (char*)prtclHead, sizeof(QcPrtclHead));
			if (ret != sizeof(QcPrtclHead))
				goto failed;

			prtclReply->result = 0;
			prtclReply->msg_len = msg_len;
			qc_prtcl_reply_hton(prtclReply);
			ret = qc_tcp_send(socket, (char*)prtclReply, sizeof(QcPrtclReply));
			if (ret != sizeof(QcPrtclReply))
				goto failed;

			ret = qc_tcp_send(socket, (char*)qc_message_buff(message), msg_len);
			if (ret != msg_len)
				goto failed;
		}
	}

	return 0;

failed:
	return -1;
}


//-----------------------------------------------------------------------------------------------------------------

void work_thread_routine(void *param)
{
	int ret;
	QcErr err;
	int msg_len = 0;
	WorkParam *workParam = param;
	QcSocket *socket = workParam->socket;

	char* head_buff = NULL;
	char *body_buff = NULL;
	char qname[32];

	memset(qname, 0, sizeof(qname));

	int head_len = sizeof(QcPrtclHead);
	head_buff = (char*)malloc(sizeof(QcPrtclHead));

	QcPrtclReply *prtclReply;
	qc_malloc(prtclReply, sizeof(QcPrtclReply));

	while (1) {
		ret = qc_tcp_recvall(socket, head_buff, head_len);
		if (ret <= 0)
			goto failed;

		QcPrtclHead *prtclHead = (QcPrtclHead*)head_buff;
		qc_prtcl_head_ntoh(prtclHead);
		int body_len = prtclHead->body_len;

		body_buff = (char*)malloc(body_len);
		body_len = prtclHead->body_len;
		ret = qc_tcp_recvall(socket, body_buff, body_len);
		if (ret <= 0)
			goto failed;
		
		QcProducerHdl producerHdl;
		QcConsumerHdl consumerHdl;

		switch (prtclHead->type) {
		case QC_TYPE_PRODUCER:
			producerHdl.qSystem = workParam->queueSvc->qSystem;
			producerHdl.socket  = socket;
			ret = qc_proc_producer(&producerHdl, prtclHead, body_buff, &err);
			if (ret < 0)
				goto failed;
			break;
		case QC_TYPE_CONSUMER:
			consumerHdl.qSystem = workParam->queueSvc->qSystem;
			consumerHdl.socket  = socket;
			ret = qc_proc_consumer(&consumerHdl, prtclHead, body_buff, &err);
			if (ret < 0)
				goto failed;
			break;
		}
	}

	return;
failed:
	if (head_buff) qc_free(head_buff);
	if (body_buff) qc_free(body_buff);
	return;
}


void listen_thread_routine(void *param)
{
	ListenParam *listenParam = param;

	if (0 != qc_tcp_listen(listenParam->socket))
		return;

	while (1) {
		QcSocket* socket = qc_tcp_accept(listenParam->socket);
		qc_assert(socket);

		WorkParam *workParam = (WorkParam*)malloc(sizeof(WorkParam));
		workParam->queueSvc = listenParam->queueSvc;
		workParam->socket = socket;
		QcThread* thread = qc_thread_create(work_thread_routine, (void*)workParam);

		qc_list_inserttail(listenParam->queueSvc->workThreadList, thread);
	}
}


//-----------------------------------------------------------------------------------------------------------------


QcQueueSvc* qc_queuesvc_create(const char* ip, int port, QcQSystem *qSystem, QcErr *err)
{
	QcQueueSvc *queueSvc;
	qc_malloc(queueSvc, sizeof(QcQueueSvc));

	QcList *procList = qc_list_create(1);

	strcpy(queueSvc->svc_ip, ip);
	queueSvc->svc_port = port;
	queueSvc->workThreadList = procList;
	queueSvc->listenThread = NULL;
	queueSvc->qSystem = qSystem;

	return queueSvc;
}


void qc_queuesvc_destory(QcQueueSvc *queueSvc)
{
	qc_list_destroy(queueSvc->workThreadList);
	qc_free(queueSvc);
}


int qc_queuesvc_start(QcQueueSvc *queueSvc, QcErr *err)
{
	int ret, excode=0;
	QcSocket* socket = qc_socket_create(AF_INET, SOCK_STREAM, 0);

	ListenParam *listenParam;
	qc_malloc(listenParam, sizeof(ListenParam));

	ret = qc_tcp_bind(socket, queueSvc->svc_ip, queueSvc->svc_port);
	if (0 != ret) {
		qc_seterr(err, QC_ERR_SOCKET, "socket bind failed.");
		return -1;
	}

	listenParam->socket = socket;
	listenParam->queueSvc = queueSvc;

	queueSvc->listenThread = qc_thread_create(listen_thread_routine, (void*)listenParam);

	qc_thread_join(queueSvc->listenThread, &excode);

	while (1) {
		QcThread *thread = qc_list_pophead(queueSvc->workThreadList);
		if (!thread)
			break;
		qc_thread_cancel(thread);
		qc_thread_join(thread, &excode);
	}

	return 0;
}


void qc_queuesvc_stop(QcQueueSvc *queueSvc)
{
	qc_thread_cancel(queueSvc->listenThread);
}
