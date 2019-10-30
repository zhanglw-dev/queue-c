#include "qc_client.h"
#include "qc_protocol.h"



//type: 1 producer, 2 consumer
static QcClient* client_connect(const char *ip, int port, const char *qname, unsigned short type, QcErr *err)
{
	int ret;

	QcClient *client = (QcClient*)malloc(sizeof(QcClient));
	QcSocket *socket = qc_socket_create(AF_INET, SOCK_STREAM, 0);

	ret = qc_tcp_connect(socket, ip, port);
	if (0 != ret)
		goto failed;

	QcPrtclHead prtclHead;
	memset(&prtclHead, 0, sizeof(QcPrtclHead));
	prtclHead.protocol = QC_PROTOCOL_MQ;
	prtclHead.version = QC_PROTOCOL_VERSION;
	prtclHead.type = type;
	prtclHead.subtype = QC_TYPE_REGISTER;
	prtclHead.packsn = 0;
	prtclHead.body_len = sizeof(QcPrtclRegister);

	QcPrtclRegister prtclRegister;
	memset(&prtclRegister, 0, sizeof(QcPrtclRegister));
	strcpy(prtclRegister.qname, qname);
	qc_prtcl_register_hton(&prtclRegister);

	qc_prtcl_head_hton(&prtclHead);
	ret = qc_tcp_send(socket, &prtclHead, sizeof(QcPrtclHead));
	if (ret != sizeof(QcPrtclHead))
		goto failed;

	qc_prtcl_register_hton(&prtclRegister);
	ret = qc_tcp_send(socket, &prtclRegister, sizeof(QcPrtclRegister));
	if (ret != sizeof(QcPrtclRegister))
		goto failed;

	ret = qc_tcp_recvall(socket, &prtclHead, sizeof(QcPrtclHead));
	if (ret < 0)
		goto failed;
	qc_prtcl_head_ntoh(&prtclHead);

	QcPrtclReply prtclReply;
	ret = qc_tcp_recvall(socket, &prtclReply, sizeof(QcPrtclReply));
	if (ret <= 0)
		goto failed;
	qc_prtcl_reply_ntoh(&prtclReply);

	if (prtclReply.result != 0) {
		return -1;
	}

	client->socket = socket;
	return client;

failed:
	return NULL;
}


static void client_disconnect(QcClient *client)
{
	qc_socket_close(client->socket);
}


QcClient* qc_producer_connect(const char *ip, int port, const char* qname, QcErr *err)
{
	return client_connect(ip, port, qname, QC_TYPE_PRODUCER, err);
}


void qc_producer_disconnect(QcClient *client)
{
	client_disconnect(client);
}


int qc_producer_msgput(QcClient *client, QcMessage *message, int msec, QcErr *err)
{
	int ret;

	QcPrtclHead prtclHead;
	memset(&prtclHead, 0, sizeof(QcPrtclHead));
	prtclHead.protocol = QC_PROTOCOL_MQ;
	prtclHead.version = QC_PROTOCOL_VERSION;
	prtclHead.type = QC_TYPE_PRODUCER;
	prtclHead.subtype = QC_TYPE_MSGPUT;
	prtclHead.packsn = 0;
	////prtclHead.body_len = ;

	QcPrtclProduce prtclProduce;
	prtclProduce.wait_msec = msec;
	prtclProduce.msg_prioriy = qc_message_priority(message);
	prtclProduce.msg_len = qc_message_bufflen(message);

	prtclHead.body_len = sizeof(QcPrtclProduce) + prtclProduce.msg_len;

	qc_prtcl_head_hton(&prtclHead);
	ret = qc_tcp_send(socket, &prtclHead, sizeof(QcPrtclHead));
	if (ret != sizeof(QcPrtclHead))
		goto failed;

	qc_prtcl_head_hton(&prtclProduce);
	ret = qc_tcp_send(socket, &prtclProduce, sizeof(QcPrtclProduce));
	if (ret != sizeof(QcPrtclProduce))
		goto failed;

	ret = qc_tcp_send(socket, qc_message_buff(message), qc_message_bufflen(message));
	if (ret != qc_message_bufflen(message))
		goto failed;

	QcPrtclReply prtclReply;
	ret = qc_tcp_recvall(socket, &prtclReply, sizeof(QcPrtclReply));
	if (ret <= 0)
		goto failed;

	return 0;

failed:
	return -1;
}


QcClient* qc_consumer_connect(const char *ip, int port, const char* qname, QcErr *err)
{
	return client_connect(ip, port, qname, QC_TYPE_CONSUMER, err);
}


void qc_consumer_disconnect(QcClient *client)
{
	client_disconnect(client);
}


QcMessage* qc_consumer_msgget(QcClient *client, int msec, QcErr *err)
{
	int ret;

	QcPrtclHead prtclHead;
	memset(&prtclHead, 0, sizeof(QcPrtclHead));
	prtclHead.protocol = QC_PROTOCOL_MQ;
	prtclHead.version = QC_PROTOCOL_VERSION;
	prtclHead.type = QC_TYPE_PRODUCER;
	prtclHead.subtype = QC_TYPE_MSGPUT;
	prtclHead.packsn = 0;
	////prtclHead.body_len = ;

	QcPrtclConsume prtclConsume;
	prtclConsume.wait_msec = msec;

	prtclHead.body_len = sizeof(QcPrtclConsume);

	qc_prtcl_head_hton(&prtclHead);
	ret = qc_tcp_send(socket, &prtclHead, sizeof(QcPrtclHead));
	if (ret != sizeof(QcPrtclHead))
		goto failed;

	qc_prtcl_head_hton(&prtclConsume);
	ret = qc_tcp_send(socket, &prtclConsume, sizeof(QcPrtclConsume));
	if (ret != sizeof(QcPrtclConsume))
		goto failed;

	//receive
	ret = qc_tcp_recvall(socket, &prtclHead, sizeof(QcPrtclHead));
	if (ret <= 0)
		goto failed;

	QcPrtclReply prtclReply;
	ret = qc_tcp_recvall(socket, &prtclReply, sizeof(QcPrtclReply));
	if (ret <= 0)
		goto failed;

	if (prtclReply.result != 0)
		goto failed;

	char *buff = (char*)malloc(prtclReply.msg_len);
	int buff_len = prtclReply.msg_len;
	ret = qc_tcp_recvall(socket, &buff, buff_len);
	if (ret <= 0)
		goto failed;

	QcMessage *message = qc_message_create(buff, buff_len, 0);
	return message;

failed:
	return NULL;
}
