#include "qc_pool.h"
#include "qc_thread.h"
#include "qc_list.h"
#include "qc_log.h"

//////////////////////////////////////////////////////////////////////////////////
//                               NumPool
//////////////////////////////////////////////////////////////////////////////////


struct __QcNumPool{
    int with_rwlock;
    QcMutex *mutex;
    QcStaticList *qc_freeList;
    int limit;
};


QcNumPool* qc_numpool_create(int init_count, int with_rwlock)
{
	QcNumPool *acqHdl = NULL;

	qc_malloc(acqHdl, sizeof(QcNumPool))
    if(NULL == acqHdl)
	{
		qc_error("qc_malloc numpool failed");
		return NULL;
	}

    acqHdl->qc_freeList = NULL;
    acqHdl->limit = -1;
    acqHdl->mutex = NULL;
    acqHdl->with_rwlock = with_rwlock;

    if(with_rwlock)
    {
        acqHdl->mutex = qc_thread_mutex_create();
        if(NULL == acqHdl->mutex)
        {
            qc_error("create mutex for numpool failed");
            return NULL;
        }
    }

	acqHdl->qc_freeList = qc_staticlist_create(init_count);
	if(NULL == acqHdl->qc_freeList)
	{
		qc_free(acqHdl);
		return NULL;
	}

	acqHdl->limit = init_count;

	return acqHdl;
}


void qc_numpool_destroy(QcNumPool *numpool)
{
	qc_staticlist_destroy(numpool->qc_freeList);
    if(numpool->with_rwlock)
	    qc_thread_mutex_destroy(numpool->mutex);
	qc_free(numpool);
}

/*
int qc_numpool_expand(QcNumPool *qcResHdl, int expd_num)
{
	return 0;
}
*/

int qc_numpool_freenum(QcNumPool *numpool)
{
	int num;

    if(numpool->with_rwlock) qc_thread_mutex_lock(numpool->mutex);
    num = qc_staticlist_count(numpool->qc_freeList);
    if(numpool->with_rwlock) qc_thread_mutex_unlock(numpool->mutex);

	return num;
}


int qc_numpool_usednum(QcNumPool *numpool)
{
	int used;

	used = numpool->limit - qc_numpool_freenum(numpool);
	return used;
}


int qc_numpool_get(QcNumPool *numpool)
{
	int idx;

	if(numpool->with_rwlock) qc_thread_mutex_lock(numpool->mutex);
	idx = qc_staticlist_get_head(numpool->qc_freeList);
	if(numpool->with_rwlock) qc_thread_mutex_unlock(numpool->mutex);

	return idx;
}


int qc_numpool_put(QcNumPool *numpool, int idx)
{
    int ret;

	if(numpool->with_rwlock) qc_thread_mutex_lock(numpool->mutex);
	ret = qc_staticlist_add_tail(numpool->qc_freeList, idx);
	if(numpool->with_rwlock) qc_thread_mutex_unlock(numpool->mutex);

    if(ret>0)
        return 0;
    else
        return -1;
}



//////////////////////////////////////////////////////////////////////////////////
//                               Unit Pool
//////////////////////////////////////////////////////////////////////////////////


struct __QcUnitPool{
    QcNumPool *numpool;
    char *unit_buff;
    int unit_size;
    int unit_num;
    int with_rwlock;
};



QcUnitPool* qc_unitpool_create(int unit_size, int unit_num, int with_rwlock)
{
    QcUnitPool *unitpool;

    qc_malloc(unitpool, sizeof(QcUnitPool));
    if(NULL == unitpool)
    {
        return NULL;
    }

    unitpool->numpool = NULL;
    unitpool->unit_buff = NULL;
    unitpool->unit_size = unit_size;
    unitpool->unit_num  = unit_num;
    unitpool->with_rwlock = with_rwlock;

    unitpool->numpool = qc_numpool_create(unit_num, with_rwlock);
    if(NULL == unitpool->numpool)
    {
        goto failed;
    }

    qc_malloc(unitpool->unit_buff, unit_size*unit_num);
    if(NULL == unitpool->unit_buff)
    {
        goto failed;
    }

    return unitpool;

failed:
    qc_unitpool_destroy(unitpool);
    return NULL;
}


void qc_unitpool_destroy(QcUnitPool *unitpool)
{
    if(NULL == unitpool)
        return;

    if(unitpool->unit_buff)
        qc_free(unitpool->unit_buff);

    if(unitpool->numpool)
        qc_numpool_destroy(unitpool->numpool);

    qc_free(unitpool);
}


void* qc_unitpool_get(QcUnitPool *unitpool, int *idx)
{
    int index;
    char *ptr;

    index = qc_numpool_get(unitpool->numpool);
    if(index<0)
    {
        if(idx)
            *idx = -1;
        return NULL;
    }

    ptr = (char*)unitpool->unit_buff + index*(unitpool->unit_size);
    if(idx)
        *idx = index;

    return ptr;
}


int qc_unitpool_put(QcUnitPool *unitpool, void *unit)
{
    int index, ret;

    index = (int)((char*)unit - unitpool->unit_buff)/unitpool->unit_size;
    ret = qc_numpool_put(unitpool->numpool, index);

    return ret;
}


void* qc_unitpool_ptr_byindex(QcUnitPool *unitpool, int index)
{
    char *ptr;

    qc_assert(unitpool);
    qc_assert(unitpool->unit_num > index);

    ptr = unitpool->unit_buff + index*(unitpool->unit_size);

    return ptr;
}


int qc_unitpool_usednum(QcUnitPool *unitpool)
{
    return qc_numpool_usednum(unitpool->numpool);
}
