#include "qc_qtbl.h"
#include "qc_qtbl_file.h"
#include "qc_utils.h"



struct __QcQTbl {
	QcQTblFile *qtbl;
	QcNumPool *persistIdPool;
};



QcQTbl* qc_qtbl_open(int msgbuff_size, int msgcount_limit, const char *table_filepath, QcErr *err)
{
	qc_assert(table_filepath);

	QcQTbl *queQdb;
	qc_malloc(queQdb, sizeof(QcQTbl));

	QcQTblFile* qtbl = qc_qtbl_file_open(msgbuff_size, msgcount_limit, table_filepath, err);
	if (!qtbl) {
		qc_free(queQdb);
		return NULL;
	}

	QcNumPool *persistIdPool;
	qc_malloc(persistIdPool, sizeof(QcNumPool));
	qc_numpool_init(persistIdPool, msgcount_limit);

	queQdb->qtbl = qtbl;
	queQdb->persistIdPool = persistIdPool;

	return queQdb;
}


void qc_qtbl_close(QcQTbl *queQdb)
{
	qc_qtbl_file_close(queQdb->qtbl);
	qc_numpool_release(queQdb->persistIdPool);
	qc_free(queQdb);
}


int qc_qtbl_append(QcQTbl *qTbl, QcMessage *message, QcErr *err)
{
	int ret;

	Qc_MsgRecord *msgRecord = (Qc_MsgRecord*)message;
	msgRecord->persist_id = qc_numpool_get(qTbl->persistIdPool);
	if (msgRecord->persist_id < 0)
		return -1;

	ret = qc_qtbl_file_append(qTbl->qtbl, msgRecord, err);
	if (0 != ret)
		return -1;

	return 0;
}


int qc_qtbl_remove(QcQTbl *qTbl, QcMessage *message, QcErr *err)
{
	int ret;

	Qc_MsgRecord *msgRecord = (Qc_MsgRecord*)message;
	ret = qc_qtbl_file_remove(qTbl->qtbl, (Qc_MsgRecord*)message, err);
	if (0 != ret)
		return -1;

	qc_numpool_put(qTbl->persistIdPool, msgRecord->persist_id);

	return 0;
}


int qc_qtbl_loadqueue(QcQTbl *qTbl, QcQueue *queue, QcErr *err)
{
	int ret;
	QcQTblFile *qtbl = qTbl->qtbl;

	if (0 != qc_qtbl_file_fetch_ready(qtbl, err))
		return -1;

	Qc_MsgRecord *msgRecord = NULL;

	while (1) {
		qc_malloc(msgRecord, sizeof(Qc_MsgRecord));  //Qc_MsgRecord === QcMessage :-|
		ret = qc_qtbl_file_do_fetch(qtbl, msgRecord, err);
		if (0 == ret) {
			ret = qc_queue_msgput(queue, (QcMessage*)msgRecord, 0, err);
			if (ret < 0)
				goto failed;
		}
		else if (1 == ret) { //completed!
			qc_free(msgRecord);
			break;
		}
		else {
			goto failed;
		}
	}
	return 0;

failed:
	qc_free(msgRecord);
	return -1;
}
