#ifndef QC_SHM_H
#define QC_SHM_H

#include "qc_error.h"


typedef struct __QcShm__ QcShm;


QcShm* qc_shm_create(const char *name, size_t shmsize, QcErr *err);

int qc_shm_destroy(QcShm *shm);

QcShm* qc_shm_open(const char *name, QcErr *err);

int qc_shm_close(QcShm *shm);

size_t qc_shm_getsize(QcShm *shm);

char* qc_shm_getaddr(QcShm *shm);


#endif //QC_SHM_H
