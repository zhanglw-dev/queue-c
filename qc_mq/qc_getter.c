#include "qc_getter.h"
#include "qc_log.h"


QcGetter* qc_getter_create() {
	QcGetter *getter;

	qc_malloc(getter, sizeof(QcGetter));
	if (NULL == getter) {
		qc_error("malloc QcGetter faild.");
		return NULL;
	}

	getter->cond = qc_thread_cond_create();
	if (NULL == getter->cond) {
		qc_error("create getter->cond failed.");
		qc_free(getter);
		return NULL;
	}

	getter->condlock = qc_thread_condlock_create();
	if (NULL == getter->condlock) {
		qc_error("create getter->condlock failed.");
		qc_free(getter);
		return NULL;
	}

	//getter->message = NULL;
	getter->is_timedout = 0;

	return getter;
}


int qc_getter_destroy(QcGetter *getter) {
	if (NULL == getter) {
		qc_error("invalid params.");
		return -1;
	}

	qc_thread_cond_destroy(getter->cond);
	qc_thread_condlock_destroy(getter->condlock);

	qc_free(getter);

	return 0;
}
