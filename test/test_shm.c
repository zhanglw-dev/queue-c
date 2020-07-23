#include "test_shm.h"

#include "qc_shm.h"


int test_shm()
{
	QcErr err;
	QcShm *qcShm1;
	QcShm *qcShm2;


	qcShm1 = qc_shm_create("test_shm_1", 10 * 1024 * 1024, &err);
	if (NULL == qcShm1)
	{
		printf("create shm failed.\n");
		return -1;
	}

	char *addr1 = qc_shm_getaddr(qcShm1);
	strcpy(addr1, "123456");

	qcShm2 = qc_shm_open("test_shm_1", &err);
	if(NULL == qcShm2)
	{
		printf("open shm failed.\n");
		return -1;
	}

	char *addr2 = qc_shm_getaddr(qcShm2);

	if (0 != strcmp(addr2, "123456"))
	{
		printf("shm content compare failed.\n");
		return -1;
	}

	qc_shm_close(qcShm2);
	qc_shm_destroy(qcShm1);

	printf("test shm ok.\n");
	return 0;
}
