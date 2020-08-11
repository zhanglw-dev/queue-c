#ifndef QC_SHMQUE_API_H
#define QC_SHMQUE_API_H

#include "qc_error.h"
#include "shm_message.h"

typedef struct _ShmQHdl_ ShmQHdl;


#ifdef __cplusplus
extern "C" {
#endif

ShmQHdl* shm_queue_attach(const char *shm_name, const char *que_name, QcErr *err);

void shm_queue_deattach(ShmQHdl *ShmQHdl);

int shm_queue_putmsg(ShmQHdl *hdl, ShmMessage *message, QcErr *err);

int shm_queue_getmsg(ShmQHdl *shmQHdl, ShmMessage **pp_message, int wait_msec, QcErr *err);

#ifdef __cplusplus
}
#endif


#endif //QC_SHMQUE_API_H
