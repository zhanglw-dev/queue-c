#include "test_sem.h"

#include "qc_sem.h"


int test_sem()
{
	int ret;
	QcErr err;
	QcSem *qcSem1;
	QcSem *qcSem2;

	qcSem1 = qc_sem_create("test_sem_1", 1, &err);
	if (NULL == qcSem1)
	{
		printf("sem create failed.\n");
		return -1;
	}

	qcSem2 = qc_sem_open("test_sem_1", &err);
	if (NULL == qcSem2)
	{
		printf("sem open failed.\n");
		return -1;
	}

	ret = qc_sem_wait(qcSem1, -1);
	if (-1 == ret)
	{
		printf("sem wait failed.\n");
		return -1;
	}

	ret = qc_sem_wait(qcSem2, 1000);
	if (-1 == ret)
	{
		printf("sem wait failed.\n");
		return -1;
	}
	printf("sem wait kik 1s.\n");

	ret = qc_sem_post(qcSem1);
	if (-1 == ret)
	{
		printf("sem post failed.\n");
		return -1;
	}

	ret = qc_sem_wait(qcSem2, -1);
	if (-1 == ret)
	{
		printf("sem wait failed.\n");
		return -1;
	}

	ret = qc_sem_wait(qcSem1, 1000);
	if (-1 == ret)
	{
		printf("sem wait failed.\n");
		return -1;
	}
	printf("sem wait kik 1s.\n");

	ret = qc_sem_post(qcSem2);
	if (-1 == ret)
	{
		printf("sem post failed.\n");
		return -1;
	}

	qc_sem_close(qcSem2);
	qc_sem_destroy(qcSem1);

	printf("test sem ok.\n");
	return 0;
}
