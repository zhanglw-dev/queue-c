#ifndef QC_HDL_QUEUE_H
#define QC_HDL_QUEUE_H

#include "qc_message.h"
#include "qc_error.h"
#include "qc_socket.h"
#include "qc_mqueue.h"
#include "qc_qsystem.h"


struct __QcProduceHdl {
	QcQSystem *qSystem;
};

struct __QcConsumeHdl {
	QcQSystem *qSystem;
};


typedef struct __QcProduceHdl QcProduceHdl;
typedef struct __QcConsumeHdl QcConsumeHdl;



QcProduceHdl* qc_producehdl_create(QcQSystem *qSystem, QcErr *err);

void qc_producehdl_destory(QcProduceHdl *produceHdl);

int qc_producehdl_put(QcProduceHdl *produceHdl, const char* qname, QcMessage *message, int msec, QcErr *err);

//-------------------------------------------------------------------------------------------------

QcConsumeHdl* qc_consumehdl_create(QcQSystem *qSystem, QcErr *err);

void qc_consumehdl_destory(QcConsumeHdl *consumeHdl);

QcMessage* qc_consumehdl_get(QcConsumeHdl *consumeHdl, const char* qname, int msec, QcErr *err);


#endif /*QC_HDL_QUEUE_H*/
