#include "qc_qtbl.h"
#include "qc_qtbl_file.h"
#include "qc_utils.h"



struct __QcQTbl {
	QcQTblFile *qtbl;
	QcNumPool *rcdIdPool;
};



QcQTbl* qc_qtbl_open(int msgbuff_size, int msgcount_limit, char *table_filepath, QcErr *err)
{
	qc_assert(table_filepath);

	QcQTbl *queQdb;
	qc_malloc(queQdb, sizeof(QcQTbl));

	QcQTblFile* qtbl = qc_qtbl_file_open(msgbuff_size, msgcount_limit, table_filepath, err);
	if (!qtbl) {
		qc_free(queQdb);
		return NULL;
	}

	QcNumPool *rcdIdPool;
	qc_malloc(rcdIdPool, sizeof(QcNumPool));
	qc_numpool_init(rcdIdPool, msgcount_limit);

	queQdb->qtbl = qtbl;
	queQdb->rcdIdPool = rcdIdPool;

	return queQdb;
}


void qc_qtbl_close(QcQTbl *queQdb)
{
	qc_qtbl_file_close(queQdb->qtbl);
	qc_numpool_release(queQdb->rcdIdPool);
	qc_free(queQdb);
}


int qc_qtbl_append(QcQTbl *qcQueQdb, QcMessage *message, QcErr *err)
{
	int ret;
	ret = qc_qtbl_file_append(qcQueQdb->qtbl, (Qc_MsgRecord*)message, err);
	if (0 != ret)
		return 0;
	return -1;
}


int qc_qtbl_remove(QcQTbl *qcQueQdb, QcMessage *message, QcErr *err)
{
	int ret;
	ret = qc_qtbl_file_remove(qcQueQdb->qtbl, (Qc_MsgRecord*)message, err);
	if (0 != ret)
		return 0;
	return -1;
}


int qc_qtbl_loadqueue(QcQTbl *qcQueQdb, QcQueue *queue, QcErr *err)
{
	int ret;
	QcQTblFile *qtbl = qcQueQdb->qtbl;

	if (!qc_qtbl_file_fetch_ready(qtbl, err))
		return -1;

	QcMessage *message;
	qc_malloc(message, sizeof(Qc_MsgRecord));  //Qc_MsgRecord === QcMessage :-|

	while (1) {
		ret = qc_qtbl_file_do_fetch(qtbl, (Qc_MsgRecord*)message, err);
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
