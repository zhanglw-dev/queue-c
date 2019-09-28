#ifndef QC_HDL_QUEUE_H
#define QC_HDL_QUEUE_H

#include "qc_message.h"
#include "qc_error.h"
#include "qc_socket.h"
#include "qc_mqueue.h"
#include "qc_qsystem.h"


struct __QcProduceHdl {
	QcQSystem *qSystem;
	char qname[32];
};




typedef struct __QcProduceHdl QcProduceHdl;



QcProduceHdl* qc_producehdl_create(QcQSystem *qSystem, const char* qname, QcErr *err);

void qc_producehdl_destory(QcProduceHdl *produceHdl);

int qc_producehdl_put(QcProduceHdl *produceHdl, char *buff, int bufflen, int msg_priorty, int msec, QcErr *err);

//-------------------------------------------------------------------------------------------------



#endif /*QC_HDL_QUEUE_H*/
