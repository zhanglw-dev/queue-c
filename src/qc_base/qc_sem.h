#ifndef QC_SEM_H
#define QC_SEM_H

#include "qc_error.h"

#define QC_SEMNAME_MAXLEN 256

typedef struct __QcSem__ QcSem;



QcSem* qc_sem_create(const char *name, int initcount, QcErr *err);

int qc_sem_destroy(QcSem *qcSem);

QcSem* qc_sem_open(const char *name, QcErr *err);

int qc_sem_close(QcSem *qcSem);

int qc_sem_wait(QcSem *qcSem, int wait_msec);

int qc_sem_post(QcSem *qcSem);


#endif //QC_SEM_H
