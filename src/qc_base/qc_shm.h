#ifndef QC_SHM_H
#define QC_SHM_H

#include "qc_error.h"

#define QC_SHMNAME_MAXLEN 256

typedef struct __QcShm__ QcShm;



QcShm* qc_shm_create(const char *name, size_t shmsize, QcErr *err);

int qc_shm_destroy(QcShm *qcShm);

QcShm* qc_shm_open(const char *name, QcErr *err);

int qc_shm_close(QcShm *qcShm);

size_t qc_shm_getsize(QcShm *qcShm);

char* qc_shm_getaddr(QcShm *qcShm);


#endif //QC_SHM_H
