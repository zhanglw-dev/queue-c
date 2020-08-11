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

#include "qc_utils.h"
#include "qc_thread.h"
#include "qc_prelude.h"
#include "qc_error.h"
#include "qc_log.h"
#include "qc_list.h"


//////////////////////////////////////////////////////////////////////////////////
//                               NumPool
//////////////////////////////////////////////////////////////////////////////////


int qc_numpool_init(QcNumPool *numPool, int init_count)
{
	int ret;

	memset(numPool, 0, sizeof(QcNumPool));

	ret = qc_staticlist_init(&numPool->qc_freeList, init_count, NULL);
	if (ret != 0)
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

	if (ret > 0)
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
	unitPool->unit_count = unit_count;

	ret = qc_numpool_init(&unitPool->numPool, unit_count);
	if (ret != 0)
	{
		goto failed;
	}

	qc_malloc(unitPool->unit_buff, unit_size*unit_count);
	if (NULL == unitPool->unit_buff)
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
	if (NULL == unitPool)
		return;

	if (unitPool->unit_buff)
		qc_free(unitPool->unit_buff);

	qc_numpool_release(&unitPool->numPool);
}


void* qc_unitpool_get(QcUnitPool *unitPool, int *idx)
{
	int index;
	char *ptr;

	index = qc_numpool_get(&unitPool->numPool);
	if (index < 0)
	{
		if (idx)
			*idx = -1;
		return NULL;
	}

	ptr = (char*)unitPool->unit_buff + index * (unitPool->unit_size);
	if (idx)
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

	ptr = unitPool->unit_buff + index * (unitPool->unit_size);

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


/*////////////////////////////////////////////////////////////////////////////////
//                               No-Block Queue
////////////////////////////////////////////////////////////////////////////////*/


struct __QcNBlockQue {
	int with_rwlock;
	QcMutex *mutex;
	QcStaticList lst_free;
	QcStaticList lst_used;
	void *buff;
	int  limit;
};



QcNBlockQue* qc_nblockque_create(int limit, int with_rwlock)
{
	QcNBlockQue *nblckQ = NULL;

	qc_assert(limit > 0);
	qc_assert(with_rwlock == 0 || with_rwlock == 1);

	if (NULL == (nblckQ = malloc(sizeof(struct __QcNBlockQue))))
	{
		qc_error("nblckque malloc failed");
		goto failed;
	}

	nblckQ->buff = NULL;
	nblckQ->mutex = NULL;
	nblckQ->limit = limit;
	nblckQ->with_rwlock = with_rwlock;

	if (0 != qc_staticlist_init(&nblckQ->lst_used, limit, NULL))
	{
		qc_error("nblckque lst_used init failed");
		goto failed;
	}

	if (0 != qc_staticlist_init(&nblckQ->lst_free, limit, NULL))
	{
		qc_error("nblckque lst_free init failed");
		goto failed;
	}

	qc_staticlist_clear(&nblckQ->lst_used);

	nblckQ->buff = malloc(sizeof(void*)*limit);
	if (NULL == nblckQ->buff)
	{
		qc_error("nblckque ptr buff malloc failed");
		goto failed;
	}

	if (1 == nblckQ->with_rwlock)
	{
		nblckQ->mutex = qc_thread_mutex_create();
		if (NULL == nblckQ->mutex)
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
	if (nblckQ)
	{
		if (nblckQ->with_rwlock) qc_thread_mutex_destroy(nblckQ->mutex);
		qc_staticlist_release(&nblckQ->lst_used);
		qc_staticlist_release(&nblckQ->lst_free);
		if (nblckQ->buff) free(nblckQ->buff);
		free(nblckQ);
	}
}



int qc_nblockque_put(QcNBlockQue *nblckQ, void *ptr)
{
	int index;
	void **pp_data;

	if (nblckQ->with_rwlock)
		qc_thread_mutex_lock(nblckQ->mutex);

	index = qc_staticlist_get_head(&nblckQ->lst_free);
	if (QC_INVALID_INT == index)
	{
		if (nblckQ->with_rwlock)
			qc_thread_mutex_unlock(nblckQ->mutex);
		return QC_FULLED;
	}
	else if (index < 0)
	{
		qc_error("error happend when get from freelst");
		if (nblckQ->with_rwlock)
			qc_thread_mutex_unlock(nblckQ->mutex);
		return -1;
	}

	pp_data = nblckQ->buff;
	pp_data[index] = ptr;

	index = qc_staticlist_add_tail(&nblckQ->lst_used, index);
	if (index < 0)
	{
		qc_error("error happend when add tail to usedlst");
		if (nblckQ->with_rwlock)
			qc_thread_mutex_unlock(nblckQ->mutex);
		return -1;
	}

	if (nblckQ->with_rwlock)
		qc_thread_mutex_unlock(nblckQ->mutex);

	return index;
}



int qc_nblockque_get(QcNBlockQue *nblckQ, void **pptr)
{
	int index;
	void *data, **pp_data;

	if (nblckQ->with_rwlock)
		qc_thread_mutex_lock(nblckQ->mutex);

	if (0 == qc_staticlist_count(&nblckQ->lst_used))
	{
		if (nblckQ->with_rwlock)
			qc_thread_mutex_unlock(nblckQ->mutex);

		return QC_INVALID_INT;
	}

	index = qc_staticlist_get_head(&nblckQ->lst_used);
	if (index < 0)
	{
		qc_error("error happend when get from usedlst");
		if (nblckQ->with_rwlock)
			qc_thread_mutex_unlock(nblckQ->mutex);
		return -1;
	}

	pp_data = nblckQ->buff;
	data = pp_data[index];

	index = qc_staticlist_add_tail(&nblckQ->lst_free, index);
	if (index < 0)
	{
		qc_error("error happend when add tail to freelst");
		if (nblckQ->with_rwlock)
			qc_thread_mutex_unlock(nblckQ->mutex);
		return -1;
	}

	if (nblckQ->with_rwlock)
		qc_thread_mutex_unlock(nblckQ->mutex);

	*pptr = data;
	return index;
}



/*////////////////////////////////////////////////////////////////////////////////
//                               Block Queue
////////////////////////////////////////////////////////////////////////////////*/


struct __QcBlockQue {
	void **ptr_array;
	int  limit;
	QcStaticList lst_free;
	QcStaticList lst_used;
	QcCondition *cond;
	QcCondLock *cond_lock;
	QcMutex *threads_mutex;
};



QcBlockQue* qc_blockque_create(int limit)
{
	QcBlockQue *queue = NULL;

	qc_assert(limit > 0);

	qc_malloc(queue, sizeof(QcBlockQue));
	if (NULL == queue)
	{
		qc_error("QcBlockQue malloc failed.");
		goto failed;
	}

	memset(queue, 0, sizeof(QcBlockQue));

	if (0 != qc_staticlist_init(&queue->lst_used, limit, NULL))
	{
		qc_error("QcBlockQue->lst_used init failed.");
		goto failed;
	}

	qc_staticlist_clear(&queue->lst_used);

	if (0 != qc_staticlist_init(&queue->lst_free, limit, NULL))
	{
		qc_error("QcBlockQue->lst_free init failed.");
		goto failed;
	}

	qc_malloc(queue->ptr_array, sizeof(void*)*limit);
	if (NULL == queue->ptr_array)
	{
		qc_error("QcBlockQue->ptr_array malloc failed.");
		goto failed;
	}

	memset(queue->ptr_array, 0, sizeof(void*)*limit);

	queue->cond_lock = qc_thread_condlock_create();
	if (NULL == queue->cond_lock)
	{
		qc_error("QcBlockQue->cond_lock create failed.");
		goto failed;
	}

	queue->cond = qc_thread_condition_create();
	if (NULL == queue->cond)
	{
		qc_error("QcBlockQue->cond create failed.");
		goto failed;
	}

	queue->threads_mutex = qc_thread_mutex_create();
	if (NULL == queue->threads_mutex)
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
	if (NULL != queue)
	{
		if (NULL != queue->cond_lock) qc_thread_condlock_destroy(queue->cond_lock);
		if (NULL != queue->cond) qc_thread_condition_destroy(queue->cond);
		if (NULL != queue->threads_mutex) qc_thread_mutex_destroy(queue->threads_mutex);
		qc_staticlist_release(&queue->lst_used);
		qc_staticlist_release(&queue->lst_free);
		if (NULL != queue->ptr_array) free(queue->ptr_array);
		free(queue);
	}
}



int qc_blockque_put(QcBlockQue *queue, void *ptr)
{
	int index;

	qc_thread_condlock_lock(queue->cond_lock);

	index = qc_staticlist_get_head(&queue->lst_free);
	if (QC_INVALID_INT == index)
	{
		qc_thread_condlock_unlock(queue->cond_lock);
		return QC_FULLED;
	}
	else if (index < 0)
	{
		qc_error("error happend when get from QcBlockQue->qc_staticlist_free.");
		qc_thread_condlock_unlock(queue->cond_lock);
		return -1;
	}

	queue->ptr_array[index] = ptr;

	index = qc_staticlist_add_tail(&queue->lst_used, index);
	if (index < 0)
	{
		qc_error("error happend when add tail to QcBlockQue->qc_staticlist_used.");
		qc_thread_condlock_unlock(queue->cond_lock);
		return -1;
	}

	qc_thread_condition_signal(queue->cond);
	qc_thread_condlock_unlock(queue->cond_lock);

	return 0;
}



void* qc_blockque_get(QcBlockQue *queue)
{
	int index;
	void *ptr;

	qc_thread_mutex_lock(queue->threads_mutex);
	qc_thread_condlock_lock(queue->cond_lock);

	if (0 == qc_staticlist_count(&queue->lst_used))
	{
		qc_thread_condition_wait(queue->cond, queue->cond_lock);
	}

	qc_thread_mutex_unlock(queue->threads_mutex);

	index = qc_staticlist_get_head(&queue->lst_used);
	if (index < 0)
	{
		qc_error("error happend when get from queue->qc_staticlist_used.");
		qc_thread_condlock_unlock(queue->cond_lock);
		return NULL;
	}

	ptr = queue->ptr_array[index];

	index = qc_staticlist_add_tail(&queue->lst_free, index);
	if (index < 0)
	{
		qc_error("error happend when add tail to queue->qc_staticlist_free.");
		qc_thread_condlock_unlock(queue->cond_lock);
		return NULL;
	}

	qc_thread_condlock_unlock(queue->cond_lock);

	return ptr;
}

