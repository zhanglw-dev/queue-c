/*
 * BSD 3-Clause License
 * 
 * Copyright (c) 2019, zhanglw (zhanglw366@163.com)
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
#include "qc_thread.h"
#include "qc_log.h"


struct __QcThread{
    pthread_t thread_t;
};


struct __QcThreadKey{
    pthread_key_t key_t;
};


struct __QcMutex{
    pthread_mutex_t mutex_t;
};


struct __QcCondLock{
    pthread_mutex_t mutex_t;
};


struct __QcCondition{
    pthread_cond_t cond_t;
};


struct __QcRWLock{
    pthread_rwlock_t rwlock_t;
};


/*------------------------------------------------------------------------------------------*/

QcThread* qc_thread_create(qc_thread_routine routine, void *routine_arg)
{
    QcThread *thread;

    qc_assert(routine);

    qc_malloc(thread, sizeof(QcThread));
    if(NULL == thread)
    {
        return NULL;
    }

create:

    if(0 != pthread_create(&thread->thread_t, NULL, routine, routine_arg))
    {
        if(EAGAIN == errno)
        {
            goto create;
        }

        qc_error("create thread failed");
        qc_free(thread);
        return NULL;
    }

    return thread;
}


int qc_thread_cancel(QcThread *thread)
{
    return pthread_cancel(thread->thread_t);
}


void qc_thread_exit(int exitcode)
{
    /*in posix-thread, 'exitcode' is type (void*), here I want to pass exit code number*/
    pthread_exit((void*)((long)exitcode));
}


int qc_thread_join(QcThread *thread, int *p_exitcode)
{
    qc_assert(thread);

    /*in posix-thread, 'p_exitcode' is type (void**), here I want to get exit code number*/
    if(0 != pthread_join(thread->thread_t, (void**)&p_exitcode))
    {
        qc_error("thread join failed");
        return -1;
    }

    qc_free(thread);
    return 0;
}

/*------------------------------------------------------------------------------------------*/

QcThreadKey* qc_thread_key_create()
{
    QcThreadKey *threadkey;

    qc_malloc(threadkey, sizeof(QcThreadKey));
    if(NULL == threadkey)
    {
        return NULL;
    }

    if(0 != pthread_key_create(&threadkey->key_t, NULL))
    {
        qc_error("threadkey create failed");
        qc_free(threadkey);
        return NULL;
    }

    return threadkey;
}


int qc_thread_key_delete(QcThreadKey *threadkey)
{
    qc_assert(threadkey);

    if(0 != pthread_key_delete(threadkey->key_t))
    {
        qc_error("threadkey delete failed");
        return -1;
    }

    qc_free(threadkey);
    return 0;
}


int qc_thread_setspecific(QcThreadKey *threadkey, const void *value)
{
    qc_assert(threadkey);

    if(0 != pthread_setspecific(threadkey->key_t, value))
    {
        qc_error("thread_key setspecific failed");
        return -1;
    }

    return 0;
}


void* qc_thread_getspecific(QcThreadKey *threadkey)
{
    qc_assert(threadkey);

    return pthread_getspecific(threadkey->key_t);
}

/*------------------------------------------------------------------------------------------*/

QcMutex* qc_thread_mutex_create()
{
    QcMutex *mutex;

    qc_malloc(mutex, sizeof(QcMutex));
    if(NULL == mutex)
    {
        return NULL;
    }

    if(0 != pthread_mutex_init(&mutex->mutex_t, NULL))
    {
        qc_error("thread mutex create failed");
        qc_free(mutex);
        return NULL;
    }

    return mutex;
}


int qc_thread_mutex_destroy(QcMutex *mutex)
{
    qc_assert(mutex);

    if(0 != pthread_mutex_destroy(&mutex->mutex_t))
    {
        qc_error("thread mutex destroy failed");
        return -1;
    }

    qc_free(mutex);
    return 0;
}


int qc_thread_mutex_lock(QcMutex *mutex)
{
    qc_assert(mutex);

    if(0 != pthread_mutex_lock(&mutex->mutex_t))
    {
        qc_error("thread mutex lock failed");
        return -1;
    }

    return 0;
}


int qc_thread_mutex_unlock(QcMutex *mutex)
{
    qc_assert(mutex);

    if(0 != pthread_mutex_unlock(&mutex->mutex_t))
    {
        qc_error("thread mutex unlock failed");
        return -1;
    }

    return 0;
}

/*------------------------------------------------------------------------------------------*/

QcCondLock* qc_thread_condlock_create()
{
    QcCondLock *condlock;

    qc_malloc(condlock, sizeof(QcCondLock));
    if(NULL == condlock)
    {
        return NULL;
    }

    if(0 != pthread_mutex_init(&condlock->mutex_t, NULL))
    {
        qc_error("thread condlock create failed");
        qc_free(condlock);
        return NULL;
    }

    return condlock;
}


int qc_thread_condlock_destroy(QcCondLock *condlock)
{
    if(0 != pthread_mutex_destroy(&condlock->mutex_t))
    {
        qc_error("thread condlock destroy failed");
        return -1;
    }

    qc_free(condlock);
    return 0;
}


int qc_thread_condlock_lock(QcCondLock *condlock)
{
    if(0 != pthread_mutex_lock(&condlock->mutex_t))
    {
        qc_error("thread condlock lock failed");
        return -1;
    }

    return 0;
}


int qc_thread_condlock_unlock(QcCondLock *condlock)
{
    if(0 != pthread_mutex_unlock(&condlock->mutex_t))
    {
        qc_error("thread condlock unlock failed");
        return -1;
    }

    return 0;
}

/*------------------------------------------------------------------------------------------*/

QcCondition* qc_thread_condition_create()
{
    QcCondition *cond;

    qc_malloc(cond, sizeof(QcCondition));
    if(NULL == cond)
    {
        return NULL;
    }

    if(0 != pthread_cond_init(&cond->cond_t, NULL))
    {
        qc_error("thread cond create failed");
        qc_free(cond);
        return NULL;
    }

    return cond;
}


int qc_thread_condition_destroy(QcCondition *cond)
{
    if(0 != pthread_cond_destroy(&cond->cond_t))
    {
        qc_error("thread cond destroy failed");
        return -1;
    }

    qc_free(cond);
    return 0;
}


int qc_thread_condition_wait(QcCondition *cond, QcCondLock *condlock)
{
    if(0 != pthread_cond_wait(&cond->cond_t, &condlock->mutex_t))
    {
        qc_error("thread cond wait failed");
        return -1;
    }

    return 0;
}


int qc_thread_condition_timedwait(QcCondition *cond, QcCondLock *condlock, int sec)
{
    struct timespec timeout;
    timeout.tv_sec = sec;
    timeout.tv_nsec = 0;
    int ret;

    if(sec >= 0)
    {
    	  ret = pthread_cond_timedwait(&cond->cond_t, &condlock->mutex_t, &timeout);
    	  if (ETIMEDOUT == ret){
    	  	return QC_TIMEOUT;
    	  }
    	  
        if(0 != ret)
        {
            qc_perror("thread cond timewait(%d) failed", sec);
            return -1;
        }
    }
    else
    {
        if(0 != pthread_cond_wait(&cond->cond_t, &condlock->mutex_t))
        {
            qc_perror("thread cond timewait(%d) failed", sec);
            return -1;
        }
    }

    return 0;
}


int qc_thread_condition_signal(QcCondition *cond)
{
    if(0 != pthread_cond_signal(&cond->cond_t))
    {
        qc_error("thread cond signal failed");
        return -1;
    }

    return 0;
}

/*------------------------------------------------------------------------------------------*/

QcRWLock* qc_thread_rwlock_create()
{
    QcRWLock *rwlock;

    qc_malloc(rwlock, sizeof(QcRWLock));
    if(NULL == rwlock)
    {
        return NULL;
    }

    if(0 != pthread_rwlock_init(&rwlock->rwlock_t, NULL))
    {
        qc_error("thread rwlock create failed");
        qc_free(rwlock);
        return NULL;
    }

    return rwlock;
}


int qc_thread_rwlock_destroy(QcRWLock *rwlock)
{
    if(0 != pthread_rwlock_destroy(&rwlock->rwlock_t))
    {
        qc_error("thread rwlock destroy failed");
        return -1;
    }

    qc_free(rwlock);
    return 0;
}


int qc_thread_rdlock_lock(QcRWLock *rwlock)
{
    if(0 != pthread_rwlock_rdlock(&rwlock->rwlock_t))
    {
        qc_error("thread rwlock rd_lock failed");
        return -1;
    }

    return 0;
}


int qc_thread_rdlock_unlock(QcRWLock *rwlock)
{
    if(0 != pthread_rwlock_unlock(&rwlock->rwlock_t))
    {
        qc_error("thread rwlock rd_unlock failed");
        return -1;
    }

    return 0;
}


int qc_thread_wrlock_lock(QcRWLock *rwlock)
{
    if(0 != pthread_rwlock_wrlock(&rwlock->rwlock_t))
    {
        qc_error("thread rwlock wr_lock failed");
        return -1;
    }

    return 0;
}


int qc_thread_wrlock_unlock(QcRWLock *rwlock)
{
    if(0 != pthread_rwlock_unlock(&rwlock->rwlock_t))
    {
        qc_error("thread rwlock wr_unlock failed");
        return -1;
    }

    return 0;
}

