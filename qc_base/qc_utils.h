#ifndef QC_UTILS_H
#define QC_UTILS_H

#include "qc_list.h"


#ifdef __cplusplus
extern "C" {
#endif

//////////////////////////////////////////////////////////////////////////////////
//                               Num pool
//////////////////////////////////////////////////////////////////////////////////

struct __QcNumPool {
	QcStaticList qc_freeList;
};

typedef struct __QcNumPool QcNumPool;


int qc_numpool_init(QcNumPool *numPool, int init_count);

void qc_numpool_release(QcNumPool *numPool);

int qc_numpool_count(QcNumPool *numPool);

int qc_numpool_freenum(QcNumPool *numPool);

int qc_numpool_usednum(QcNumPool *numPool);

int qc_numpool_get(QcNumPool *numPool);

int qc_numpool_put(QcNumPool *numPool, int idx);



//////////////////////////////////////////////////////////////////////////////////
//                               Unit Pool
//////////////////////////////////////////////////////////////////////////////////

struct __QcUnitPool {
	QcNumPool numPool;
	int unit_size;
	int unit_count;
	char *unit_buff;
};


typedef struct __QcUnitPool QcUnitPool;


int qc_unitpool_init(QcUnitPool *unitPool, int unit_size, int unit_num);

void qc_unitpool_release(QcUnitPool *unitPool);

void* qc_unitpool_get(QcUnitPool *unitPool, int *idx);

int qc_unitpool_put(QcUnitPool *unitPool, int idx);

int qc_unitpool_count(QcUnitPool *unitPool);

int qc_unitpool_usednum(QcUnitPool *unitPool);

int qc_unitpool_freenum(QcUnitPool *unitPool);

void* qc_unitpool_ptr_byindex(QcUnitPool *unitPool, int idx);


/*////////////////////////////////////////////////////////////////////////////////
//                               No-Block Queue
////////////////////////////////////////////////////////////////////////////////*/

typedef struct __QcNBlockQue QcNBlockQue;


QcNBlockQue* qc_nblockque_create(int limit, int with_rwlock);

void qc_nblockque_destroy(QcNBlockQue *queue);

/* 0: ok  QC_FULLED: can not put because full  -1: error */
int qc_nblockque_put(QcNBlockQue *queue, void *ptr);

int qc_nblockque_get(QcNBlockQue *queue, void **pptr);



/*////////////////////////////////////////////////////////////////////////////////
//                               Block Queue
////////////////////////////////////////////////////////////////////////////////*/

typedef struct __QcBlockQue QcBlockQue;


QcBlockQue* qc_blockque_create(int limit);

void qc_blockque_destroy(QcBlockQue *queue);

int qc_blockque_put(QcBlockQue *queue, void *buff);

void* qc_blockque_get(QcBlockQue *queue);


#ifdef __cplusplus
}
#endif



#endif /*QC_UTILS_H*/
