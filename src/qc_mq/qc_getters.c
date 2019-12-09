#include "qc_getters.h"
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



//--------------------------------------------------------------------------------

QcGettersList* qc_getterslist_create() {
	QcGettersList *getterList;
	qc_malloc(getterList, sizeof(QcGettersList));
	if (NULL == getterList) {
		return NULL;
	}

	QcList *list = qc_list_create(1);
	if (NULL == list) {
		qc_free(list);
		return NULL;
	}

	getterList->_gettersList = list;
	return getterList;
}


int qc_getterslist_destroy(QcGettersList *getterList) {
	if (NULL == getterList) {
		return -1;
	}

	if (getterList->_gettersList) {
		qc_list_destroy(getterList->_gettersList);
	}

	qc_free(getterList);

	return 0;
}


int qc_getterslist_push(QcGettersList *getterList, QcGetter *getter) {
	if (NULL == getterList || NULL == getter) {
		return -1;
	}

	QcListEntry *listEntry;

	qc_list_w_lock(getterList->_gettersList);
	int ret = qc_list_inserttail2(getterList->_gettersList, getter, &listEntry);
	if (ret < 0) {
		qc_list_w_unlock(getterList->_gettersList);
		return -1;
	}

	getter->_entry = listEntry;
	qc_list_w_unlock(getterList->_gettersList);
	return 0;
}


QcGetter* qc_getterslist_pop(QcGettersList *getterList) {
	QcGetter *getter;
	qc_list_w_lock(getterList->_gettersList);
	getter = qc_list_pophead(getterList->_gettersList);
	qc_list_w_unlock(getterList->_gettersList);

	//qc_assert(getter);
	return getter;
}


int qc_getterslist_remove(QcGettersList *getterList, QcGetter *getter) {
	if (NULL == getterList || NULL == getter) {
		return -1;
	}

	QcListEntry *listEntry = getter->_entry;
	qc_list_w_lock(getterList->_gettersList);
	int ret = qc_list_removeentry(getterList->_gettersList, listEntry);
	qc_list_w_unlock(getterList->_gettersList);

	qc_assert(ret == 0);
	return 0;
}

