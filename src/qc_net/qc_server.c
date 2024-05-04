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

#include "qc_server.h"
#include "qc_message.h"
#include "qc_error.h"
#include "qc_socket.h"
#include "qc_queue.h"
#include "qc_qsystem.h"
#include "qc_protocol.h"
#include "qc_list.h"
#include "qc_thread.h"


struct __QcQueueSrv {
	char ip[128+1];
	int port;
	int is_running;
	QcQSystem *qSystem;
	QcThread *acceptThread;
	QcList *workThreadList;
};


typedef struct {
	QcQueueSrv *queueSrv;
	QcSocket* socket;
	QcErr err;
}AcceptParam;


typedef struct {
	QcQueueSrv *queueSrv;
	QcSocket* socket;
	QcErr err;
}WorkParam;


typedef struct {
	QcSocket *socket;
	char qname[32];
	QcQSystem *qSystem;
	QcErr err;
}ProcParam;


int qc_proc_msgput(ProcParam *procParam, QcPrtclHead *prtclHead, char *prtcl_body, QcErr *err)
{
	int ret;
	int result = 0;

	QcPrtclMsgPut* prtclMsgPut = (QcPrtclMsgPut*)prtcl_body;
	qc_prtcl_msgput_ntoh(prtclMsgPut);

	if (prtclHead->type != QC_TYPE_MSGPUT) {
		qc_seterr(err, QC_ERR_RUNTIME, "runtime error");
		goto failed;
	}

	unsigned short msg_prioriy = prtclMsgPut->msg_prioriy;
	int wait_msec = prtclMsgPut->wait_msec;
	unsigned int msg_len = prtclMsgPut->msg_len;

	char *buff = prtcl_body + sizeof(QcPrtclMsgPut);
	QcMessage *message = qc_message_create(buff, msg_len, BUFFFLAG_CLONE);

	ret = qc_qsys_putmsg(procParam->qSystem, prtclMsgPut->qname, message, wait_msec, err);
	if (0 != ret){
		if(QC_TIMEOUT == err->code){
			result = QC_RESULT_TIMEOUT;
		}
		else{
			result = QC_RESULT_FAILED;
		}
	}
	else{
			result = QC_RESULT_SUCC;
	}

	prtclHead->type = QC_TYPE_REPLY;
	ret = qc_tcp_send(procParam->socket, (char*)prtclHead, sizeof(QcPrtclHead));
	if (ret != sizeof(QcPrtclHead)){
		qc_seterr(err, QC_ERR_SOCKET, "tcp send head err");
		goto failed;
	}

	QcPrtclReply prtclReply;
	prtclReply.result = result;
	prtclReply.msg_len = 0;
	qc_prtcl_reply_hton(&prtclReply);
	ret = qc_tcp_send(procParam->socket, (char*)&prtclReply, sizeof(QcPrtclReply));
	if (ret != sizeof(QcPrtclReply)){
		qc_seterr(err, QC_ERR_SOCKET, "tcp send reply err");
		goto failed;
	}

	return 0;

failed:
	return -1;
}


int qc_proc_msgget(ProcParam *procParam, QcPrtclHead *prtclHead, char *prtcl_body, QcErr *err)
{
	int ret;
	int result = 0;
	int msg_len = 0;
	QcMessage* message  = NULL;

	QcPrtclMsgGet* prtclMsgGet = (QcPrtclMsgGet*)prtcl_body;
	qc_prtcl_msgget_ntoh(prtclMsgGet);

	if (prtclHead->type != QC_TYPE_MSGGET) {
		qc_seterr(err, QC_ERR_RUNTIME, "runtime error");
		goto failed;
	}

	int wait_msec = prtclMsgGet->wait_msec;

	message = qc_qsys_getmsg(procParam->qSystem, prtclMsgGet->qname, prtclMsgGet->wait_msec, err);

	if (NULL == message){
		if(QC_TIMEOUT == err->code){
			result = QC_RESULT_TIMEOUT;
		}
		else{
			result = QC_RESULT_FAILED;
		}
	}
	else{
			result = QC_RESULT_SUCC;
			msg_len = qc_message_bufflen(message);
	}

	prtclHead->type = QC_TYPE_REPLY;
	ret = qc_tcp_send(procParam->socket, (char*)prtclHead, sizeof(QcPrtclHead));
	if (ret != sizeof(QcPrtclHead)){
		qc_seterr(err, QC_ERR_SOCKET, "socket send err");
		goto failed;
	}

	QcPrtclReply prtclReply;
	prtclReply.result = result;
	prtclReply.msg_len = msg_len;
	qc_prtcl_reply_hton(&prtclReply);
	ret = qc_tcp_send(procParam->socket, (char*)&prtclReply, sizeof(QcPrtclReply));
	if (ret != sizeof(QcPrtclReply)){
		qc_seterr(err, QC_ERR_SOCKET, "socket send err");
		goto failed;
	}

    if(msg_len >0){
		ret = qc_tcp_send(procParam->socket, (char*)qc_message_buff(message), msg_len);
		if (ret != msg_len){
			qc_seterr(err, QC_ERR_SOCKET, "socket send err");
			goto failed;
		}
	}

	qc_message_release(message);
	return 0;

failed:
    if(message){
		//try to return this message to queue
		QcErr _err;
		qc_qsys_putmsg(procParam->qSystem, prtclMsgGet->qname, message, 1000, &_err);
	} 
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
		if (ret <= 0){
			goto failed;
		}

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
			procParam.qSystem = workParam->queueSrv->qSystem;
			procParam.socket  = socket;
			ret = qc_proc_msgget(&procParam, prtclHead, body_buff, &workParam->err);
			if (ret < 0)
				goto failed;
			break;
		case QC_TYPE_MSGPUT:
			procParam.qSystem = workParam->queueSrv->qSystem;
			procParam.socket  = socket;
			ret = qc_proc_msgput(&procParam, prtclHead, body_buff, &workParam->err);
			if (ret < 0)
				goto failed;
			break;
		}

		qc_free(body_buff);
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

	//if (0 != qc_tcp_listen(acceptParam->socket))
	//	return NULL;

	while (1) {
		QcSocket* socket = qc_tcp_accept(acceptParam->socket);
		if (!socket) {
			qc_seterr(&acceptParam->err, QC_ERR_SOCKET, "tcp accept failed");
			goto failed;
		}

		WorkParam *workParam = (WorkParam*)malloc(sizeof(WorkParam));
		workParam->queueSrv = acceptParam->queueSrv;
		workParam->err = acceptParam->err;
		workParam->socket = socket;

		QcThread* thread = qc_thread_create(work_thread_routine, (void*)workParam);

		qc_list_inserttail(acceptParam->queueSrv->workThreadList, thread);
	}
	return NULL;

failed:
	qc_free(param);
	return NULL;
}

//-----------------------------------------------------------------------------------------------------------------

QcQueueSrv* qc_queuesrv_create(const char *ip, int port, QcQSystem *qSystem, QcErr *err)
{
	QcQueueSrv *queueSrv;
	qc_malloc(queueSrv, sizeof(QcQueueSrv));

	QcList *procList = qc_list_create(1);
	strcpy(queueSrv->ip, ip);
	queueSrv->port = port;
	queueSrv->is_running = 0;
	queueSrv->workThreadList = procList;
	queueSrv->acceptThread = NULL;
	queueSrv->qSystem = qSystem;

	return queueSrv;
}


void qc_queuesrv_destory(QcQueueSrv *queueSrv)
{
	qc_list_destroy(queueSrv->workThreadList);
	qc_free(queueSrv);
}


int qc_queuesrv_start(QcQueueSrv *queueSrv, int is_async, QcErr *err)
{
	QcSocket* socket = qc_socket_create(AF_INET, SOCK_STREAM, 0);
	if (!socket) {
		qc_seterr(err, QC_ERR_SOCKET, "create socket failed.");
		return -1;
	}
	qc_socket_nagle_onoff(socket, 1);

	AcceptParam *acceptParam;
	qc_malloc(acceptParam, sizeof(AcceptParam));

	int ret = qc_tcp_bind(socket, queueSrv->ip, queueSrv->port);
	if (0 != ret) {
		qc_seterr(err, QC_ERR_SOCKET, "socket bind failed.");
		return -1;
	}

	if (0 != qc_tcp_listen(socket)){
		qc_seterr(err, QC_ERR_SOCKET, "socket listen failed.");
		return -1;
	}

	acceptParam->socket = socket;
	acceptParam->queueSrv = queueSrv;

	queueSrv->acceptThread = qc_thread_create(accept_thread_routine, (void*)acceptParam);
	queueSrv->is_running = 1;

	if (!is_async) {
		qc_queuesrv_stop(queueSrv);
	}

	return 0;
}


void qc_queuesrv_stop(QcQueueSrv *queueSrv)
{
	int excode;

	if (queueSrv->is_running)
		return;

	qc_thread_cancel(queueSrv->acceptThread);
	qc_thread_join(queueSrv->acceptThread, &excode);

	while (1) {
		QcThread *thread = qc_list_pophead(queueSrv->workThreadList);
		if (!thread)
			break;
		qc_thread_cancel(thread);
		qc_thread_join(thread, &excode);
	}

	queueSrv->is_running = 0;
}
