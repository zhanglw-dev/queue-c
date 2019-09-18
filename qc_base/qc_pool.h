#ifndef QCLIB_POOL_H
#define QCLIB_POOL_H

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



#ifdef __cplusplus
}
#endif


#endif /*QCLIB_POOL_H*/
