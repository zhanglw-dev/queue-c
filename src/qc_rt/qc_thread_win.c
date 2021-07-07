
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

#include "qc_thread.h"
#include "qc_prelude.h"


struct __QcThread{
	HANDLE handle;
	DWORD  id;
};

struct __QcThreadKey{
	DWORD key;
	void  *destructor;
};


struct __QcMutex{
	HANDLE handle;
};


struct __QcCondLock{
	CRITICAL_SECTION lock_hdl;
};


struct __QcCondition{
	CONDITION_VARIABLE cond_hdl;
};


struct __QcRWLock{
	SRWLOCK rwlock_hdl;
};



typedef  unsigned (__stdcall *PTHREAD_START)(void   *);
#define  chBEGINTHREADEX(psa, cbStack, pfnStartAddr,      \
                         pvParam, fdwCreate, pdwThreadId) \
         ((HANDLE)_beginthreadex(                         \
                  (void *)(psa),                          \
                  (unsigned)(cbStack),                    \
                  (PTHREAD_START)(pfnStartAddr),          \
                  (void *)(pvParam),                      \
                  (unsigned)(fdwCreate),                  \
                  (unsigned *)(pdwThreadId)))



QcThread* qc_thread_create(qc_thread_routine routine, void *routine_arg)
{
	QcThread* thread;

	thread = malloc(sizeof(QcThread));
	qc_assert(thread);

    if((thread->handle = chBEGINTHREADEX(NULL, 0, (PTHREAD_START_ROUTINE)routine, \
                          routine_arg, 0, &thread->id)) == 0)
	{
		free(thread);
		return NULL;
	}

	return thread;
}


int qc_thread_cancel(QcThread *thread)
{
	if(TRUE == TerminateThread(thread->handle, (int)QCTHREAD_CANCELED))
		return 0;
	else
		return -1;
}


void qc_thread_exit(int exitcode)
{
	_endthreadex((long)exitcode);
}


int qc_thread_join(QcThread *thread, int *p_exitcode)
{
	int rv;

	rv = WaitForSingleObject(thread->handle, INFINITE);
	switch(rv){
		case WAIT_OBJECT_0:
			break;
		case WAIT_FAILED:
			return -1;
		default:
			return -1;
	}

	if(NULL != p_exitcode)
	{
		if(!GetExitCodeThread(thread->handle, p_exitcode))
		{
			*p_exitcode = -1;
		}
	}

	free(thread);

	return 0;
}


/*--------------------------------------------------------------------------------------------------*/

QcThreadKey* qc_thread_key_create()
{
	QcThreadKey *threadKey;

	threadKey = malloc(sizeof(QcThreadKey));
	threadKey->key =TlsAlloc();

	if(TLS_OUT_OF_INDEXES == threadKey->key)
		return NULL;

	threadKey->destructor = NULL;

	return threadKey;
}


int qc_thread_key_delete(QcThreadKey *threadKey)
{
	void *value;
	qc_destruct_routine destruct_routine;

	value = TlsGetValue(threadKey->key);
	destruct_routine = threadKey->destructor;
	if(NULL != destruct_routine)
	{
		destruct_routine(value);
	}

	if(!TlsFree(threadKey->key))
		return -1;

	free(threadKey);
	return 0;
}


int qc_thread_setspecific(QcThreadKey *threadKey, const void *value)
{
	if(!TlsSetValue(threadKey->key, (void*)value))
		return -1;

	return 0;
}


void* qc_thread_getspecific(QcThreadKey *threadKey)
{
	void *value;

	value = TlsGetValue(threadKey->key);

	return value;
}

/*--------------------------------------------------------------------------------------------------*/


QcMutex* qc_thread_mutex_create()
{
	QcMutex *mutex;

	mutex = malloc(sizeof(QcMutex));
	qc_assert(mutex);

	mutex->handle = CreateMutex(0, FALSE, NULL);
	if(NULL == mutex->handle)
	{
		free(mutex);
		return NULL;
	}

	return mutex;
}


int qc_thread_mutex_destroy(QcMutex *mutex)
{
	if(!CloseHandle(mutex->handle))
		return -1;

	free(mutex);
	return 0;
}


int qc_thread_mutex_lock(QcMutex *mutex)
{
	if(WAIT_FAILED == WaitForSingleObject(mutex->handle, INFINITE))
		return -1;

	return 0;
}


int qc_thread_mutex_unlock(QcMutex *mutex)
{
	if(!ReleaseMutex(mutex->handle))
		return -1;

	return 0;
}

/*--------------------------------------------------------------------------------------------------*/

QcCondition* qc_thread_condition_create()
{
	QcCondition *cond;

	cond = malloc(sizeof(QcCondition));

	InitializeConditionVariable(&cond->cond_hdl);

	return cond;
}


int qc_thread_condition_destroy(QcCondition *cond)
{
	free(cond);

	return 0;
}


int qc_thread_condition_wait(QcCondition *cond, QcCondLock *condlock)
{
	if(!SleepConditionVariableCS(&cond->cond_hdl, &condlock->lock_hdl, INFINITE))
		return -1;

	return 0;
}


int qc_thread_condition_timedwait(QcCondition *cond, QcCondLock *condlock, int msec)
{
	if (!SleepConditionVariableCS(&cond->cond_hdl, &condlock->lock_hdl, msec)) {
		if (ERROR_TIMEOUT == GetLastError())
			return QC_TIMEOUT;
		return -1;
	}

	return 0;
}


int qc_thread_condition_signal(QcCondition *cond)
{
	WakeConditionVariable(&cond->cond_hdl);

	return 0;
}

/*--------------------------------------------------------------------------------------------------*/

QcCondLock* qc_thread_condlock_create()
{
	QcCondLock *condlock;

	condlock = malloc(sizeof(QcCondLock));
	qc_assert(condlock);

	InitializeCriticalSection(&condlock->lock_hdl);

	return condlock;
}


int qc_thread_condlock_destroy(QcCondLock *condlock)
{

	DeleteCriticalSection(&condlock->lock_hdl);

	free(condlock);
	return 0;
}


int qc_thread_condlock_lock(QcCondLock *condlock)
{
	EnterCriticalSection(&condlock->lock_hdl);
	return 0;
}


int qc_thread_condlock_unlock(QcCondLock *condlock)
{
	LeaveCriticalSection(&condlock->lock_hdl);
	return 0;
}

/*--------------------------------------------------------------------------------------------------*/

QcRWLock* qc_thread_rwlock_create()
{
	QcRWLock *rwlock;

	rwlock = malloc(sizeof(SRWLOCK));
	qc_assert(rwlock);

	InitializeSRWLock(&rwlock->rwlock_hdl);

	return rwlock;
}


int qc_thread_rwlock_destroy(QcRWLock *rwlock)
{
	free(rwlock);

	return 0;
}


int qc_thread_rdlock_lock(QcRWLock *rwlock)
{
	AcquireSRWLockShared(&rwlock->rwlock_hdl);

	return 0;
}


int qc_thread_rdlock_unlock(QcRWLock *rwlock)
{
	ReleaseSRWLockShared(&rwlock->rwlock_hdl);

	return 0;
}


int qc_thread_wrlock_lock(QcRWLock *rwlock)
{
	AcquireSRWLockExclusive(&rwlock->rwlock_hdl);

	return 0;
}


int qc_thread_wrlock_unlock(QcRWLock *rwlock)
{
	ReleaseSRWLockExclusive(&rwlock->rwlock_hdl);

	return 0;
}

/*--------------------------------------------------------------------------------------------------*/
