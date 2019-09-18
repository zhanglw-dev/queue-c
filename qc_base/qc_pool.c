#include "qc_pool.h"
#include "qc_list.h"
#include "qc_log.h"


//////////////////////////////////////////////////////////////////////////////////
//                               NumPool
//////////////////////////////////////////////////////////////////////////////////


int qc_numpool_init(QcNumPool *numPool, int init_count)
{
	int ret;

	memset(numPool, 0, sizeof(QcNumPool));

	ret = qc_staticlist_init(&numPool->qc_freeList, init_count);
	if(ret != 0)
	{
		return -1;
	}

	return 0;
}


void qc_numpool_release(QcNumPool *numPool)
{
	qc_staticlist_release(&numPool->qc_freeList);
}


int qc_numpool_get(QcNumPool *numPool)
{
	int idx;
	idx = qc_staticlist_get_head(&numPool->qc_freeList);
	return idx;
}


int qc_numpool_put(QcNumPool *numPool, int idx)
{
    int ret;
	ret = qc_staticlist_add_tail(&numPool->qc_freeList, idx);

    if(ret>0)
        return 0;
    else
        return -1;
}


int qc_numpool_count(QcNumPool *numPool)
{
	return numPool->qc_freeList.limit;
}


int qc_numpool_freenum(QcNumPool *numPool)
{
	return numPool->qc_freeList.num;
}


int qc_numpool_usednum(QcNumPool *numPool)
{
	return numPool->qc_freeList.limit - numPool->qc_freeList.num;
}


//////////////////////////////////////////////////////////////////////////////////
//                               UnitPool
//////////////////////////////////////////////////////////////////////////////////


int qc_unitpool_init(QcUnitPool *unitPool, int unit_size, int unit_count)
{
	int ret;

	memset(unitPool, 0, sizeof(QcUnitPool));

    unitPool->unit_buff = NULL;
    unitPool->unit_size = unit_size;
    unitPool->unit_count  = unit_count;

    ret = qc_numpool_init(&unitPool->numPool, unit_count);
    if(ret != 0)
    {
        goto failed;
    }

    qc_malloc(unitPool->unit_buff, unit_size*unit_count);
    if(NULL == unitPool->unit_buff)
    {
        goto failed;
    }

    return 0;

failed:
    qc_unitpool_release(unitPool);
    return -1;
}


void qc_unitpool_release(QcUnitPool *unitPool)
{
    if(NULL == unitPool)
        return;

    if(unitPool->unit_buff)
        qc_free(unitPool->unit_buff);

    qc_numpool_release(&unitPool->numPool);
}


void* qc_unitpool_get(QcUnitPool *unitPool, int *idx)
{
    int index;
    char *ptr;

    index = qc_numpool_get(&unitPool->numPool);
    if(index<0)
    {
        if(idx)
            *idx = -1;
        return NULL;
    }

    ptr = (char*)unitPool->unit_buff + index*(unitPool->unit_size);
    if(idx)
        *idx = index;

    return ptr;
}


int qc_unitpool_put(QcUnitPool *unitPool, int index)
{
    int ret;
    ret = qc_numpool_put(&unitPool->numPool, index);
    return ret;
}


void* qc_unitpool_ptr_byindex(QcUnitPool *unitPool, int index)
{
    char *ptr;

    qc_assert(unitPool);
    qc_assert(unitPool->unit_count > index);

    ptr = unitPool->unit_buff + index*(unitPool->unit_size);

    return ptr;
}


int qc_unitpool_count(QcUnitPool *unitPool)
{
	return qc_numpool_count(&unitPool->numPool);
}


int qc_unitpool_usednum(QcUnitPool *unitPool)
{
    return qc_numpool_usednum(&unitPool->numPool);
}


int qc_unitpool_freenum(QcUnitPool *unitPool)
{
	return qc_numpool_freenum(&unitPool->numPool);
}
