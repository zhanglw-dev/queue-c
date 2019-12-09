#ifndef H_QC_PERSISTENT
#define H_QC_PERSISTENT

#include "qc_message.h"
#include "qc_queue.h"
#include "qc_error.h"


typedef struct __QcPsist QcPsist;



QcPsist* qc_psist_open(int msgbuff_size, int msgcount_limit, const char *table_filepath, QcErr *err);

void qc_psist_close(QcPsist *qTable);

int qc_psist_append(QcPsist *qTable, QcMessage *message, QcErr *err);

int qc_psist_remove(QcPsist *qTable, QcMessage *message, QcErr *err);

int qc_psist_loadqueue(QcPsist *qTable, QcQueue *queue, QcErr *err);


#endif //H_QC_PERSISTENT
