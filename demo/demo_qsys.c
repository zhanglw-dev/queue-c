#include "demo_qsys.h"
#include "qc_qsystem.h"
#include "qc_message.h"


int demo_qsys()
{
    QcErr err;

    QcQueue *queue = qc_queue_create(1000, 10, &err);
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

    char *buff = "hello qsys!";
    QcMessage *message_put = qc_message_create(buff, strlen(buff)+1, 0);

    if( 0!= qc_qsys_putmsg(qSys, qname, message_put, 0, &err)){
        printf("qsys put msg failed: %s\n", err.desc);
        exit(-1);
    }

    QcMessage *message_got = qc_qsys_getmsg(qSys, qname, 0, &err);
    if(NULL == message_got){
        printf("qsys get msg failed: %s\n", err.desc);
        exit(-1);
    }

    printf("got message: buff=%s\n", qc_message_buff(message_got));

    qc_message_release(message_got, 0);

	qc_qsys_destory(qSys);

    printf("qc qsys is ok!\n");
	return 0;
}
