#ifndef QC_SERVICE_H
#define QC_SERVICE_H

#include "qc_list.h"
#include "qc_thread.h"
#include "qc_error.h"
#include "qc_qsystem.h"
#include "qc_netconf.h"


typedef struct __QcQueueSvc QcQueueSvc;


QcQueueSvc* qc_queuesvc_create(QcErr *err);

QcQueueSvc* qc_queuesvc_create_ex(QcNetConfig *netConf, QcQSystem *qSystem, QcErr *err);

void qc_queuesvc_destory(QcQueueSvc *queueSvc);

int qc_queuesvc_start(QcQueueSvc *queueSvc, QcErr *err);

void qc_queuesvc_stop(QcQueueSvc *queueSvc);


#endif /*QC_SERVICE_H*/
