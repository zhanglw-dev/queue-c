#include "qc_prelude.h"
#include "qc_sem.h"


struct __QcSem__ {
	char name[QC_SEMNAME_MAXLEN+1];
	sem_t sem;
};



QcSem* qc_sem_create(const char *name, int initcount, QcErr *err)
{
    int flags;
    sem_t *sem;
    mode_t mode;

    flags = O_CREAT|O_EXCL|O_RDWR;
    mode  = S_IRUSR|S_IWUSR;

	if (strlen(name) > QC_SEMNAME_MAXLEN)
	{
		qc_seterr(err, -1, "parameter name (%s) is too long.", name);
		return NULL;
	}

    sem = sem_open(name, flags, mode, initcount);
    if(0 != sem)
    {
        qc_seterr(err, -1, "create sem (name : %s) failed.", name);
        return NULL;
    }

    QcSem *qcSem;

    qc_malloc(qcSem, sizeof(QcSem));
    qcSem->sem = sem;
    strncpy(qcSem->name, name, QC_SEMNAME_MAXLEN);

    return qcSem;
}


int qc_sem_destroy(QcSem *qcSem)
{
    sem_close(qcSem->sem);
    sem_unlink(qcSem->name);
    return 0;
}


QcSem* qc_sem_open(const char *name, QcErr *err)
{
    int flags;
    sem_t *sem;

    flags = 0;  //just open

	if (strlen(name) > QC_SEMNAME_MAXLEN)
	{
		qc_seterr(err, -1, "parameter name (%s) is too long.", name);
		return NULL;
	}

    sem = sem_open(name, flags);
    if(0 != sem)
    {
        qc_seterr(err, -1, "create sem (name : %s) failed.", name);
        return NULL;
    }

    QcSem *qcSem;

    qc_malloc(qcSem, sizeof(QcSem));
    qcSem->sem = sem;
    strncpy(qcSem->name, name, QC_SEMNAME_MAXLEN);

    return qcSem;
}


int qc_sem_close(QcSem *qcSem)
{
    sem_close(qcSem->sem);
    return 0;
}


int qc_sem_wait(QcSem *qcSem, int wait_msec)
{
    int ret;
    struct timespec ts;

    if(wait_msec < 0)
    {
        sem_wait(qcSem->sem);
    }
    else if(0 == wait_msec)
    {
        ret = sem_trywait(qcSem->sem);
        if(-1 == ret && errno == EAGAIN)
        {
            return QC_ERR_TIMEOUT;
        }
        else
        {
            return -1;
        }
    }
    else
    {
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += msec/1000;
        ts.tv_nsec += (msec%1000)*1000000;

        ret = sem_timedwait(qcSem->sem, &ts);
        if(-1 == ret && errno == EAGAIN)
        {
            return QC_ERR_TIMEOUT;
        }
        else
        {
            return -1;
        }
    }

    return 0;
}


int qc_sem_post(QcSem *qcSem)
{
    sem_post(qcSem->sem);
    return 0;
}
