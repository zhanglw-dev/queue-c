#ifndef H_QC_QUEUE
#define H_QC_QUEUE

#include "qc_prelude.h"
#include "qc_error.h"
#include "qc_message.h"


#ifdef __cplusplus
extern "C" {
#endif


typedef struct __QcQueue QcQueue;


QcQueue* qc_queue_create(unsigned int limit, unsigned int priority_max, QcErr *err);

int qc_queue_destroy(QcQueue *queue, QcErr *err);

unsigned int qc_queue_msgcount(QcQueue *queue);

int qc_queue_msgput(QcQueue *queue, QcMessage *message, int sec, QcErr *err);

QcMessage* qc_queue_msgget(QcQueue *queue, int sec, QcErr *err);


#ifdef __cplusplus
}
#endif


#endif /*H_QC_QUEUE*/
