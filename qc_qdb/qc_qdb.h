#ifndef H_QC_PERSISTENT
#define H_QC_PERSISTENT

#include "qc_message.h"
#include "qc_mqueue.h"
#include "qc_error.h"


typedef struct __QcQdb QcQdb;



QcQdb* qc_qdb_open(int msgbuff_size, int msgcount_limit, char *persist_filepath, QcErr *err);

void qc_qdb_close(QcQdb *quePersist);

int qc_qdb_append(QcQdb *qcQuePersist, QcMessage *message, QcErr *err);

int qc_qdb_remove(QcQdb *qcQuePersist, QcMessage *message, QcErr *err);

int qc_qdb_loadqueue(QcQdb *qcQuePersist, QcQueue *queue, QcErr *err);


#endif //H_QC_PERSISTENT
