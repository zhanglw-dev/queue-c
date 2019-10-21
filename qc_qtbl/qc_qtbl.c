#include "qc_qtbl.h"
#include "qc_qtbl_file.h"
#include "qc_utils.h"



struct __QcQTbl {
	QcQTblFile *qtbl;
	QcNumPool *persistIdPool;
};



QcQTbl* qc_qtbl_open(int msgbuff_size, int msgcount_limit, char *persist_filepath, QcErr *err)
{
	qc_assert(persist_filepath);

	QcQTbl *queQdb = (QcQTbl*)malloc(sizeof(QcQTbl));
	qc_assert(queQdb);
	
	QcQTblFile* qtbl = qc_qtbl_file_open(msgbuff_size, msgcount_limit, persist_filepath, err);
	if (!qtbl) {
		return NULL;
	}

	QcNumPool *persistIdPool = (QcNumPool*)malloc(sizeof(QcNumPool));
	qc_numpool_init(persistIdPool, msgcount_limit);

	queQdb->qtbl = qtbl;
	queQdb->persistIdPool = persistIdPool;


	return queQdb;
}


void qc_qtbl_close(QcQTbl *queQdb)
{
	qc_free(queQdb);
}


int qc_qtbl_append(QcQTbl *qcQueQdb, QcMessage *message, QcErr *err)
{
	int ret;
	ret = qc_qtbl_file_append(qcQueQdb->qtbl, (Qc_MsgEqual*)message, err);
	return ret;
}


int qc_qtbl_remove(QcQTbl *qcQueQdb, QcMessage *message, QcErr *err)
{
	int ret;
	ret = qc_qtbl_file_remove(qcQueQdb->qtbl, (Qc_MsgEqual*)message, err);
	return ret;
}


int qc_qtbl_loadqueue(QcQTbl *qcQueQdb, QcQueue *queue, QcErr *err)
{
	QcQTblFile *qtbl = qcQueQdb->qtbl;

	if (!qc_qtbl_file_fetch_ready(qtbl, err))
		return -1;

	QcMessage *message = (QcMessage*)malloc(sizeof(Qc_MsgEqual));  //Qc_MsgEqual === QcMessage :-|

	while (1) {
		int ret = qc_qtbl_file_do_fetch(qtbl, (Qc_MsgEqual*)message, err);
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
