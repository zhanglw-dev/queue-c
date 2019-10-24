#include "qc_qsystem.h"
#include "qc_qmanager.h"



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
QcQSystem* qc_qsys_create_byconfig(const char* config_file, QcErr *err)
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


int qc_qsys_queue_add(QcQSystem *qSys, const char *qname, QcQueue *queue, QcErr *err)
{
	int ret;
	ret = qc_qmng_addque(qSys->qManager, qname, queue, err);
	if (0 != ret)
		return -1;
	return 0;
}


QcQueue* qc_qsys_queue_get(QcQSystem *qSys, const char *qname, QcErr *err)
{
	QcQueue *queue;
	queue = qc_qmng_getque(qSys->qManager, qname, err);
	return queue;
}

