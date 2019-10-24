#ifndef H_QC_PUTTER
#define H_QC_PUTTER

#include "qc_thread.h"
#include "qc_message.h"
#include "qc_list.h"


typedef struct __QcPutter {
	QcCondLock *condlock;
	QcCond *cond;
	QcMessage *message;
	int is_timedout;
	int priority;
	QcListEntry *_entry;
}QcPutter;


typedef struct {
	QcList *puttersChain;
	int pop_counter;
}QcPutBucket;


typedef struct {
	QcPutBucket **putBuckets;
	int bucket_count;
	int cursor_bucketsn;
	int putter_count;
}QcPuttersChain;



#ifdef __cplusplus
extern "C" {
#endif

QcPutter* qc_putter_create();

int qc_putter_destroy(QcPutter *putter);

//----------------------------------------------------------------------
QcPutBucket* qc_putbucket_create();

int qc_putbucket_destroy(QcPutBucket *putBucket);

//----------------------------------------------------------------------
QcPuttersChain* qc_putterschain_create(int bucket_count);

int qc_putterschain_destroy(QcPuttersChain *puttersChain);

int qc_putterschain_push(QcPuttersChain *puttersChain, QcPutter *putter);

QcPutter* qc_putterschain_pop(QcPuttersChain *puttersChain);

int qc_putterschain_remove(QcPuttersChain *puttersChain, QcPutter *putter);


#ifdef __cplusplus
}
#endif


#endif  //H_QC_PUTTER
