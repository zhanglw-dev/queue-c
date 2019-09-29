#ifndef QC_CONSUME_HDL_H
#define QC_CONSUME_HDL_H

#include "qc_message.h"
#include "qc_error.h"
#include "qc_socket.h"
#include "qc_mqueue.h"
#include "qc_qsystem.h"

/*
struct __QcConsumeHdl {
	QcQSystem *qSystem;
	char qname[32];
};


typedef struct __QcConsumeHdl QcConsumeHdl;



QcConsumeHdl* qc_consumehdl_create(QcQSystem *qSystem, const char* qname, QcErr *err);

void qc_consumehdl_destory(QcConsumeHdl *consumeHdl);

int qc_consumehdl_get(QcConsumeHdl *consumeHdl, char *prtcl_body, int body_len, char **out_buff, int *bufflen, QcErr *err);
*/


typedef struct {
	QcSocket *socket;
	char qname[32];
	QcQSystem *qSystem;
}QcConsumerProc;


int qc_proc_consumer(QcConsumerProc *consumerProc, char *prtcl_buff, QcErr *err);



#endif /*QC_CONSUME_HDL_H*/
