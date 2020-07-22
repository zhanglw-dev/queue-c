#ifndef QC_SEM_H
#define QC_SEM_H

#include "qc_error.h"

typedef struct __QcSem__ QcSem;



QcSem* qc_sem_create(const char *name, int initcount, int maxcount, QcErr *err);

int qc_sem_delete(QcSem *sem);

QcSem* qc_sem_open(const char *name, QcErr *err);

int qc_sem_close(QcSem *sem);

int qc_sem_wait(QcSem *sem, int wait_msec);

int qc_sem_post(QcSem *sem);


#endif //QC_SEM_H
