/*
 * BSD 3-Clause License
 * 
 * Copyright (c) 2019, zhanglw (zhanglw366@163.com)
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "qc_getters.h"
#include "qc_log.h"


QcGetter* qc_getter_create() {
	QcGetter *getter;

	qc_malloc(getter, sizeof(QcGetter));
	if (NULL == getter) {
		qc_error("malloc QcGetter faild.");
		return NULL;
	}

	getter->cond = qc_thread_condition_create();
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

	getter->mutex = qc_thread_mutex_create();
	if (NULL == getter->mutex) {
		qc_error("create getter->mutex failed.");
		qc_free(getter);
		return NULL;
	}

	//getter->message = NULL;
	getter->is_timedout = 0;
	getter->ref_count = 1;

	return getter;
}


int qc_getter_destroy(QcGetter *getter) {
	int ref_count;

	if (NULL == getter) {
		qc_error("invalid params.");
		return -1;
	}

	qc_thread_mutex_lock(getter->mutex);
	ref_count = --getter->ref_count;
	qc_thread_mutex_unlock(getter->mutex);

	if(0 == ref_count){
		qc_thread_condition_destroy(getter->cond);
		qc_thread_condlock_destroy(getter->condlock);
		qc_thread_mutex_destroy(getter->mutex);
		qc_free(getter);
	}

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
	if(getter){
		getter->_entry = NULL;

		qc_thread_mutex_lock(getter->mutex);
		if(getter != NULL) getter->ref_count++;
		qc_thread_mutex_unlock(getter->mutex);
	}

	qc_list_w_unlock(getterList->_gettersList);

	return getter;
}


int qc_getterslist_remove(QcGettersList *getterList, QcGetter *getter) {
	if (NULL == getterList || NULL == getter) {
		return -1;
	}
	qc_list_w_lock(getterList->_gettersList);
	if(NULL == getter || NULL == getter->_entry){
		qc_list_w_unlock(getterList->_gettersList);
		return -1;
	}
	int ret = qc_list_removeentry(getterList->_gettersList, (QcListEntry*)getter->_entry);
	qc_list_w_unlock(getterList->_gettersList);

	qc_assert(ret == 0);
	return 0;
}

