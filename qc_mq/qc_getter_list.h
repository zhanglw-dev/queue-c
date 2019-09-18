#ifndef H_QC_GETTER_LIST
#define H_QC_GETTER_LIST

#include "qc_thread.h"
#include "qc_error.h"
#include "qc_message.h"
#include "qc_list.h"
#include "qc_getter.h"


typedef struct{
    QcList *_getterList;
}QcGetterList;



#ifdef __cplusplus
extern "C" {
#endif


QcGetterList* qc_getterlist_create();

int qc_getterlist_destroy(QcGetterList *getterList);

int qc_getterlist_push(QcGetterList *getterList, QcGetter *getter);

QcGetter* qc_getterlist_pop(QcGetterList *getterList);

int qc_getterlist_remove(QcGetterList *getterList, QcGetter *getter);


#ifdef __cplusplus
}
#endif


#endif /*H_QC_GETTER_LIST*/
