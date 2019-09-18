#ifndef H_QC_PUT_BUCKET
#define H_QC_PUT_BUCKET

#include "qc_prelude.h"
#include "qc_list.h"



typedef struct {
	QcList *putterList;
	int pop_counter;
}QcPutBucket;



QcPutBucket* qc_putbucket_create();

int qc_putbucket_destroy(QcPutBucket *putBucket);


#endif  //H_QC_PUT_BUCKET
