#include "qc_qmanager.h"


struct __QcQueManager {
	QcHashTbl *queueTable;
};



QcQueManager* qc_qmng_create()
{
	QcQueManager *qManager = (QcQueManager*)malloc(sizeof(QcQueManager));
	qc_assert(qManager);

	QcHashTbl* queueTable = qc_hashtbl_create(1000, 1, qc_hashstring, qc_strcompare, NULL);
	qc_assert(queueTable);

	qManager->queueTable = queueTable;

	return qManager;
}


void qc_qmng_destory(QcQueManager *qManager)
{
	qc_hashtbl_enumbegin(qManager->queueTable);

	QcQueue *queue = (QcQueue*)qc_hashtbl_enumnext(qManager->queueTable);
	while (queue) {
		qc_queue_destroy(queue);
		queue = (QcQueue*)qc_hashtbl_enumnext(qManager->queueTable);
	}

	qc_hashtbl_destroy(qManager->queueTable);
	qc_free(qManager);
}


int qc_qmng_addque(QcQueManager *qManager, const char* qname, QcQueue* queue, QcErr *err)
{
	int ret;
	ret = qc_hashtbl_insert(qManager->queueTable, (void*)qname, queue);
	if (ret != 0)
		return -1;

	return 0;
}


int qc_qmng_removeque(QcQueManager *qManager, const char *qname, QcErr *err)
{
	int ret;
	ret = qc_hashtbl_delete(qManager->queueTable, (void*)qname);
	if (ret != 0)
		return -1;
	return 0;
}


QcQueue* qc_qmng_getque(QcQueManager *qManager, const char *qname, QcErr *err)
{
	QcQueue *queue = qc_hashtbl_find(qManager->queueTable, (void*)qname);
	return queue;
}
