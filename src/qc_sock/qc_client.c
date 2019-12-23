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

#include "qc_client.h"
#include "qc_protocol.h"
#include "qc_socket.h"


struct __QcClient {
	QcSocket *socket;
};


QcClient* qc_client_connect(const char *ip, int port, QcErr *err)
{
	int ret;

	QcClient *client = (QcClient*)malloc(sizeof(QcClient));
	QcSocket *socket = qc_socket_create(AF_INET, SOCK_STREAM, 0);
	qc_socket_nagle_onoff(socket, 1);

	ret = qc_tcp_connect(socket, ip, port);
	if (0 != ret)
		goto failed;

	client->socket = socket;

	return client;
failed:
	return NULL;
}


void qc_client_disconnect(QcClient *client)
{
	qc_socket_close(client->socket);
}


int qc_client_msgput(QcClient *client, const char *qname, QcMessage *message, int msec, QcErr *err)
{
	int ret;
	QcSocket *socket = client->socket;

	QcPrtclHead prtclHead;
	memset(&prtclHead, 0, sizeof(QcPrtclHead));
	prtclHead.protocol = QC_PROTOCOL_MQ;
	prtclHead.version = QC_PROTOCOL_VERSION;
	prtclHead.type = QC_TYPE_MSGPUT;
	prtclHead.packsn = 0;

	QcPrtclMsgPut prtclMsgPut;
	prtclMsgPut.wait_msec = msec;
	prtclMsgPut.msg_prioriy = qc_message_priority(message);
	prtclMsgPut.msg_len = qc_message_bufflen(message);
	strcpy(prtclMsgPut.qname, qname);

	prtclHead.body_len = sizeof(QcPrtclMsgPut) + prtclMsgPut.msg_len;

	qc_prtcl_head_hton(&prtclHead);
	ret = qc_tcp_send(socket, (char*)&prtclHead, sizeof(QcPrtclHead));
	if (ret != sizeof(QcPrtclHead)){
		goto failed;
	}

	qc_prtcl_msgput_hton(&prtclMsgPut);
	ret = qc_tcp_send(socket, (char*)&prtclMsgPut, sizeof(QcPrtclMsgPut));
	if (ret != sizeof(QcPrtclMsgPut)){
		goto failed;
	}

	ret = qc_tcp_send(socket, (char*)qc_message_buff(message), qc_message_bufflen(message));
	if (ret != qc_message_bufflen(message)){
		goto failed;
	}

	ret = qc_tcp_recvall(socket, (char*)&prtclHead, sizeof(QcPrtclHead));
	if (ret <= 0){
		goto failed;
	}

	QcPrtclReply prtclReply;
	ret = qc_tcp_recvall(socket, (char*)&prtclReply, sizeof(QcPrtclReply));
	if (ret <= 0){
		goto failed;
	}

	if (prtclReply.result != QC_RESULT_SUCC){
		if(prtclReply.result == QC_RESULT_TIMEOUT){
			qc_seterr(err, QC_ERR_TIMEOUT, "time out");
			goto failed;
		}
		else{
			qc_seterr(err, QC_ERR_UNKOWN, "unkown failed");
			goto failed;
		}
	}

	return 0;

failed:
	return -1;
}


QcMessage* qc_client_msgget(QcClient *client, const char *qname, int msec, QcErr *err)
{
	int ret;
	QcSocket *socket = client->socket;

	QcPrtclHead prtclHead;
	memset(&prtclHead, 0, sizeof(QcPrtclHead));
	prtclHead.protocol = QC_PROTOCOL_MQ;
	prtclHead.version = QC_PROTOCOL_VERSION;
	prtclHead.type = QC_TYPE_MSGGET;
	prtclHead.packsn = 0;

	QcPrtclMsgGet prtclMsgGet;
	prtclMsgGet.wait_msec = msec;
	strcpy(prtclMsgGet.qname, qname);

	prtclHead.body_len = sizeof(QcPrtclMsgGet);

	qc_prtcl_head_hton(&prtclHead);
	ret = qc_tcp_send(socket, (char*)&prtclHead, sizeof(QcPrtclHead));
	if (ret != sizeof(QcPrtclHead)){
		qc_seterr(err, QC_ERR_SOCKET, "tcp send head failed.");
		goto failed;
	}

	qc_prtcl_msgget_hton(&prtclMsgGet);
	ret = qc_tcp_send(socket, (char*)&prtclMsgGet, sizeof(prtclMsgGet));
	if (ret != sizeof(QcPrtclMsgGet)){
		qc_seterr(err, QC_ERR_SOCKET, "tcp send body failed.");
		goto failed;
	}

	//receive
	ret = qc_tcp_recvall(socket, (char*)&prtclHead, sizeof(QcPrtclHead));
	if (ret <= 0){
		qc_seterr(err, QC_ERR_SOCKET, "tcp recv head failed.");
		goto failed;
	}

	QcPrtclReply prtclReply;
	ret = qc_tcp_recvall(socket, (char*)&prtclReply, sizeof(QcPrtclReply));
	if (ret <= 0){
		qc_seterr(err, QC_ERR_SOCKET, "tcp recv relpy failed.");
		goto failed;
	}

	qc_prtcl_reply_ntoh(&prtclReply);

	if (prtclReply.result == QC_RESULT_SUCC){
		char *buff;
		qc_malloc(buff, prtclReply.msg_len+1);

		int buff_len = prtclReply.msg_len;
		ret = qc_tcp_recvall(socket, buff, buff_len);
		if (ret <= 0){
			qc_seterr(err, QC_ERR_SOCKET, "tcp recv buff failed.");
			goto failed;
		}

		QcMessage *message = qc_message_create(buff, buff_len, 0);
		return message;

	}
	else if(prtclReply.result == QC_RESULT_TIMEOUT){
		qc_seterr(err, QC_ERR_TIMEOUT, "time out");
		goto failed;
	}
	else{
		qc_seterr(err, QC_ERR_UNKOWN, "unkown failed");
		goto failed;
	}

failed:
	return NULL;
}
