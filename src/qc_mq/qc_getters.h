#ifndef H_QC_GETTER
#define H_QC_GETTER

#include "qc_thread.h"
#include "qc_error.h"
#include "qc_message.h"
#include "qc_list.h"
#include "qc_thread.h"
#include "qc_error.h"
#include "qc_message.h"


typedef struct {
	QcCondLock *condlock;
	QcCond *cond;
	//QcMessage *message;
	int is_timedout;

	QcListEntry *_entry;
}QcGetter;


typedef struct {
	QcList *_gettersList;
}QcGettersList;



#ifdef __cplusplus
extern "C" {
#endif

QcGetter* qc_getter_create();

int qc_getter_destroy(QcGetter *getter);


QcGettersList* qc_getterslist_create();

int qc_getterslist_destroy(QcGettersList *getterList);

int qc_getterslist_push(QcGettersList *getterList, QcGetter *getter);

QcGetter* qc_getterslist_pop(QcGettersList *getterList);

int qc_getterslist_remove(QcGettersList *getterList, QcGetter *getter);

#ifdef __cplusplus
}
#endif


#endif  //H_QC_GETTER
