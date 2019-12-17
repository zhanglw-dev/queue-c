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

#ifndef QCLIB_THREAD_H
#define QCLIB_THREAD_H


typedef struct __QcThread QcThread;
typedef struct __QcThreadKey QcThreadKey;
typedef struct __QcMutex QcMutex;
typedef struct __QcCondLock QcCondLock;
typedef struct __QcCondition  QcCondition;
typedef struct __QcRWLock QcRWLock;


#define QCTHREAD_CANCELED PTHREAD_CANCELED

typedef void* (*qc_thread_routine)(void *);
typedef void (*qc_destruct_routine)(void *);




#ifdef __cplusplus
extern "C" {
#endif


QcThread* qc_thread_create(qc_thread_routine routine, void *routine_arg);

int qc_thread_cancel(QcThread *thread);

void qc_thread_exit(int exitcode);

int qc_thread_join(QcThread *thread, int *p_exitcode);

/*------------------------------------------------------------------------------*/

QcThreadKey* qc_thread_key_create();

int qc_thread_key_delete(QcThreadKey *threadkey);

int qc_thread_setspecific(QcThreadKey *threadkey, const void *value);

void* qc_thread_getspecific(QcThreadKey *threadkey);

/*------------------------------------------------------------------------------*/

QcMutex* qc_thread_mutex_create();

int qc_thread_mutex_destroy(QcMutex *mutex);

int qc_thread_mutex_lock(QcMutex *mutex);

int qc_thread_mutex_unlock(QcMutex *mutex);

/*------------------------------------------------------------------------------*/

QcCondLock* qc_thread_condlock_create();

int qc_thread_condlock_destroy(QcCondLock *condlock);

int qc_thread_condlock_lock(QcCondLock *condlock);

int qc_thread_condlock_unlock(QcCondLock *condlock);

/*------------------------------------------------------------------------------*/

QcCondition* qc_thread_condition_create();

int qc_thread_condition_destroy(QcCondition *cond);

int qc_thread_condition_wait(QcCondition *cond, QcCondLock *condlock);

int qc_thread_condition_timedwait(QcCondition *cond, QcCondLock *condlock, int msec);

int qc_thread_condition_signal(QcCondition *cond);

/*------------------------------------------------------------------------------*/

QcRWLock* qc_thread_rwlock_create();

int qc_thread_rwlock_destroy(QcRWLock *rwlock);

int qc_thread_rdlock_lock(QcRWLock *rwlock);

int qc_thread_rdlock_unlock(QcRWLock *rwlock);

int qc_thread_wrlock_lock(QcRWLock *rwlock);

int qc_thread_wrlock_unlock(QcRWLock *rwlock);


#ifdef __cplusplus
}
#endif


#endif /*QCLIB_THREAD_H*/
