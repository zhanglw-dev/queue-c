#include "qc_prelude.h"
#include "qc_thread.h"
#include "qc_list.h"
#include "qc_queue.h"
#include "qc_log.h"



/*////////////////////////////////////////////////////////////////////////////////
//                               No-Block Queue
////////////////////////////////////////////////////////////////////////////////*/


struct __QcNBlockQue{
    int with_rwlock;
	QcMutex *mutex;
	QcStaticList *lst_free;
	QcStaticList *lst_used;
	void *buff;
	int  limit;
};



QcNBlockQue* qc_nblockque_create(int limit, int with_rwlock)
{
    QcNBlockQue *nblckQ = NULL;

    qc_assert(limit>0);
    qc_assert(with_rwlock==0 || with_rwlock==1);

    if(NULL == (nblckQ = malloc(sizeof(struct __QcNBlockQue))))
    {
        qc_error("nblckque malloc failed");
        goto failed;
    }

    nblckQ->buff = NULL;
    nblckQ->lst_free = NULL;
    nblckQ->lst_used = NULL;
    nblckQ->mutex = NULL;
    nblckQ->limit = limit;
    nblckQ->with_rwlock = with_rwlock;

    if(NULL == (nblckQ->lst_used = qc_staticlist_create(limit)))
    {
        qc_error("nblckque lst_used malloc failed");
        goto failed;
    }

    if(NULL == (nblckQ->lst_free = qc_staticlist_create(limit)))
    {
        qc_error("nblckque lst_free malloc failed");
        goto failed;
    }

    qc_staticlist_clear(nblckQ->lst_used);

    nblckQ->buff = malloc(sizeof(void*)*limit);
    if(NULL == nblckQ->buff)
    {
        qc_error("nblckque ptr buff malloc failed");
        goto failed;
    }

    if(1 == nblckQ->with_rwlock)
    {
        nblckQ->mutex = qc_thread_mutex_create();
        if(NULL == nblckQ->mutex)
        {
            qc_error("nblckque mutex create failed");
            goto failed;
        }
    }

    return nblckQ;

failed:
    qc_nblockque_destroy(nblckQ);
    return NULL;
}



void qc_nblockque_destroy(QcNBlockQue *nblckQ)
{
    if(nblckQ)
    {
        if(nblckQ->with_rwlock) qc_thread_mutex_destroy(nblckQ->mutex);
        if(nblckQ->lst_used) qc_staticlist_destroy(nblckQ->lst_used);
        if(nblckQ->lst_free) qc_staticlist_destroy(nblckQ->lst_free);
        if(nblckQ->buff) free(nblckQ->buff);
        free(nblckQ);
    }
}



int qc_nblockque_put(QcNBlockQue *nblckQ, void *ptr)
{
    int index;
    void **pp_data;

    if(nblckQ->with_rwlock)
        qc_thread_mutex_lock(nblckQ->mutex);

    index = qc_staticlist_get_head(nblckQ->lst_free);
    if(QC_INVALID_INT == index)
    {
        if(nblckQ->with_rwlock)
            qc_thread_mutex_unlock(nblckQ->mutex);
        return QC_FULLED;
    }
    else if(index < 0)
    {
        qc_error("error happend when get from freelst");
        if(nblckQ->with_rwlock)
            qc_thread_mutex_unlock(nblckQ->mutex);
        return -1;
    }

    pp_data = nblckQ->buff;
    pp_data[index] = ptr;

    index = qc_staticlist_add_tail(nblckQ->lst_used, index);
    if(index < 0)
    {
        qc_error("error happend when add tail to usedlst");
        if(nblckQ->with_rwlock)
            qc_thread_mutex_unlock(nblckQ->mutex);
        return -1;
    }

    if(nblckQ->with_rwlock)
        qc_thread_mutex_unlock(nblckQ->mutex);

    return index;
}



int qc_nblockque_get(QcNBlockQue *nblckQ, void **pptr)
{
    int index;
    void *data, **pp_data;

    if(nblckQ->with_rwlock)
        qc_thread_mutex_lock(nblckQ->mutex);

    if(0 == qc_staticlist_count(nblckQ->lst_used))
    {
        if(nblckQ->with_rwlock)
            qc_thread_mutex_unlock(nblckQ->mutex);

        return QC_INVALID_INT;
    }

    index = qc_staticlist_get_head(nblckQ->lst_used);
    if(index < 0)
    {
        qc_error("error happend when get from usedlst");
        if(nblckQ->with_rwlock)
            qc_thread_mutex_unlock(nblckQ->mutex);
        return -1;
    }

    pp_data = nblckQ->buff;
    data = pp_data[index];

    index = qc_staticlist_add_tail(nblckQ->lst_free, index);
    if(index < 0)
    {
        qc_error("error happend when add tail to freelst");
        if(nblckQ->with_rwlock)
            qc_thread_mutex_unlock(nblckQ->mutex);
        return -1;
    }

    if(nblckQ->with_rwlock)
        qc_thread_mutex_unlock(nblckQ->mutex);

    *pptr = data;
    return index;
}



/*////////////////////////////////////////////////////////////////////////////////
//                               Block Queue
////////////////////////////////////////////////////////////////////////////////*/


struct __QcBlockQue{
	void **ptr_array;
	int  limit;
	QcStaticList *qc_staticlist_free;
	QcStaticList *qc_staticlist_used;
	QcCond *cond;
	QcCondLock *cond_lock;
    QcMutex *threads_mutex;
};



QcBlockQue* qc_blockque_create(int limit)
{
    QcBlockQue *queue = NULL;

    qc_assert(limit>0);

    qc_malloc(queue, sizeof(QcBlockQue));
    if(NULL == queue)
    {
        qc_error("QcBlockQue malloc failed.");
        goto failed;
    }

    memset(queue, 0, sizeof(QcBlockQue));

    queue->qc_staticlist_used = qc_staticlist_create(limit);
    if(NULL == queue->qc_staticlist_used)
    {
        qc_error("QcBlockQue->qc_staticlist_used malloc failed.");
        goto failed;
    }

    qc_staticlist_clear(queue->qc_staticlist_used);

    queue->qc_staticlist_free = qc_staticlist_create(limit);
    if(NULL == queue->qc_staticlist_free)
    {
        qc_error("QcBlockQue->qc_staticlist_free malloc failed.");
        goto failed;
    }

    qc_malloc(queue->ptr_array, sizeof(void*)*limit);
    if(NULL == queue->ptr_array)
    {
        qc_error("QcBlockQue->ptr_array malloc failed.");
        goto failed;
    }

    memset(queue->ptr_array, 0, sizeof(void*)*limit);

    queue->cond_lock = qc_thread_condlock_create();
    if(NULL == queue->cond_lock)
    {
        qc_error("QcBlockQue->cond_lock create failed.");
        goto failed;
    }

    queue->cond = qc_thread_cond_create();
    if(NULL == queue->cond)
    {
        qc_error("QcBlockQue->cond create failed.");
        goto failed;
    }

    queue->threads_mutex = qc_thread_mutex_create();
    if(NULL == queue->threads_mutex)
    {
        qc_error("QcBlockQue->threads_mutex create failed.");
        goto failed;
    }

    return queue;

failed:
    qc_blockque_destroy(queue);
    return NULL;
}



void qc_blockque_destroy(QcBlockQue *queue)
{
    if(NULL != queue)
    {
        if(NULL != queue->cond_lock) qc_thread_condlock_destroy(queue->cond_lock);
        if(NULL != queue->cond) qc_thread_cond_destroy(queue->cond);
        if(NULL != queue->threads_mutex) qc_thread_mutex_destroy(queue->threads_mutex);
        if(NULL != queue->qc_staticlist_used) qc_staticlist_destroy(queue->qc_staticlist_used);
        if(NULL != queue->qc_staticlist_free) qc_staticlist_destroy(queue->qc_staticlist_free);
        if(NULL != queue->ptr_array) free(queue->ptr_array);
        free(queue);
    }
}



int qc_blockque_put(QcBlockQue *queue, void *ptr)
{
    int index;

    qc_thread_condlock_lock(queue->cond_lock);

    index = qc_staticlist_get_head(queue->qc_staticlist_free);
    if(QC_INVALID_INT == index)
    {
        qc_thread_condlock_unlock(queue->cond_lock);
        return QC_FULLED;
    }
    else if(index < 0)
    {
        qc_error("error happend when get from QcBlockQue->qc_staticlist_free.");
        qc_thread_condlock_unlock(queue->cond_lock);
        return -1;
    }

    queue->ptr_array[index] = ptr;

    index = qc_staticlist_add_tail(queue->qc_staticlist_used, index);
    if(index < 0)
    {
        qc_error("error happend when add tail to QcBlockQue->qc_staticlist_used.");
        qc_thread_condlock_unlock(queue->cond_lock);
        return -1;
    }

    qc_thread_cond_signal(queue->cond);
    qc_thread_condlock_unlock(queue->cond_lock);

    return 0;
}



void* qc_blockque_get(QcBlockQue *queue)
{
    int index;
    void *ptr;

    qc_thread_mutex_lock(queue->threads_mutex);
    qc_thread_condlock_lock(queue->cond_lock);

    if(0 == qc_staticlist_count(queue->qc_staticlist_used))
    {
        qc_thread_cond_wait(queue->cond, queue->cond_lock);
    }

    qc_thread_mutex_unlock(queue->threads_mutex);

    index = qc_staticlist_get_head(queue->qc_staticlist_used);
    if(index < 0)
    {
        qc_error("error happend when get from queue->qc_staticlist_used.");
        qc_thread_condlock_unlock(queue->cond_lock);
        return NULL;
    }

    ptr = queue->ptr_array[index];

    index = qc_staticlist_add_tail(queue->qc_staticlist_free, index);
    if(index < 0)
    {
        qc_error("error happend when add tail to queue->qc_staticlist_free.");
        qc_thread_condlock_unlock(queue->cond_lock);
        return NULL;
    }

    qc_thread_condlock_unlock(queue->cond_lock);

    return ptr;
}
