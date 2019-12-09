#ifndef QC_SERVICE_H
#define QC_SERVICE_H

#include "qc_error.h"
#include "qc_qsystem.h"


typedef struct __QcQueueSvc QcQueueSvc;


QcQueueSvc* qc_queuesvc_create(const char *ip, int port, QcQSystem *qSystem, QcErr *err);

void qc_queuesvc_destory(QcQueueSvc *queueSvc);

int qc_queuesvc_start(QcQueueSvc *queueSvc, int is_async, QcErr *err);

void qc_queuesvc_stop(QcQueueSvc *queueSvc);


#endif /*QC_SERVICE_H*/
