#ifndef QC_SHM_QUEUE_H
#define QC_SHM_QUEUE_H

#include "qc_prelude.h"
#include "qc_error.h"
#include "shmq_def.h"
#include "shmq_config.h"


#define QUEDATA_SAFE_MARGIN_SIZE 512



int shm_queue_init(char* shm_addr, off_t *offset, QueConf *queConf, int que_num, int sn, QcErr *err);

P_ShmQue* shm_queue_find(char* shm_addr, const char *que_name, QcErr *err);

//int shmque_release(P_ShmQue* p_shmQue, QcErr *err);

int shm_queue_get(char* shm_addr, P_ShmQue *p_shmQue, int wait_msec, int *idx, char **pp_buff, int *p_bufflen, QcErr *err);

int shm_queue_free(char* shm_addr, P_ShmQue *p_shmQue, int idx, QcErr *err);

//pre alloc from que for put
int shm_queue_alloc(char* shm_addr, P_ShmQue *p_shmQue, int *idx, char **pp_buff, int *p_bufflen, QcErr *err);

int shm_queue_put(char* shm_addr, P_ShmQue *p_shmQue, int idx, char *buff, int bufflen, QcErr *err);

#endif //QC_SHM_QUEUE_H