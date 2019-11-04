#include "test_qsys.h"
#include "qc_qsystem.h"
#include "qc_queue.h"



int test_qsys()
{
	QcErr err;
	int ret;

	QcQSystem *qSys = qc_qsys_create();
	if (!qSys) {
		printf("create qsystem failed.\n");
		return -1;
	}

	char qname[20];

	for (int i = 0; i < 100; i++)
	{
		QcQueue *queue = qc_queue_create(1000, 10, &err);
		if (!queue) {
			printf("create queue failed.\n");
			return -1;
		}

		memset(qname, 0, sizeof(qname));
		sprintf(qname, "queue-%d", i);

		ret = qc_qsys_addqueue(qSys, qname, queue, &err);
		if (0 != ret) {
			printf("add queue to qsys failed.\n");
			return -1;
		}
	}

	for (int i = 0; i < 100; i++)
	{
		memset(qname, 0, sizeof(qname));
		sprintf(qname, "queue-%d", i);
		/*
		QcQueue *queue = qc_qsys_queue_get(qSys, qname, &err);
		if (!queue) {
			printf("add queue to qsys failed.\n");
			return -1;
		}
		*/
	}

	printf("qsys test ok!\n");
	qc_qsys_destory(qSys);
	return 0;
}
