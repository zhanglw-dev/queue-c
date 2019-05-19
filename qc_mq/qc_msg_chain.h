#ifndef H_QC_MSGCHAIN
#define H_QC_MSGCHAIN

#include "qc_prelude.h"
#include "qc_message.h"
#include "qc_list.h"
#include "qc_thread.h"



typedef struct{
    QcList *msgList;
    int pop_counter;
}QcMsgBucket;


typedef struct{
    QcMessage *message;
    QcMsgBucket *bucket;
    int cell_idx;  //for presistent feature
}QcMsgEntry;


typedef struct{
    int count_limit;  //limit of message count
    int current_count;

    int bucket_count;
    QcMsgBucket **msgBuckets;
    int cursor_bucketsn;

    QcRWLock *rwlock;
}QcMsgChain;



#ifdef __cplusplus
extern "C" {
#endif


QcMsgChain* qc_msgchain_create(int count_limit, int priority_max);

int qc_msgchain_destroy(QcMsgChain *msgChain);

int qc_msgchain_msgcount(QcMsgChain *msgChain);

int qc_msgchain_pushmsg(QcMsgChain *msgChain, QcMessage *message);

QcMessage* qc_msgchain_popmsg(QcMsgChain *msgChain);


#ifdef __cplusplus
}
#endif


#endif /*H_QC_MSGCHAIN*/
