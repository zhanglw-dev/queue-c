#include "qc_service.h"
#include "qc_message.h"
#include "qc_error.h"
#include "qc_socket.h"
#include "qc_queue.h"
#include "qc_qsystem.h"
#include "qc_protocol.h"
#include "qc_list.h"
#include "qc_thread.h"


struct __QcQueueSvc {
	char ip[128+1];
	int port;
	int is_running;
	QcQSystem *qSystem;
	QcThread *acceptThread;
	QcList *workThreadList;
};


typedef struct {
	QcQueueSvc *queueSvc;
	QcSocket* socket;
	QcErr *err;
}AcceptParam;


typedef struct {
	QcQueueSvc *queueSvc;
	QcSocket* socket;
	QcErr *err;
}WorkParam;


typedef struct {
	QcSocket *socket;
	char qname[32];
	QcQSystem *qSystem;
	QcErr *err;
}ProcParam;


int qc_proc_msgput(ProcParam *procParam, QcPrtclHead *prtclHead, char *prtcl_body, QcErr *err)
{
	int ret;

	QcPrtclMsgPut* prtclMsgPut = (QcPrtclMsgPut*)prtcl_body;
	qc_prtcl_msgput_ntoh(prtclMsgPut);

	if (prtclHead->type != QC_TYPE_MSGPUT) {
		goto failed;
	}

	unsigned short msg_prioriy = prtclMsgPut->msg_prioriy;
	int wait_msec = prtclMsgPut->wait_msec;
	unsigned int msg_len = prtclMsgPut->msg_len;

	char *buff = prtcl_body + sizeof(QcPrtclMsgPut);
	QcMessage *message = qc_message_create(buff, msg_len, 1);

	ret = qc_qsys_putmsg(procParam->qSystem, prtclMsgPut->qname, message, wait_msec, err);
	if (0 != ret)
		goto failed;

	prtclHead->type = QC_TYPE_REPLY;
	ret = qc_tcp_send(procParam->socket, (char*)prtclHead, sizeof(QcPrtclHead));
	if (ret != sizeof(QcPrtclHead))
		goto failed;

	QcPrtclReply prtclReply;
	prtclReply.result = 0;
	prtclReply.msg_len = msg_len;
	qc_prtcl_reply_hton(&prtclReply);
	ret = qc_tcp_send(procParam->socket, (char*)&prtclReply, sizeof(QcPrtclReply));
	if (ret != sizeof(QcPrtclReply))
		goto failed;

	return 0;
failed:
	return -1;
}


int qc_proc_msgget(ProcParam *procParam, QcPrtclHead *prtclHead, char *prtcl_body, QcErr *err)
{
	int ret;

	QcPrtclMsgGet* prtclMsgGet = (QcPrtclMsgGet*)prtcl_body;
	qc_prtcl_msgget_ntoh(prtclMsgGet);

	if (prtclHead->type != QC_TYPE_MSGGET) {
		goto failed;
	}

	int wait_msec = prtclMsgGet->wait_msec;

	QcMessage* message = qc_qsys_getmsg(procParam->qSystem, prtclMsgGet->qname, prtclMsgGet->wait_msec, err);
	if (NULL == message)
		return -1;

	int msg_len = qc_message_bufflen(message);

	prtclHead->type = QC_TYPE_REPLY;
	ret = qc_tcp_send(procParam->socket, (char*)prtclHead, sizeof(QcPrtclHead));
	if (ret != sizeof(QcPrtclHead))
		goto failed;

	QcPrtclReply prtclReply;
	prtclReply.result = 0;
	prtclReply.msg_len = msg_len;
	qc_prtcl_reply_hton(&prtclReply);
	ret = qc_tcp_send(procParam->socket, (char*)&prtclReply, sizeof(QcPrtclReply));
	if (ret != sizeof(QcPrtclReply))
		goto failed;

	ret = qc_tcp_send(procParam->socket, (char*)qc_message_buff(message), msg_len);
	if (ret != msg_len)
		goto failed;

	return 0;

failed:
	return -1;
}

//-----------------------------------------------------------------------------------------------------------------

void* work_thread_routine(void *param)
{
	int ret;
	int msg_len = 0;
	WorkParam *workParam = param;
	QcSocket *socket = workParam->socket;

	char* head_buff = NULL;
	char *body_buff = NULL;

	int head_len = sizeof(QcPrtclHead);
	qc_malloc(head_buff, sizeof(QcPrtclHead));

	while (1) {
		ret = qc_tcp_recvall(socket, head_buff, head_len);
		if (ret <= 0)
			goto failed;

		QcPrtclHead *prtclHead = (QcPrtclHead*)head_buff;
		qc_prtcl_head_ntoh(prtclHead);
		int body_len = prtclHead->body_len;
		body_buff = (char*)malloc(body_len);

		ret = qc_tcp_recvall(socket, body_buff, body_len);
		if (ret <= 0)
			goto failed;
		
		ProcParam procParam;

		switch (prtclHead->type) {
		case QC_TYPE_MSGGET:
			procParam.qSystem = workParam->queueSvc->qSystem;
			procParam.socket  = socket;
			ret = qc_proc_msgget(&procParam, prtclHead, body_buff, workParam->err);
			if (ret < 0)
				goto failed;
			break;
		case QC_TYPE_MSGPUT:
			procParam.qSystem = workParam->queueSvc->qSystem;
			procParam.socket  = socket;
			ret = qc_proc_msgput(&procParam, prtclHead, body_buff, workParam->err);
			if (ret < 0)
				goto failed;
			break;
		}
	}

	return NULL;
failed:
	qc_free(param);
	if (head_buff) qc_free(head_buff);
	if (body_buff) qc_free(body_buff);
	return NULL;
}


void* accept_thread_routine(void *param)
{
	AcceptParam *acceptParam = param;

	if (0 != qc_tcp_listen(acceptParam->socket))
		return NULL;

	while (1) {
		QcSocket* socket = qc_tcp_accept(acceptParam->socket);
		if (!socket) {
			if (acceptParam->err)
				qc_seterr(acceptParam->err, QC_ERR_SOCKET, "tcp accept failed");
			goto failed;
		}

		WorkParam *workParam = (WorkParam*)malloc(sizeof(WorkParam));
		workParam->queueSvc = acceptParam->queueSvc;
		workParam->err = acceptParam->err;
		workParam->socket = socket;

		QcThread* thread = qc_thread_create(work_thread_routine, (void*)workParam);

		qc_list_inserttail(acceptParam->queueSvc->workThreadList, thread);
	}
	return NULL;

failed:
	qc_free(param);
	return NULL;
}

//-----------------------------------------------------------------------------------------------------------------

QcQueueSvc* qc_queuesvc_create(const char *ip, int port, QcQSystem *qSystem, QcErr *err)
{
	QcQueueSvc *queueSvc;
	qc_malloc(queueSvc, sizeof(QcQueueSvc));

	QcList *procList = qc_list_create(1);
	strcpy(queueSvc->ip, ip);
	queueSvc->port = port;
	queueSvc->is_running = 0;
	queueSvc->workThreadList = procList;
	queueSvc->acceptThread = NULL;
	queueSvc->qSystem = qSystem;

	return queueSvc;
}


void qc_queuesvc_destory(QcQueueSvc *queueSvc)
{
	qc_list_destroy(queueSvc->workThreadList);
	qc_free(queueSvc);
}


int qc_queuesvc_start(QcQueueSvc *queueSvc, int is_async, QcErr *err)
{
	QcSocket* socket = qc_socket_create(AF_INET, SOCK_STREAM, 0);
	if (!socket) {
		qc_seterr(err, QC_ERR_SOCKET, "create socket failed.");
		return -1;
	}

	AcceptParam *acceptParam;
	qc_malloc(acceptParam, sizeof(AcceptParam));

	int ret = qc_tcp_bind(socket, queueSvc->ip, queueSvc->port);
	if (0 != ret) {
		qc_seterr(err, QC_ERR_SOCKET, "socket bind failed.");
		return -1;
	}

	acceptParam->socket = socket;
	acceptParam->queueSvc = queueSvc;

	queueSvc->acceptThread = qc_thread_create(accept_thread_routine, (void*)acceptParam);
	queueSvc->is_running = 1;

	if (!is_async) {
		qc_queuesvc_stop(queueSvc);
	}

	return 0;
}


void qc_queuesvc_stop(QcQueueSvc *queueSvc)
{
	int excode;

	if (queueSvc->is_running)
		return;

	qc_thread_cancel(queueSvc->acceptThread);
	qc_thread_join(queueSvc->acceptThread, &excode);

	while (1) {
		QcThread *thread = qc_list_pophead(queueSvc->workThreadList);
		if (!thread)
			break;
		qc_thread_cancel(thread);
		qc_thread_join(thread, &excode);
	}

	queueSvc->is_running = 0;
}
