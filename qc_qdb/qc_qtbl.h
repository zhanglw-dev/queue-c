#ifndef H_QC_PERSISTENT
#define H_QC_PERSISTENT

#include "qc_message.h"
#include "qc_queue.h"
#include "qc_error.h"


typedef struct __QcQTbl QcQTbl;



QcQTbl* qc_qtbl_open(int msgbuff_size, int msgcount_limit, const char *table_filepath, QcErr *err);

void qc_qtbl_close(QcQTbl *qTable);

int qc_qtbl_append(QcQTbl *qTable, QcMessage *message, QcErr *err);

int qc_qtbl_remove(QcQTbl *qTable, QcMessage *message, QcErr *err);

int qc_qtbl_loadqueue(QcQTbl *qTable, QcQueue *queue, QcErr *err);


#endif //H_QC_PERSISTENT
