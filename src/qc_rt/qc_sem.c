/*
 * BSD 3-Clause License
 * 
 * Copyright (c) 2021, zhanglw (zhanglw366@163.com)
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "qc_prelude.h"
#include "qc_sem.h"


struct __QcSem__ {
	char name[QC_SEMNAME_MAXLEN + 1];
	sem_t* sem;
    int flags;
};


QcSem* qc_sem_create(const char *name, int initcount, QcErr *err)
{
    int flags;
    sem_t *sem;
    mode_t mode;

    flags = O_CREAT|O_EXCL|O_RDWR;
    mode  = S_IRUSR|S_IWUSR;

    //unlink first?!
    sem_unlink(name);

	if (strlen(name) > QC_SEMNAME_MAXLEN)
	{
		qc_seterr(err, -1, "parameter name (%s) is too long.", name);
		return NULL;
	}

    sem = sem_open(name, flags, mode, initcount);
    if(NULL == sem)
    {
        qc_seterr(err, -1, "create sem (name : %s) failed.", name);
        return NULL;
    }

    QcSem *qcSem;

    qc_malloc(qcSem, sizeof(QcSem));
    qcSem->sem = sem;
    strncpy(qcSem->name, name, QC_SEMNAME_MAXLEN);
    qcSem->flags = flags;

    return qcSem;
}


int qc_sem_destroy(QcSem *qcSem)
{
    sem_close(qcSem->sem);
    sem_unlink(qcSem->name);
    qc_free(qcSem);
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
    if(NULL == sem)
    {
        qc_seterr(err, -1, "open sem (name : %s) failed.", name);
        return NULL;
    }

    QcSem *qcSem;

    qc_malloc(qcSem, sizeof(QcSem));
    qcSem->sem = sem;
    strncpy(qcSem->name, name, QC_SEMNAME_MAXLEN);
    qcSem->flags = flags;

    return qcSem;
}


int qc_sem_close(QcSem *qcSem)
{
    sem_close(qcSem->sem);
    qc_free(qcSem);
    return 0;
}


int qc_sem_wait(QcSem *qcSem, int wait_msec)
{
    int ret;
    struct timespec ts;

    if(wait_msec < 0)
    {
        while ((ret = sem_wait(qcSem->sem)) == -1 && errno == EINTR)
            continue;       /* Restart if interrupted by handler */
        if(ret < 0)
            return -1;
    }
    else if(0 == wait_msec)
    {
        while ((ret = sem_trywait(qcSem->sem)) == -1 && errno == EINTR)
            continue;       /* Restart if interrupted by handler */
        if(ret < 0)
            return -1;
    }
    else
    {
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += wait_msec/1000;
        ts.tv_nsec += (wait_msec%1000)*1000000;

#ifdef IS_DARWIN   //since no sem_timedwait() in darwin
        int slp_int = 10; //10 millsec
        int slp_time = wait_msec/slp_int;

        while(1)
        {
            ret = sem_trywait(qcSem->sem);
            if(ret == -1 && errno == EINTR) continue;

            if(0 == ret) break;
            usleep(slp_int*1000);
            if((--slp_time)<0)
                return QC_ERR_TIMEOUT;
            //printf("sem_trywait slp_time : %d\n", slp_time);
        }
#else
        while ((ret = sem_timedwait(qcSem->sem, &ts)) == -1 && errno == EINTR)
            continue;       /* Restart if interrupted by handler */
        if(-1 == ret && errno == ETIMEDOUT)
        {
            return QC_ERR_TIMEOUT;
        }
        if(ret < 0)
            return -1;
#endif
    }

    return 0;
}


int qc_sem_post(QcSem *qcSem)
{
    sem_post(qcSem->sem);
    return 0;
}
