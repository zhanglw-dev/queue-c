#ifndef QC_SERVICE_H
#define QC_SERVICE_H

#include "qc_list.h"
#include "qc_thread.h"
#include "qc_error.h"
#include "qc_consume_hdl.h"
#include "qc_produce_hdl.h"


struct __QcQueueSvc {
	char svc_ip[256];
	int svc_port;

	QcThread *listenThread;
	QcList *workThreadList;

	QcQSystem *qSystem;
};


typedef struct __QcQueueSvc QcQueueSvc;


QcQueueSvc* qc_queuesvc_create(const char* ip, int port, QcQSystem *qSystem, QcErr *err);

void qc_queuesvc_destory(QcQueueSvc *queueSvc);

int qc_queuesvc_start(QcQueueSvc *queueSvc, QcErr *err);

void qc_queuesvc_stop(QcQueueSvc *queueSvc);


#endif /*QC_SERVICE_H*/
