#include "qc_prelude.h"
#include "qc_sem.h"


#define QC_SEMNAME_MAXLEN 256


struct __QcSem__ {
	char name[QC_SEMNAME_MAXLEN];
	HANDLE semHdl;
};



QcSem* qc_sem_create(const char *name, int initcount, int maxcount, QcErr *err)
{
	HANDLE semHdl;
	QcSem *sem;

	if (strlen(name) > QC_SEMNAME_MAXLEN)
	{
		qc_seterr(err, -1, "parameter name (%s) is too long.", name);
		return NULL;
	}

	semHdl = CreateSemaphore(name, initcount, maxcount, NULL);
	if (NULL == semHdl)
	{
		qc_seterr(err, -1, "create semaphore (name : s%) call failed.", name);
		return NULL;
	}

	if (ERROR_ALREADY_EXISTS == GetLastError())
	{
		qc_seterr(err, -1, "semaphore (name : s%) is already existed.", name);
		return NULL;
	}

	qc_malloc(sem, QC_SEMNAME_MAXLEN+1);
	strncpy((char*)sem->name, name, QC_SEMNAME_MAXLEN);
	sem->semHdl = semHdl;

	return sem;
}


int qc_sem_delete(QcSem *sem)
{
	CloseHandle(sem->semHdl);
	qc_free(sem);
	return 0;
}


QcSem* qc_sem_open(const char *name, QcErr *err)
{
	HANDLE semHdl;
	QcSem *sem;

	if (strlen > QC_SEMNAME_MAXLEN)
	{
		qc_seterr(err, -1, "parameter name (%s) is too long.", name);
		return NULL;
	}

	semHdl = OpenSemaphore(SEMAPHORE_ALL_ACCESS, TRUE, name);
	if (NULL == semHdl)
	{
		qc_seterr(err, -1, "open semaphore (name : s%) call failed.", name);
		return NULL;
	}

	qc_malloc(sem, QC_SEMNAME_MAXLEN + 1);
	strncpy((char*)sem->name, name, QC_SEMNAME_MAXLEN);
	sem->semHdl = semHdl;

	return sem;
}


int qc_sem_close(QcSem *sem)
{
	CloseHandle(sem->semHdl);
	qc_free(sem);
	return 0;
}


int qc_sem_wait(QcSem *sem, int wait_msec)
{
	DWORD dwResult;

	if (wait_msec < 0)
	{
		WaitForSingleObject(sem->semHdl, INFINITE);
	}
	else
	{
		dwResult = WaitForSingleObject(sem->semHdl, (DWORD)wait_msec);
		if (WAIT_TIMEOUT == dwResult)
		{
			return QC_ERR_TIMEOUT;
		}
	}

	return 0;
}


int qc_sem_post(QcSem *sem)
{
	BOOL ret;
	ret = ReleaseSemaphore(sem->semHdl, 1, NULL);
	if (FALSE == ret)
		return -1;
	return 0;
}

