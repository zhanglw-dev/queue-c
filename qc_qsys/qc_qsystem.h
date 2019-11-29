#ifndef QC_QSYSTEM_H
#define QC_QSYSTEM_H

#include "qc_qmanager.h"
#include "qc_queue.h"
#include "qc_error.h"


typedef struct __QcQSystem QcQSystem;



QcQSystem* qc_qsys_create();

void qc_qsys_destory(QcQSystem *qSys);

int qc_qsys_addqueue(QcQSystem *qSys, const char *qname, QcQueue *queue, QcErr *err);

int qc_qsys_delqueue(QcQSystem *qSys, const char *qname, QcErr *err);

QcQueue* qc_qsys_queue_get(QcQSystem *qSys, const char *qname, QcErr *err);

int qc_qsys_putmsg(QcQSystem *qSys, const char *qname, QcMessage *message, int msec, QcErr *err);

QcMessage* qc_qsys_getmsg(QcQSystem *qSys, const char *qname, int msec, QcErr *err);


#endif //QC_QSYSTEM_H
