#include "qc_consumer_proc.h"
#include "qc_socket.h"
#include "qc_qsystem.h"
#include "qc_protocol.h"


/*
QcConsumeHdl* qc_consumehdl_create(QcQSystem *qSystem, const char* qname, QcErr *err)
{
	QcConsumeHdl *consumeHdl = (QcConsumeHdl*)malloc(sizeof(QcConsumeHdl));
	qc_assert(consumeHdl);

	consumeHdl->qSystem = qSystem;
	strcpy(consumeHdl->qname, qname);

	return consumeHdl;
}


void qc_consumehdl_destory(QcConsumeHdl *consumeHdl)
{
	qc_free(consumeHdl);
}


int qc_consumehdl_get(QcConsumeHdl *consumeHdl, char *prtcl_body, int body_len, char **out_buff, int *bufflen, QcErr *err)
{
	int ret;
	QcPrtclConsume *prtclConsume = prtcl_body;
	qc_prtcl_consume_ntoh(prtclConsume);

	QcQueue *queue = qc_qsys_queue_get(consumeHdl->qSystem, consumeHdl->qname, err);
	if (!queue)
		return -1;

	QcMessage* message = qc_queue_msgget(queue, prtclConsume->wait_msec, err);
	if (!message)
		return -1;

	int msg_len = qc_message_bufflen(message);
	*out_buff = qc_message_buff(message);

	*bufflen = msg_len;
	return 0;
}
*/


int qc_proc_consumer(QcConsumerProc *consumerProc, char *prtcl_buff, QcErr *err)
{
	int ret;
	char *head_buff;
	char *body_buff;
	int msg_len;
	QcSocket *socket = consumerProc->socket;

	QcPrtclReply *prtclReply = (char*)malloc(sizeof(QcPrtclReply));
	QcPrtclRegister* prtclResiter = prtcl_buff + sizeof(QcPrtclRegister);
	strcpy(consumerProc->qname, prtclResiter->qname);


	QcPrtclHead *prtclHead = prtcl_buff;
	qc_prtcl_head_ntoh(prtclHead);
	int body_len = prtclHead->body_len;

	if (prtclHead->subtype != QC_TYPE_REGISTER) {
		goto failed;
	}

	prtclHead->type = QC_TYPE_REPLY;
	ret = qc_tcp_send(socket, prtclHead, sizeof(QcPrtclHead));
	if (ret != sizeof(QcPrtclHead))
		goto failed;

	prtclReply->result = 0;
	prtclReply->msg_len = 0;
	qc_prtcl_reply_hton(prtclReply);
	ret = qc_tcp_send(socket, prtclReply, sizeof(QcPrtclReply));
	if (ret != sizeof(QcPrtclReply))
		goto failed;

	int head_len = sizeof(QcPrtclHead);
	head_buff = (char*)malloc(sizeof(QcPrtclHead));

	while (1) {
		ret = qc_tcp_recvall(socket, head_buff, head_len);
		if (ret <= 0)
			goto failed;

		QcPrtclHead *prtclHead = head_buff;
		qc_prtcl_head_ntoh(prtclHead);
		int body_len = prtclHead->body_len;

		body_buff = (char*)malloc(body_len);
		body_len = prtclHead->body_len;
		ret = qc_tcp_recvall(socket, body_buff, body_len);
		if (ret <= 0)
			goto failed;

		if (prtclHead->type != QC_TYPE_PRODUCER)
			goto failed;

		if (prtclHead->subtype != QC_TYPE_MSGGET) {
			QcPrtclConsume* prtclConsume = body_buff;
			qc_prtcl_produce_ntoh(prtclConsume);

			int wait_msec = prtclConsume->wait_msec;

			QcQueue *queue = qc_qsys_queue_get(consumerProc->qSystem, consumerProc->qname, err);
			if (!queue)
				goto failed;

			char *buff = head_buff + sizeof(QcPrtclProduce);

			QcMessage* message = qc_queue_msgget(queue, prtclConsume->wait_msec, err);
			if (!message)
				return -1;
			msg_len = qc_message_bufflen(message);

			prtclHead->type = QC_TYPE_REPLY;
			ret = qc_tcp_send(socket, prtclHead, sizeof(QcPrtclHead));
			if (ret != sizeof(QcPrtclHead))
				goto failed;

			prtclReply->result = 0;
			prtclReply->msg_len =msg_len;
			qc_prtcl_reply_hton(prtclReply);
			ret = qc_tcp_send(socket, prtclReply, sizeof(QcPrtclReply));
			if (ret != sizeof(QcPrtclReply))
				goto failed;

			ret = qc_tcp_send(socket, qc_message_buff(message), prtclReply->msg_len);
			if (ret != msg_len)
				goto failed;
		}
	}

	return 0;

failed:
	return -1;
}