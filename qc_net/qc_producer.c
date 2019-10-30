#include "qc_producer.h"
#include "qc_socket.h"
#include "qc_qsystem.h"
#include "qc_protocol.h"



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

		if (prtclHead->subtype != QC_TYPE_MSGPUT) {
			QcPrtclProduce* prtclProduce = (QcPrtclProduce*)body_buff;
			qc_prtcl_produce_ntoh(prtclProduce);

			unsigned short msg_prioriy = prtclProduce->msg_prioriy;
			int wait_msec = prtclProduce->wait_msec;
			unsigned int msg_len = prtclProduce->msg_len;

			QcQueue *queue = qc_qsys_queue_get(producerHdl->qSystem, producerHdl->qname, err);
			if (!queue)
				goto failed;

			char *buff = head_buff + sizeof(QcPrtclProduce);
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
