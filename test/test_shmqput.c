#include "qc_prelude.h"
#include "shmque_api.h"



int main()
{
    int ret;
    QcErr err;
    ShmQHdl *shmQHdl;
    ShmMessage *message;
    char *buff = "hello world!";
    
    shmQHdl = shm_queue_attach("shm_1", "queue_2", &err);
    if(NULL == shmQHdl)
    {
        printf("shmque_attach failed, err=%s.\n", err.desc);
        exit(-1);
    }

    message = shm_message_alloc(shmQHdl, strlen(buff), &err);
    if(NULL == message)
    {
        printf("shm_message_alloc failed.\n");
        exit(-1);
    }

    ret = shm_message_setbuff(message, buff, strlen(buff));
    if(ret < 0)
    {
        printf("shm_message_setbuff failed.\n");
        exit(-1);
    }

    ret = shm_queue_putmsg(shmQHdl, message, &err);
    if(ret < 0)
    {
        printf("shmque_putmsg failed, err=%s.\n", err.desc);
        exit(-1);
    }

    printf("put message succeed, len=%d\n", shm_message_getlen(message));

    shm_message_free(message);
    shm_queue_deattach(shmQHdl);

    exit(0);
}
