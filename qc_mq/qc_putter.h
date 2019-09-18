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

	QcListEntry *_entry;
}QcPutter;


#endif  //H_QC_PUTTER
