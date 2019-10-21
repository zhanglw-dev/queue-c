#ifndef QC_HDL_QUEUE_H
#define QC_HDL_QUEUE_H

#include "qc_message.h"
#include "qc_error.h"
#include "qc_socket.h"
#include "qc_queue.h"
#include "qc_qsystem.h"



typedef struct {
	QcSocket *socket;
	char qname[32];
	QcQSystem *qSystem;
}QcProducerHdl;



int qc_proc_producer(QcProducerHdl *producerProc, char *prtcl_buff, QcErr *err);


#endif /*QC_HDL_QUEUE_H*/
