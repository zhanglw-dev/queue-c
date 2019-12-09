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
	if (ret != sizeof(QcPrtclHead))
		goto failed;

	qc_prtcl_msgput_hton(&prtclMsgPut);
	ret = qc_tcp_send(socket, (char*)&prtclMsgPut, sizeof(QcPrtclMsgPut));
	if (ret != sizeof(QcPrtclMsgPut))
		goto failed;

	ret = qc_tcp_send(socket, (char*)qc_message_buff(message), qc_message_bufflen(message));
	if (ret != qc_message_bufflen(message))
		goto failed;
	
	ret = qc_tcp_recvall(socket, (char*)&prtclHead, sizeof(QcPrtclHead));
	if (ret <= 0)
		goto failed;

	QcPrtclReply prtclReply;
	ret = qc_tcp_recvall(socket, (char*)&prtclReply, sizeof(QcPrtclReply));
	if (ret <= 0)
		goto failed;

	if (prtclReply.result != 0)
		goto failed;

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
	if (ret != sizeof(QcPrtclHead))
		goto failed;

	qc_prtcl_msgget_hton(&prtclMsgGet);
	ret = qc_tcp_send(socket, (char*)&prtclMsgGet, sizeof(prtclMsgGet));
	if (ret != sizeof(QcPrtclMsgGet))
		goto failed;

	//receive
	ret = qc_tcp_recvall(socket, (char*)&prtclHead, sizeof(QcPrtclHead));
	if (ret <= 0)
		goto failed;

	QcPrtclReply prtclReply;
	ret = qc_tcp_recvall(socket, (char*)&prtclReply, sizeof(QcPrtclReply));
	if (ret <= 0)
		goto failed;
	qc_prtcl_reply_ntoh(&prtclReply);

	if (prtclReply.result != 0)
		goto failed;

	char *buff;
	qc_malloc(buff, prtclReply.msg_len+1);

	int buff_len = prtclReply.msg_len;
	ret = qc_tcp_recvall(socket, buff, buff_len);
	if (ret <= 0)
		goto failed;

	QcMessage *message = qc_message_create(buff, buff_len, 0);
	return message;

failed:
	return NULL;
}
