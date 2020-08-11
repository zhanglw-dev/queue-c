#ifndef SHMQ_DEF_H
#define SHMQ_DEF_H

#include "qc_prelude.h"
#include "qc_list.h"

#define QUENAME_MAXLEN 32


#pragma pack(push)
#pragma pack(1)


typedef struct {
	int status;
	unsigned int quenum;
}P_Shmem;


typedef struct{
    char quename[QUENAME_MAXLEN];
    int que_sn;
    int que_size;
    int msg_size;

    sem_t sem;
    pthread_mutex_t mutex;

    QcStaticList lst_free;
	QcStaticList lst_used;

    off_t data_offset;
    off_t next_offset;
}P_ShmQue;


#pragma pack(pop)


typedef struct _ShmQHdl_{
    int que_sn;
    char *shm_addr;
    P_ShmQue *p_shmQue;
}ShmQHdl;


#endif //SHMQ_DEF_H