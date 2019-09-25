#ifndef QC_PUBLISH_H
#define QC_PUBLISH_H

#include "qc_message.h"
#include "qc_error.h"
#include "qc_mqueue.h"
#include "qc_qsystem.h"


struct __QcPublishHdl {
	QcQueue *queue;
};


struct __QcSubscribHdl {
	QcQueue *queue;
};


typedef struct __QcPublishHdl  QcPublishHdl;
typedef struct __QcSubscribHdl QcSubscribHdl;



QcPublishHdl* qc_publishhdl_create(QcQueue *queue, QcErr *err);

void qc_publishhdl_destory(QcPublishHdl *publishHdl);

int qc_publishhdl_msgput(QcPublishHdl *publishHdl, QcMessage *message, int msec, QcErr *err);


QcSubscribHdl* qc_subscribhdl_create(QcQueue *queue, QcErr *err);

void qc_subscribhdl_destory(QcSubscribHdl *subscribHdl);

QcMessage* qc_subscribhdl_msgget(QcSubscribHdl *subscribHdl, int msec, QcErr *err);

#endif /*QC_PUBLISH_H*/
