#ifndef QC_HDL_QUEUE_H
#define QC_HDL_QUEUE_H

#include "qc_message.h"
#include "qc_error.h"
#include "qc_socket.h"
#include "qc_mqueue.h"


struct __QcProduceHdl {
	QcSocket *socket;
	QcQueue *queue;
};

struct __QcConsumeHdl {
	QcSocket *socket;
	QcQueue *queue;
};


typedef struct __QcProduceHdl QcProduceHdl;
typedef struct __QcConsumeHdl QcConsumeHdl;



QcProduceHdl* qc_producehdl_create(const char* ip, int port, const char* queue_name, QcErr *err);

void qc_producehdl_destory(QcProduceHdl *produceHdl);

int qc_producehdl_put(QcProduceHdl *produceHdl, QcMessage *message, int msec, QcErr *err);

//-------------------------------------------------------------------------------------------------

QcConsumeHdl* qc_consumehdl_create(const char* ip, int port, const char* queue_name, QcErr *err);

void qc_consumehdl_destory(QcConsumeHdl *consumeHdl);

QcMessage* qc_subscribhdl_msgget(QcConsumeHdl *consumeHdl, int msec, QcErr *err);


#endif /*QC_HDL_QUEUE_H*/
