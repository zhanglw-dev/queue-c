#ifndef H_QC_GETTER
#define H_QC_GETTER

#include "qc_thread.h"
#include "qc_error.h"
#include "qc_message.h"
#include "qc_list.h"


typedef struct {
	QcCondLock *condlock;
	QcCond *cond;
	//QcMessage *message;
	int is_timedout;

	QcListEntry *_entry;
}QcGetter;



QcGetter* qc_getter_create();

int qc_getter_destroy(QcGetter *getter);


#endif  //H_QC_GETTER
