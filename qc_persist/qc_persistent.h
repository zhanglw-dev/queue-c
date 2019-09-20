#ifndef H_QC_PERSISTENT
#define H_QC_PERSISTENT

#include "qc_message.h"
#include "qc_msg_queue.h"
#include "qc_error.h"


typedef struct __QcQuePersist QcQuePersist;



QcQuePersist* qc_persist_open(int msgbuff_size, int msgcount_limit, char *persist_filepath, QcErr *err);

void qc_persist_close(QcQuePersist *quePersist);

int qc_persist_append(QcQuePersist *qcQuePersist, QcMessage *message, QcErr *err);

int qc_persist_remove(QcQuePersist *qcQuePersist, QcMessage *message, QcErr *err);



#endif //H_QC_PERSISTENT
