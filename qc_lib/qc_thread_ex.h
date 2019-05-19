#ifndef H_QC_THREADX
#define H_QC_THREADX


#include "qc_prelude.h"
#include "qc_thread.h"



#ifdef __cplusplus
extern "C" {
#endif

//////////////////////////////////////////////////////////////////////////////////
//                              Thread Group
//////////////////////////////////////////////////////////////////////////////////


typedef struct __QcThreadGroup QcThreadGroup;

typedef void *(*qc_threadgroup_routine)(void *);

/*------------------------------------------------------------------------------*/

QcThreadGroup* qc_threadgroup_create(int threadnum, qc_threadgroup_routine routine, void *routine_arg);

int qc_threadgroup_destroy_joinall(QcThreadGroup *threadGroup);

int qc_threadgroup_destroy_cancelall(QcThreadGroup *threadGroup);




//////////////////////////////////////////////////////////////////////////////////
//                              Thread Pool
//////////////////////////////////////////////////////////////////////////////////


typedef struct __qc_threadpool__ QcThreadPool;

typedef void *(*qc_threadpl_routine)(void *);

/*------------------------------------------------------------------------------*/

QcThreadPool* qc_threadpool_create(int thread_num);

int qc_threadpool_active(QcThreadPool *pool, qc_threadpl_routine exec_routine, qc_threadpl_routine exit_routine, void *routine_arg);

int qc_threadpool_destroy_joinall(QcThreadPool *pool);

int qc_threadpool_destroy_cancelall(QcThreadPool *pool);


#ifdef __cplusplus
}
#endif


#endif /*H_QC_THREADX*/
