#ifndef QC_SHM_MESSAGE_H
#define QC_SHM_MESSAGE_H

#include "qc_error.h"


typedef struct _ShmMessage_ ShmMessage;
typedef struct _ShmQHdl_ ShmQHdl;


#ifdef __cplusplus
extern "C" {
#endif

ShmMessage* shm_message_create();

ShmMessage* shm_message_alloc(ShmQHdl *shmQHdl, int bufflen, QcErr *err);

void shm_message_free(ShmMessage *message);

char* shm_message_getbuff(ShmMessage *message);

int shm_message_getlen(ShmMessage *message);

int shm_message_setbuff(ShmMessage *message, char *buff, int len);

int* _shm_message_idx(ShmMessage *message);

#ifdef __cplusplus
}
#endif

#endif //QC_SHM_MESSAGE_H