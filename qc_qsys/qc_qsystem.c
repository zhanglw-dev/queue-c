#include "qc_qsystem.h"
#include "qc_qmanager.h"



struct __QcQSystem {
	QcQueManager *qManager;
};


QcQSystem *globalQSystem;



QcQSystem* qc_qsys_init()
{
	QcQueManager *qManager = (QcQueManager*)malloc(sizeof(QcQueManager));
	qc_assert(qManager);

	QcQSystem *qSystem = (QcQSystem*)malloc(sizeof(QcQSystem));
	qc_assert(qSystem);

	qSystem->qManager = qManager;

	return qSystem;
}


QcQSystem* qc_qsys_init_byconfig(const char* config_file, QcErr *err)
{
	//
	return NULL;
}


void qc_qsys_release(QcQSystem *qSys)
{
	qc_qmng_destory(qSys->qManager);
	qc_free(qSys);
}


QcQSystem* getQSystem()
{
	return globalQSystem;
}


int qc_qsys_queue_register(QcQSystem *qSys, const char *qname, QcQueue *queue, QcErr *err)
{
	int ret;
	ret = qc_qmng_addque(qSys->qManager, qname, queue, err);
	return 0;
}


QcQueue* qc_qsys_queue_get(QcQSystem *qSys, const char *qname, QcErr *err)
{
	QcQueue *queue;
	queue = qc_qmng_getque(qSys->qManager, qname, err);
	return queue;
}

