#include "qc_qsystem.h"
#include "qc_qmanager.h"
#include "qc_queue.h"



struct __QcQSystem {
	QcQueManager *qManager;
};



QcQSystem* qc_qsys_create()
{
	QcQueManager *qManager = qc_qmng_create();
	if (!qManager)
		return NULL;

	QcQSystem *qSystem = (QcQSystem*)malloc(sizeof(QcQSystem));
	qc_assert(qSystem);

	qSystem->qManager = qManager;

	return qSystem;
}

/*
QcQSystem* qc_qsys_create_ex(const char* config_file, QcErr *err)
{
	//
	return NULL;
}
*/

void qc_qsys_destory(QcQSystem *qSys)
{
	qc_qmng_destory(qSys->qManager);
	qc_free(qSys);
}


int qc_qsys_addqueue(QcQSystem *qSys, const char *qname, QcQueue *queue, QcErr *err)
{
	int ret;
	ret = qc_qmng_addqueue(qSys->qManager, qname, queue, err);
	if (0 != ret)
		return -1;
	return 0;
}


int qc_qsys_delqueue(QcQSystem *qSys, const char *qname, QcErr *err)
{
	int ret;
	ret = qc_qmng_delqueue(qSys->qManager, qname, err);
	if (0 != ret)
		return -1;
	return 0;
}


/*
QcQueue* qc_qsys_queue_get(QcQSystem *qSys, const char *qname, QcErr *err)
{
	QcQueue *queue;
	queue = qc_qmng_getqueue(qSys->qManager, qname, err);
	return queue;
}
*/


int qc_qsys_putmsg(QcQSystem *qSys, const char *qname, QcMessage *message, int msec, QcErr *err)
{
	qc_assert(qname);
	QcQueue *queue;
	queue = qc_qmng_getqueue(qSys->qManager, qname, err);
	if (NULL == queue) {
		qc_seterr(err, QC_ERR_QUEUE_NOTEXIST, "queue not exist.");
		return QC_ERR_QUEUE_NOTEXIST;
	}

	int ret = qc_queue_msgput(queue, message, msec, err);
	return ret;
}


QcMessage* qc_qsys_getmsg(QcQSystem *qSys, const char *qname, int msec, QcErr *err)
{
	qc_assert(qname);
	QcQueue *queue;
	queue = qc_qmng_getqueue(qSys->qManager, qname, err);
	if (NULL == queue) {
		return NULL;
	}

	QcMessage* message = qc_queue_msgget(queue, msec, err);
	return message;
}
