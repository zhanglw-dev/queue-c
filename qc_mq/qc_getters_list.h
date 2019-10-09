#ifndef H_QC_GETTER_LIST
#define H_QC_GETTER_LIST

#include "qc_thread.h"
#include "qc_error.h"
#include "qc_message.h"
#include "qc_list.h"
#include "qc_getter.h"


typedef struct{
    QcList *_gettersList;
}QcGettersList;



#ifdef __cplusplus
extern "C" {
#endif


QcGettersList* qc_getterslist_create();

int qc_getterslist_destroy(QcGettersList *getterList);

int qc_getterslist_push(QcGettersList *getterList, QcGetter *getter);

QcGetter* qc_getterslist_pop(QcGettersList *getterList);

int qc_getterslist_remove(QcGettersList *getterList, QcGetter *getter);


#ifdef __cplusplus
}
#endif


#endif /*H_QC_GETTER_LIST*/
