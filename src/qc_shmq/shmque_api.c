#include "shmque_api.h"
#include "shmq_def.h"
#include "shm_message.h"
#include "shm_queue.h"

#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>



ShmQHdl* shm_queue_attach(const char *shm_name, const char *que_name, QcErr *err)
{
    int fd;
    int que_sn;
    char *shm_addr;
    P_ShmQue *p_shmQue;
    struct stat sb;
    ShmQHdl *shmQHdl;
    off_t offset = 0;

    fd = shm_open(shm_name, O_RDWR, 0);
    if(fd == -1)
    {
        qc_seterr(err, -1, "shm_open failed, errno=%d\n", errno);
        return NULL;
    }

    if(fstat(fd, &sb) == -1)
    {
        qc_seterr(err, -1, "fstat failed, errno=%d\n", errno);
        return NULL;
    }

    shm_addr = mmap(NULL, sb.st_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if(shm_addr == MAP_FAILED)
    {
        fprintf(stderr, "mmap failed, errno=%d\n", errno);
        exit(-1);
    }

    close(fd);

    offset = sizeof(P_Shmem);

    p_shmQue = shm_queue_find(shm_addr, que_name, err);
    if(NULL == p_shmQue)
    {
        return NULL;
    }

    shmQHdl = (ShmQHdl*)malloc(sizeof(ShmQHdl));
    shmQHdl->shm_addr = shm_addr;
    shmQHdl->p_shmQue = p_shmQue;

    return shmQHdl;
}


void shm_queue_deattach(ShmQHdl *shmQHdl)
{
    qc_free(shmQHdl);
}


int shm_queue_putmsg(ShmQHdl *shmQHdl, ShmMessage *message, QcErr *err)
{
    int ret;

    ret = shm_queue_put(shmQHdl->shm_addr, shmQHdl->p_shmQue, *_shm_message_idx(message), shm_message_getbuff(message), shm_message_getlen(message), err);
    if(ret<0)
        return -1;

    return 0;
}


int shm_queue_getmsg(ShmQHdl *shmQHdl, ShmMessage **pp_message, int wait_msec, QcErr *err)
{
    int ret;
    int *idx;
    int index;
    ShmMessage *message;
    char *buff;
    int bufflen;

    message = shm_message_create(shmQHdl);

    ret = shm_queue_get(shmQHdl->shm_addr, shmQHdl->p_shmQue, wait_msec, &index, &buff, &bufflen, err);
    if(ret<0)
    {
        return -1;
    }

    shm_message_setbuff(message, buff, bufflen);
    idx = _shm_message_idx(message);
    *idx = index;
    *pp_message = message;

    return 0;
}
