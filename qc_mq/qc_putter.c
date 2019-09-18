#include "qc_putter.h"
#include "qc_log.h"


QcPutter* qc_putter_create() {
	QcPutter *putter;

	qc_malloc(putter, sizeof(QcPutter));
	if (NULL == putter) {
		qc_error("malloc QcPutter faild.");
		return NULL;
	}

	putter->cond = qc_thread_cond_create();
	if (NULL == putter->cond) {
		qc_error("create putter->cond failed.");
		qc_free(putter);
		return NULL;
	}

	putter->condlock = qc_thread_condlock_create();
	if (NULL == putter->condlock) {
		qc_error("create putter->condlock failed.");
		qc_free(putter);
		return NULL;
	}

	putter->message = NULL;
	putter->is_timedout = 0;
	putter->priority = 0;

	return putter;
}


int qc_putter_destroy(QcPutter *putter) {
	if (NULL == putter) {
		qc_error("invalid params.");
		return -1;
	}

	qc_thread_cond_destroy(putter->cond);
	qc_thread_condlock_destroy(putter->condlock);

	qc_free(putter);

	return 0;
}
