#include "test_net.h"
#include "qc_service.h"
#include "qc_client.h"
#include "qc_message.h"
#include "qc_queue.h"
#include "qc_qsystem.h"


const char *msgstr = "123456789";

int test_net()
{
    int ret;
    QcErr err;

    QcQueue *queue = qc_queue_create(1000, 10, &err);
    if(!queue){
        printf("create queue failed.");
        return -1;
    }

    QcQSystem *qSystem = qc_qsys_create();
    ret = qc_qsys_addqueue(qSystem, "queue01", queue, &err);
    if(0 != ret){
        printf("add queue to qsys failed.");
        return -1;
    }

    QcQueueSvc *queueSvc = qc_queuesvc_create("127.0.0.1", 5555, qSystem, &err);
    if(!queueSvc){
        printf("create queuesvc failed.");
        return -1;
    }

    ret = qc_queuesvc_start(queueSvc, 1, &err);
    if(0 != ret){
        printf("start queuesvc failed");
        return -1;
    }

    QcClient* client = qc_client_connect("127.0.0.1", 5555, &err);
    if(!client){
        printf("client connect failed.");
        return -1;
    }

    QcMessage *message_put = qc_message_create(msgstr, (int)strlen(msgstr), 0);
    ret = qc_client_msgput(client, "queue01", message_put, 3, &err);
    if(0 != ret){
        printf("client msgput failed.");
        return -1;
    }

    QcMessage *message_get = qc_client_msgget(client, "queue01", 3, &err);
    if(!message_get){
        printf("client msgget failed");
        return -1;
    }

    char *buff = qc_message_buff(message_get);
    if(0 != strcmp(buff, msgstr)){
        printf("message get error");
        return -1;
    }

    qc_client_disconnect(client);

    qc_queuesvc_stop(queueSvc);
    qc_queuesvc_destory(queueSvc);

    return 0;
}
