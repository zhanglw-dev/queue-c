#ifndef QC_QSYSTEM_H
#define QC_QSYSTEM_H

#include "qc_qmanager.h"
#include "qc_mqueue.h"
#include "qc_error.h"



typedef struct __QcQSystem QcQSystem;


extern QcQSystem *globalQSystem;


QcQSystem* qc_qsys_init();

QcQSystem* qc_qsys_init_byconfig(const char* config_file, QcErr *err);

void qc_qsys_release(QcQSystem *qSys);

QcQSystem* getQSystem();

int qc_qsys_queue_register(QcQSystem *qSys, const char *qname, QcQueue *queue, QcErr *err);

QcQueue* qc_qsys_queue_get(QcQSystem *qSys, const char *qname, QcErr *err);


#endif //QC_QSYSTEM_H
