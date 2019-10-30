#include "qc_qsystem.h"
#include "qc_queue.h"
#include "qc_service.h"



int test_net()
{
	QcErr err;
	int ret;

	QcQueue *queue = qc_queue_create(1000, 10, &err);
	if (!queue) {
		printf("create queue failed.\n");
		return -1;
	}

	QcQSystem *qSys = qc_qsys_create();
	if (!qSys) {
		printf("create qsystem failed.\n");
		return -1;
	}

	ret = qc_qsys_queue_add(qSys, "queue1", queue, &err);
	if (0 != ret) {
		printf("add queue to qsys failed.\n");
		return -1;
	}
	
	QcQueueSvc* queueSvc = qc_queuesvc_create("127.0.0.1", 5555, qSys, &err);
	if (!queueSvc) {
		printf("create queue service failed.\n");
		return -1;
	}

	ret = qc_queuesvc_start(queueSvc, &err);
	if (0 != ret) {
		printf("queue service start.\n");
		return -1;
	}



	//qc_queuesvc_stop(queueSvc);

	qc_queuesvc_destory(queueSvc);
	qc_qsys_destory(qSys);
	qc_queue_destroy(queue);

	return 0;
}



int main(int argc, char **argv)
{
	int ret;

	ret = test_net();
	if (0 != ret) {
		printf("net test failed.");
		exit(-1);
	}

	exit(0);
}
