#include "qc_queue.h"
#include "qc_message.h"


int demo_mq()
{
    QcErr err;

    QcQueue* queue = qc_queue_create(1000, 3, &err);
    if(NULL == queue){
        printf("create queue failed: %s", err.desc);
        exit(-1);
    }

    int len = 256;
    for(int i=0; i<10; i++){
        char *buff = (char*)malloc(len);
        memset(buff, 0, len);
        sprintf(buff, "hello queue-c [%d]", i);

        QcMessage *message = qc_message_create(buff, len, 0);

        if(0 != qc_queue_msgput(queue, message, -1, &err)){
            printf("put msg failed: %s\n", err.desc);
            exit(-1);
        }
    }

    for(int i=0; i<10; i++){
        QcMessage *message = qc_queue_msgget(queue, -1, &err);
        if(NULL == message){
            printf("get msg failed: %s\n", err.desc);
            exit(-1);
        }

        printf("got msg: %s [length:%d]\n", qc_message_buff(message), qc_message_bufflen(message));
        qc_message_release(message, 1);
    }

    exit(0);
}
