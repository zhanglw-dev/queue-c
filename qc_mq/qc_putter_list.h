#ifndef H_QC_PUTTER_LIST
#define H_QC_PUTTER_LIST

#include "qc_thread.h"
#include "qc_message.h"
#include "qc_list.h"



typedef struct __QcPutter{
    QcCondLock *condlock;
    QcCond *cond;
    QcMessage *message;
    int is_timedout;

    QcListEntry *_entry;
}QcPutter;


typedef struct{
    QcList *_putterList;
}QcPutterList;



#ifdef __cplusplus
extern "C" {
#endif


QcPutter* qc_putter_create();

int qc_putter_destroy(QcPutter *putter);


QcPutterList* qc_putterlist_create();

int qc_putterlist_destroy(QcPutterList *putterList);

int qc_putterlist_push(QcPutterList *putterList, QcPutter *putter);

QcPutter* qc_putterlist_pop(QcPutterList *putterList);

int qc_putterlist_remove(QcPutterList *putterList, QcPutter *putter);


#ifdef __cplusplus
}
#endif


#endif /*H_QC_PUTTER_LIST*/
