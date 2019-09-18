#ifndef H_QC_MSG_BUCKET
#define H_QC_MSG_BUCKET


#include "qc_prelude.h"
#include "qc_list.h"



typedef struct {
	QcList *msgList;
	int pop_counter;
}QcMsgBucket;



QcMsgBucket* qc_msgbucket_create();

int qc_msgbucket_destroy(QcMsgBucket *msgBucket);



#endif  //H_QC_MSG_BUCKET
