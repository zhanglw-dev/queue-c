#ifndef QC_CONSUME_HDL_H
#define QC_CONSUME_HDL_H

#include "qc_message.h"
#include "qc_error.h"
#include "qc_socket.h"
#include "qc_mqueue.h"
#include "qc_qsystem.h"


struct __QcConsumeHdl {
	QcQSystem *qSystem;
	char qname[32];
};


typedef struct __QcConsumeHdl QcConsumeHdl;



QcConsumeHdl* qc_consumehdl_create(QcQSystem *qSystem, QcErr *err);

void qc_consumehdl_destory(QcConsumeHdl *consumeHdl);

int qc_consumehdl_get(QcConsumeHdl *consumeHdl, char **buff, int *bufflen, int msec, QcErr *err);



#endif /*QC_CONSUME_HDL_H*/
