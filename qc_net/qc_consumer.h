#ifndef QC_CONSUME_HDL_H
#define QC_CONSUME_HDL_H

#include "qc_message.h"
#include "qc_error.h"
#include "qc_socket.h"
#include "qc_mqueue.h"
#include "qc_qsystem.h"



typedef struct {
	QcSocket *socket;
	char qname[32];
	QcQSystem *qSystem;
}QcConsumerHdl;


int qc_proc_consumer(QcConsumerHdl *consumerHdl, char *prtcl_buff, QcErr *err);



#endif /*QC_CONSUME_HDL_H*/
