#ifndef QCLIB_QUEUE_H
#define QCLIB_QUEUE_H

#ifdef __cplusplus
extern "C" {
#endif



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


#endif /*QCLIB_QUEUE_H*/
