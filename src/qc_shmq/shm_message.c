#include "shmq_def.h"
#include "shm_message.h"
#include "shm_queue.h"


struct _ShmMessage_{
    ShmQHdl *shmQHdl;
    int idx;
    char *p_buff;
    int bufflen;
};


ShmMessage* shm_message_create(ShmQHdl *shmQHdl)
{
    ShmMessage *message;
    message = malloc(sizeof(ShmMessage));
    message->shmQHdl = shmQHdl;
    return message;
}


ShmMessage* shm_message_alloc(ShmQHdl *shmQHdl, int bufflen, QcErr *err)
{
    int ret;
    ShmMessage *message;
    int idx;
    char *p_buff;
    int buf_len;

    ret = shm_queue_alloc(shmQHdl->shm_addr, shmQHdl->p_shmQue, &idx, &p_buff, &buf_len, err);
    if(ret<0)
    {
        return NULL;
    }

    message = malloc(sizeof(ShmMessage));
    message->idx = idx;
    message->p_buff = p_buff;

    if(bufflen > buf_len)
    {
        qc_seterr(err, -1, "illegal msg_size(%d).", bufflen);
        return NULL;
    }

    message->bufflen = bufflen;
    message->shmQHdl = shmQHdl;

    return message;
}


void shm_message_free(ShmMessage *message)
{
    ShmQHdl *shmQHdl;
    shmQHdl = message->shmQHdl;
    shm_queue_free(shmQHdl->shm_addr, shmQHdl->p_shmQue, message->idx, NULL);
}


char* shm_message_getbuff(ShmMessage *message)
{
    return message->p_buff;
}


int shm_message_setbuff(ShmMessage *message, char *buff, int len)
{
    message->p_buff = buff;
    message->bufflen = len;
    return 0;
}


int shm_message_getlen(ShmMessage *message)
{
    return message->bufflen;
}


int* _shm_message_idx(ShmMessage *message)
{
    return &message->idx;
}
