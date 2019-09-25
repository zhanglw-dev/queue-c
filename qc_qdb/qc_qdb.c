#include "qc_qdb.h"
#include "qc_qdb_file.h"
#include "qc_pool.h"



struct __QcQdb {
	QcQdbFile *qdb;
	QcNumPool *persistIdPool;
};



QcQdb* qc_persist_open(int msgbuff_size, int msgcount_limit, char *persist_filepath, QcErr *err)
{
	qc_assert(persist_filepath);

	QcQdb *queQdb = (QcQdb*)malloc(sizeof(QcQdb));
	qc_assert(queQdb);
	
	QcQdbFile* qdb = qc_persist_file_open(msgbuff_size, msgcount_limit, persist_filepath, err);
	if (!qdb) {
		return NULL;
	}

	QcNumPool *persistIdPool = (QcNumPool*)malloc(sizeof(QcNumPool));
	qc_numpool_init(persistIdPool, msgcount_limit);

	queQdb->qdb = qdb;
	queQdb->persistIdPool = persistIdPool;


	return queQdb;
}


void qc_persist_close(QcQdb *queQdb)
{
	qc_free(queQdb);
}


int qc_persist_append(QcQdb *qcQueQdb, QcMessage *message, QcErr *err)
{
	int ret;
	ret = qc_persist_file_append(qcQueQdb->qdb, (Qc_MsgEqual*)message, err);
	return ret;
}


int qc_persist_remove(QcQdb *qcQueQdb, QcMessage *message, QcErr *err)
{
	int ret;
	ret = qc_persist_file_remove(qcQueQdb->qdb, (Qc_MsgEqual*)message, err);
	return ret;
}


int qc_persist_loadqueue(QcQdb *qcQueQdb, QcQueue *queue, QcErr *err)
{
	QcQdbFile *qdb = qcQueQdb->qdb;

	if (!qc_persist_file_fetch_ready(qdb, err))
		return -1;

	QcMessage *message = (QcMessage*)malloc(sizeof(Qc_MsgEqual));  //Qc_MsgEqual === QcMessage :-|

	while (1) {
		int ret = qc_persist_file_do_fetch(qdb, (Qc_MsgEqual*)message, err);
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
