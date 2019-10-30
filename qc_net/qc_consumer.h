#ifndef QC_CONSUME_HDL_H
#define QC_CONSUME_HDL_H

#include "qc_message.h"
#include "qc_error.h"
#include "qc_socket.h"
#include "qc_queue.h"
#include "qc_qsystem.h"
#include "qc_protocol.h"



typedef struct {
	QcSocket *socket;
	char qname[32];
	QcQSystem *qSystem;
}QcConsumerHdl;


int qc_proc_consumer(QcConsumerHdl *consumerHdl, QcPrtclHead *prtclHead, char *prtcl_buff, QcErr *err);



#endif /*QC_CONSUME_HDL_H*/
