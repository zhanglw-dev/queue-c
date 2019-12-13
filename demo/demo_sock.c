#include "demo_sock.h"
#include "qc_qsystem.h"
#include "qc_message.h"
#include "qc_service.h"
#include "qc_client.h"


int demo_sock()
{
    QcErr err;

    QcQueue *queue = qc_queue_create(1000, 3, &err);
    if (!queue) {
        printf("create queue failed: %s\n", err.desc);
        exit(-1);
    }

	QcQSystem *qSys = qc_qsys_create();
	if (!qSys) {
		printf("create qsystem failed.\n");
		exit(-1);
	}

	char *qname = "queue01";
    if (0 != qc_qsys_addqueue(qSys, qname, queue, &err)) {
        printf("add queue to qsys failed: %s\n", err.desc);
        exit(-1);
    }

    QcQueueSvc *queueSvc = qc_queuesvc_create("127.0.0.1", 5555, qSys, &err);
    if(!queueSvc){
        printf("create queuesvc failed.\n");
        exit(-1);
    }

    if(0 != qc_queuesvc_start(queueSvc, 1, &err)){
        printf("start queuesvc failed.\n");
        exit(-1);
    }

    QcClient *client = qc_client_connect("127.0.0.1", 5555, &err);
    if(!client){
        printf("client connect failed.\n");
        exit(-1);
    }

    char *buff = "hello qc_sock!";
    QcMessage *message_put = qc_message_create(buff, (int)strlen(buff)+1, 0);
    if(0 != qc_client_msgput(client, "queue01", message_put, 3, &err)){
        printf("client msgput failed.\n");
        exit(-1);
    }

    QcMessage *message_get = qc_client_msgget(client, "queue01", 3, &err);
    if(!message_get){
        printf("client msgget failed.\n");
        exit(-1);
    }

    char *buff_got = qc_message_buff(message_get);
    if(0 != strcmp(buff_got, buff)){
        printf("message get error.\n");
        exit(-1);
    }

    qc_client_disconnect(client);
    qc_queuesvc_stop(queueSvc);
    qc_queuesvc_destory(queueSvc);

    printf("qc sock is ok!\n");

    return 0;
}
