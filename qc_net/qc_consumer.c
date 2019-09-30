#include "qc_consumer.h"
#include "qc_socket.h"
#include "qc_qsystem.h"
#include "qc_protocol.h"



int qc_proc_consumer(QcConsumerHdl *consumerHdl, char *prtcl_buff, QcErr *err)
{
	int ret;
	char *head_buff;
	char *body_buff;
	int msg_len;
	QcSocket *socket = consumerHdl->socket;

	QcPrtclReply *prtclReply = (char*)malloc(sizeof(QcPrtclReply));
	QcPrtclRegister* prtclResiter = prtcl_buff + sizeof(QcPrtclRegister);
	strcpy(consumerHdl->qname, prtclResiter->qname);


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

			QcQueue *queue = qc_qsys_queue_get(consumerHdl->qSystem, consumerHdl->qname, err);
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
