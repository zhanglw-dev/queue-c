#include "qc_thread_ex.h"
#include "qc_log.h"



//////////////////////////////////////////////////////////////////////////////////
//                              Thread Group
//////////////////////////////////////////////////////////////////////////////////


struct __QcThreadGroup{
	int threadnum;
	QcThread **thread_arry;
};

/*------------------------------------------------------------------------------------------*/

QcThreadGroup* qc_threadgroup_create(int threadnum, qc_threadgroup_routine routine, void *routine_arg)
{
	QcThreadGroup *threadGroup;
	QcThread *thread;
	int i,j;

    qc_malloc(threadGroup, sizeof(QcThreadGroup));
    if(NULL == threadGroup)
	    return NULL;

	threadGroup->threadnum = threadnum;
	threadGroup->thread_arry = malloc(sizeof(QcThread*)*threadnum);
	if(NULL == threadGroup->thread_arry)
	{
		qc_error("malloc thread arry for threadgroup failed");
		qc_free(threadGroup);
		return NULL;
	}

	for(i=0; i<threadnum; i++)
	{
        thread = qc_thread_create(routine, routine_arg);
	    if(NULL == thread)
	    {
		    qc_error("create thread for threadgroup failed");
            for(j=0; j<i; j++)
            {
                qc_thread_cancel(threadGroup->thread_arry[j]);
            }
            qc_free(threadGroup);
		    return NULL;
	    }
            threadGroup->thread_arry[i] = thread;
	}

	return threadGroup;
}



int qc_threadgroup_destroy_cancelall(QcThreadGroup *threadGroup)
{
	int i, excode;

	for(i=0; i<threadGroup->threadnum; i++)
	{
        qc_thread_cancel(threadGroup->thread_arry[i]);
        qc_thread_join(threadGroup->thread_arry[i], &excode);
	}

	return 0;
}



int qc_threadgroup_destroy_joinall(QcThreadGroup *threadGroup)
{
	int i, excode, ret=0;

	for(i=0; i<threadGroup->threadnum; i++)
	{
		if(-1 == qc_thread_join(threadGroup->thread_arry[i], &excode))
		{
			qc_error("join thread from threadgroup failed");
			ret = -1;
		}
	}

	return ret;
}


//////////////////////////////////////////////////////////////////////////////////
//                              Thread Pool
//////////////////////////////////////////////////////////////////////////////////


typedef struct __threadobject__ ThreadObject;
typedef void *qc_threadpl_routine4hook(ThreadObject* thrdObject);



typedef struct{
	void *routine_arg;
	qc_threadpl_routine exec_routine;    /* Pointer of thread function */
	qc_threadpl_routine exit_routine;
}ThreadSwap;



struct __threadobject__{
	QcThreadPool *pool;
	QcThread *tid;			/* Thread handle */
	ThreadObject *next;
};



struct __qc_threadpool__{
	int thread_num;				   /* Total num of threads in thread pool */
	ThreadObject *threadObjects;	   /* Link-table for threads in pool */
	QcCondLock *condlock_ctrler;
	QcCondLock *condlock_worker;
	QcCond *cond_ctrler;
	QcCond *cond_worker;
	QcMutex *mutex_worker;
	int ctrler_waiting;
	int worker_waiting; 	          /* One or more threads are sleepping currently */
	ThreadSwap threadSwap;        /* Runtime parameter of current thread, for internal swap */
};


/*------------------------------------------------------------------------------------------*/

void* qc_threadpool_hookfunc(ThreadObject *thrdObject)
{
	int state = 0;
	ThreadSwap threadSwap;
	QcThreadPool *threadPool = thrdObject->pool;

	for(;;)
	{
	    if(0 == state)
		{
           		 state = 1;
		}
	   	else
		{
			if(threadSwap.exec_routine)
				threadSwap.exec_routine(threadSwap.routine_arg);
			if(threadSwap.exit_routine)
				threadSwap.exit_routine(threadSwap.routine_arg);
		}

		qc_thread_mutex_lock(threadPool->mutex_worker);

		qc_thread_condlock_lock(threadPool->condlock_ctrler);
		qc_thread_condlock_lock(threadPool->condlock_worker);
		qc_thread_condlock_unlock(threadPool->condlock_ctrler);

		if(0 == threadPool->ctrler_waiting)
		{
			threadPool->worker_waiting = 1;
			qc_thread_cond_wait(threadPool->cond_worker, threadPool->condlock_worker);
		}

		qc_thread_condlock_lock(threadPool->condlock_ctrler);
		qc_thread_cond_signal(threadPool->cond_ctrler);
		threadPool->ctrler_waiting = 0;

		/** get thread params */
		memcpy(&threadSwap, &threadPool->threadSwap, sizeof(ThreadSwap));

		qc_thread_condlock_unlock(threadPool->condlock_worker);
		qc_thread_condlock_unlock(threadPool->condlock_ctrler);

		qc_thread_mutex_unlock(threadPool->mutex_worker);
	}

	return NULL;
}



QcThreadPool* qc_threadpool_create(int thread_num)
{
	int i;
	ThreadObject *threadObject;
	QcThreadPool *threadPool;

	if(NULL == (threadPool = malloc(sizeof(QcThreadPool))))
		return NULL;
	memset(threadPool, 0, sizeof(QcThreadPool));

	threadPool->threadObjects = NULL;
	threadPool->ctrler_waiting = 0;
	threadPool->worker_waiting  = 0;

	if(NULL == (threadPool->cond_ctrler = qc_thread_cond_create()))
		return NULL;
	if(NULL == (threadPool->cond_worker = qc_thread_cond_create()))
		return NULL;
	if(NULL == (threadPool->condlock_ctrler = qc_thread_condlock_create()))
		return NULL;
	if(NULL == (threadPool->condlock_worker = qc_thread_condlock_create()))
		return NULL;
	if(NULL == (threadPool->mutex_worker = qc_thread_mutex_create()))
		return NULL;

	for(i=0; i<thread_num; i++)
	{
		threadObject = malloc(sizeof(ThreadObject));
		if (NULL == threadObject)
			return NULL;
		memset(threadObject, 0, sizeof(ThreadObject));

		/* Initialize thread */
		threadObject->pool = threadPool;

		/* Insert into link-talbe */
		threadObject->next = threadPool->threadObjects;
		threadPool->threadObjects = threadObject;

		threadObject->tid = qc_thread_create((qc_threadpl_routine)qc_threadpool_hookfunc, threadObject);
		if(NULL == threadObject->tid)
            return NULL;
	}

	threadPool->thread_num = thread_num;

	return threadPool;
}



int qc_threadpool_active(QcThreadPool *threadPool, qc_threadpl_routine exec_routine, qc_threadpl_routine exit_routine, void *routine_arg)
{
	qc_thread_condlock_lock(threadPool->condlock_ctrler);

	threadPool->threadSwap.exec_routine = exec_routine;
	threadPool->threadSwap.exit_routine = exit_routine;
	threadPool->threadSwap.routine_arg  = routine_arg;

   	qc_thread_condlock_lock(threadPool->condlock_worker);
	if(threadPool->worker_waiting)
   	{
		qc_thread_cond_signal(threadPool->cond_worker);
		threadPool->worker_waiting = 0;
   	}

	qc_thread_condlock_unlock(threadPool->condlock_worker);

	/* wait response by thread which waiting be wake up */
	threadPool->ctrler_waiting = 1;
	qc_thread_cond_wait(threadPool->cond_ctrler, threadPool->condlock_ctrler);

	qc_thread_condlock_unlock(threadPool->condlock_ctrler);

	return 0;
}



void *func_exitme(void *arg)
{
	qc_thread_exit(0);
	return 0;
}



int qc_threadpool_destroy_joinall(QcThreadPool *threadPool)
{
	int i;
	ThreadObject *threadObject;

	for(i=0; i<threadPool->thread_num; i++)
	{
		qc_threadpool_active(threadPool, func_exitme, NULL, NULL);
	}

	while(NULL != threadPool->threadObjects)
	{
		threadObject = threadPool->threadObjects;
		qc_thread_join(threadObject->tid, NULL);

		threadPool->threadObjects = threadObject->next;
		if(threadObject)
		{
			free(threadObject);
			threadObject = NULL;
		}
	}

	qc_thread_condlock_destroy(threadPool->condlock_ctrler);
	qc_thread_condlock_destroy(threadPool->condlock_worker);
	qc_thread_cond_destroy(threadPool->cond_ctrler);
	qc_thread_cond_destroy(threadPool->cond_worker);
	qc_thread_mutex_destroy(threadPool->mutex_worker);

	free(threadPool);
	threadPool = NULL;

	return 0;
}



int qc_threadpool_destroy_cancelall(QcThreadPool *threadPool)
{
	ThreadObject *threadObject;

	while(NULL != threadPool->threadObjects)
	{
		threadObject = threadPool->threadObjects;

		qc_thread_cancel(threadObject->tid);
		qc_thread_join(threadObject->tid, NULL);

		threadPool->threadObjects = threadObject->next;
		if(threadObject)
		{
			free(threadObject);
			threadObject = NULL;
		}
	}

	qc_thread_condlock_destroy(threadPool->condlock_ctrler);
	qc_thread_condlock_destroy(threadPool->condlock_worker);
	qc_thread_cond_destroy(threadPool->cond_ctrler);
	qc_thread_cond_destroy(threadPool->cond_worker);
	qc_thread_mutex_destroy(threadPool->mutex_worker);

	free(threadPool);
	threadPool = NULL;

	return 0;
}

