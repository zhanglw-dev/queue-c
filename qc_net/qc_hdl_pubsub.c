#include "qc_hdl_pubsub.h"



QcPublishHdl* qc_publishhdl_create(QcQueue *queue, QcErr *err)
{
	QcPublishHdl *publishHdl;
	publishHdl = (QcPublishHdl*)malloc(sizeof(QcPublishHdl));
	publishHdl->queue = queue;
	return publishHdl;
}


void qc_publishhdl_destory(QcPublishHdl *publishHdl)
{
	qc_free(publishHdl);
}


int qc_publishhdl_msgput(QcPublishHdl *publishHdl, QcMessage *message, int msec, QcErr *err)
{
	int ret;
	ret = qc_queue_msgput(publishHdl->queue, message, msec, err);
	if (0 != ret)
		return -1;
	return 0;
}


QcSubscribHdl* qc_subscribhdl_create(QcQueue *queue, QcErr *err)
{
	QcSubscribHdl *subscribHdl;
	subscribHdl = (QcSubscribHdl*)malloc(sizeof(QcSubscribHdl));
	subscribHdl->queue = queue;
	return subscribHdl;
}


void qc_subscribhdl_destory(QcSubscribHdl *subscribHdl)
{
	qc_free(subscribHdl);
}


QcMessage* qc_subscribhdl_msgget(QcSubscribHdl *subscribHdl, int msec, QcErr *err)
{
	QcMessage* message = qc_queue_msgget(subscribHdl->queue, msec, err);
	return message;
}
