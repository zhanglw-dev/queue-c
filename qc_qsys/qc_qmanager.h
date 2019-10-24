#ifndef QC_QMANAGER_H
#define QC_QMANAGER_H

#include "qc_queue.h"
#include "qc_error.h"
#include "qc_hash.h"


typedef struct __QcQueManager QcQueManager;



QcQueManager* qc_qmng_create();

void qc_qmng_destory(QcQueManager *qManager);

int qc_qmng_addque(QcQueManager *qManager, const char *qname, QcQueue* queue, QcErr *err);

int qc_qmng_removeque(QcQueManager *qManager, const char *qname, QcErr *err);

QcQueue* qc_qmng_getque(QcQueManager *qManager, const char *qname, QcErr *err);



#endif //QC_QMANAGER_H
