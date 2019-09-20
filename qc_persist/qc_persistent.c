#include "qc_persistent.h"
#include "qc_persist_file.h"
#include "qc_pool.h"



struct __QcQuePersist {
	QcPersistFile *persistFile;
	QcUnitPool *msgPool;
};



QcQuePersist* qc_persist_open(int msgbuff_size, int msgcount_limit, char *persist_filepath, QcErr *err)
{
	if (!persist_filepath)
		return NULL;

	QcQuePersist *quePersist = (QcQuePersist*)malloc(sizeof(QcQuePersist));
	if (NULL == quePersist)
		return NULL;

	return NULL;
}


void qc_persist_close(QcQuePersist *quePersist)
{
	qc_free(quePersist);
}


int qc_persist_append(QcQuePersist *qcQuePersist, QcMessage *message, QcErr *err)
{
	return 0;
}


int qc_persist_remove(QcQuePersist *qcQuePersist, QcMessage *message, QcErr *err)
{
	return 0;
}
