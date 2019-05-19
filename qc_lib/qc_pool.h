#ifndef QCLIB_POOL_H
#define QCLIB_POOL_H

#ifdef __cplusplus
extern "C" {
#endif


//////////////////////////////////////////////////////////////////////////////////
//                               Num pool
//////////////////////////////////////////////////////////////////////////////////

typedef struct __QcNumPool QcNumPool;


QcNumPool* qc_numpool_create(int init_count, int with_rwlock);

void qc_numpool_destroy(QcNumPool *numpool);

int qc_numpool_freenum(QcNumPool *numpool);

int qc_numpool_usednum(QcNumPool *numpool);

int qc_numpool_get(QcNumPool *numpool);

int qc_numpool_put(QcNumPool *numpool, int idx);



//////////////////////////////////////////////////////////////////////////////////
//                               Unit Pool
//////////////////////////////////////////////////////////////////////////////////

typedef struct __QcUnitPool QcUnitPool;


QcUnitPool* qc_unitpool_create(int unit_size, int unit_num, int with_rwlock);

void qc_unitpool_destroy(QcUnitPool *unitpool);

void* qc_unitpool_get(QcUnitPool *unitpool, int *idx);

int qc_unitpool_put(QcUnitPool *unitpool, void *unit);

int qc_unitpool_usednum(QcUnitPool *unitpool);

void* qc_unitpool_ptr_byindex(QcUnitPool *unitpool, int idx);




#ifdef __cplusplus
}
#endif


#endif /*QCLIB_POOL_H*/
