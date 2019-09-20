#include "qc_persistent.h"
#include "qc_persist_file.h"
#include "qc_pool.h"



struct __QcQuePersist {
	QcPersistFile *persistFile;
	QcNumPool *persistIdPool;
};



QcQuePersist* qc_persist_open(int msgbuff_size, int msgcount_limit, char *persist_filepath, QcErr *err)
{
	qc_assert(persist_filepath);

	QcQuePersist *quePersist = (QcQuePersist*)malloc(sizeof(QcQuePersist));
	qc_assert(quePersist);
	
	QcPersistFile* persistFile = qc_persist_file_open(msgbuff_size, msgcount_limit, persist_filepath, err);
	if (!persistFile) {
		return NULL;
	}

	QcNumPool *persistIdPool = (QcNumPool*)malloc(sizeof(QcNumPool));
	qc_numpool_init(persistIdPool, msgcount_limit);

	quePersist->persistFile = persistFile;
	quePersist->persistIdPool = persistIdPool;


	return quePersist;
}


void qc_persist_close(QcQuePersist *quePersist)
{
	qc_free(quePersist);
}


int qc_persist_append(QcQuePersist *qcQuePersist, QcMessage *message, QcErr *err)
{
	int ret;
	ret = qc_persist_file_append(qcQuePersist->persistFile, (Qc_MsgEqual*)message, err);
	return ret;
}


int qc_persist_remove(QcQuePersist *qcQuePersist, QcMessage *message, QcErr *err)
{
	int ret;
	ret = qc_persist_file_remove(qcQuePersist->persistFile, (Qc_MsgEqual*)message, err);
	return ret;
}


int qc_persist_loadqueue(QcQuePersist *qcQuePersist, QcQueue *queue, QcErr *err)
{
	QcPersistFile *persistFile = qcQuePersist->persistFile;

	if (!qc_persist_file_fetch_ready(persistFile, err))
		return -1;

	QcMessage *message = (QcMessage*)malloc(sizeof(Qc_MsgEqual));  //Qc_MsgEqual === QcMessage :-|

	while (1) {
		int ret = qc_persist_file_do_fetch(persistFile, (Qc_MsgEqual*)message, err);
		if (0 == ret) {
			ret = qc_queue_msgput(queue, message, 0, err);
			if (ret < 0)
				goto failed;
		}
		else if (1 == ret) { //completed!
			break;
		}
		else {
			goto failed;
		}
	}

	return 0;

failed:
	qc_free(message);
	return -1;
}
