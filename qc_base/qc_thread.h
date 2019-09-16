#ifndef QCLIB_THREAD_H
#define QCLIB_THREAD_H


#include "qc_prelude.h"


typedef struct __QcThread QcThread;
typedef struct __QcThreadKey QcThreadKey;
typedef struct __QcMutex QcMutex;
typedef struct __QcCondLock QcCondLock;
typedef struct __QcCond  QcCond;
typedef struct __QcRWLock QcRWLock;


#define QCTHREAD_CANCELED PTHREAD_CANCELED

typedef void *(*qc_thread_routine)(void *);
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

QcCond* qc_thread_cond_create();

int qc_thread_cond_destroy(QcCond *cond);

int qc_thread_cond_wait(QcCond *cond, QcCondLock *condlock);

int qc_thread_cond_timedwait(QcCond *cond, QcCondLock *condlock, int sec);

int qc_thread_cond_signal(QcCond *cond);

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
