#ifndef H_QC_PUTTER_LIST
#define H_QC_PUTTER_LIST

#include "qc_thread.h"
#include "qc_message.h"
#include "qc_list.h"
#include "qc_putter.h"
#include "qc_put_bucket.h"


typedef struct {
	QcPutBucket **putBuckets;
	int bucket_count;
	int cursor_bucketsn;
	int putter_count;
}QcPutterChain;



#ifdef __cplusplus
extern "C" {
#endif


QcPutterChain* qc_putterchain_create(int bucket_count);

int qc_putterchain_destroy(QcPutterChain *putterList);

int qc_putterchain_push(QcPutterChain *putterList, QcPutter *putter);

QcPutter* qc_putterchain_pop(QcPutterChain *putterList);

int qc_putterchain_remove(QcPutterChain *putterList, QcPutter *putter);


#ifdef __cplusplus
}
#endif


#endif /*H_QC_PUTTER_LIST*/
