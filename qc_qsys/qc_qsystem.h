#ifndef QC_QSYSTEM_H
#define QC_QSYSTEM_H

#include "qc_qmanager.h"
#include "qc_queue.h"
#include "qc_error.h"


typedef struct __QcQSystem QcQSystem;



QcQSystem* qc_qsys_create();

//QcQSystem* qc_qsys_init_byconfig(const char* config_file, QcErr *err);

void qc_qsys_destory(QcQSystem *qSys);

int qc_qsys_queue_add(QcQSystem *qSys, const char *qname, QcQueue *queue, QcErr *err);

QcQueue* qc_qsys_queue_get(QcQSystem *qSys, const char *qname, QcErr *err);



#endif //QC_QSYSTEM_H
