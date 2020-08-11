#include "qc_prelude.h"
#include "shmque_api.h"



int main()
{
    int ret;
    QcErr err;
    ShmQHdl* shmQHdl;
    ShmMessage *message;

    shmQHdl = shm_queue_attach("shm_1", "queue_2", &err);
    if(NULL == shmQHdl)
    {
        printf("shmque_attach failed, err=%s.\n", err.desc);
        exit(-1);
    }

    ret = shm_queue_getmsg(shmQHdl, &message, -1, &err);
    if(ret < 0)
    {
        printf("get msg failed, err=%s\n",err.desc);
        exit(-1);
    }

    printf("got message succeed, len=%d, msg=%s\n", shm_message_getlen(message), shm_message_getbuff(message));

    shm_message_free(message);
    shm_queue_deattach(shmQHdl);

    exit(0);
}
