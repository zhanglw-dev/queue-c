#include "qc_getter_list.h"
#include "qc_log.h"
#include "qc_list.h"



QcGetter* qc_getter_create(){
    QcGetter *getter;

    qc_malloc(getter, sizeof(QcGetter));
    if(NULL == getter){
        qc_error("malloc QcGetter faild.");
        return NULL;
    }

    getter->cond = qc_thread_cond_create();
    if(NULL == getter->cond){
        qc_error("create getter->cond failed.");
        qc_free(getter);
        return NULL;
    }

    getter->condlock = qc_thread_condlock_create();
    if(NULL == getter->condlock){
        qc_error("create getter->condlock failed.");
        qc_free(getter);
        return NULL;
    }

    getter->message = NULL;
    getter->is_timedout = 0;
    
    return getter;
}


int qc_getter_destroy(QcGetter *getter){
    if(NULL == getter){
        qc_error("invalid params.");
        return -1;
    }

    qc_thread_cond_destroy(getter->cond);
    qc_thread_condlock_destroy(getter->condlock);
    
    qc_free(getter);

    return 0;
}

//--------------------------------------------------------------------------------

QcGetterList* qc_getterlist_create(){
	QcGetterList *getterList;
	qc_malloc(getterList, sizeof(QcGetterList));
    if(NULL == getterList){
        return NULL;
    }

    QcList *list = qc_list_create(1);
    if(NULL == list){
        qc_free(list);
        return NULL;
    }

    getterList->_getterList = list;
    return getterList;
}


int qc_getterlist_destroy(QcGetterList *getterList){
    if(NULL == getterList){
        return -1;
    }

    if(getterList->_getterList){
        qc_list_destroy(getterList->_getterList);
    }

    qc_free(getterList);

    return 0;
}


int qc_getterlist_push(QcGetterList *getterList, QcGetter *getter){
    if(NULL == getterList || NULL == getter){
        return -1;
    }

    QcListEntry *listEntry;

    qc_list_w_lock(getterList->_getterList);
    int ret = qc_list_inserttail2(getterList->_getterList, getter, &listEntry);
    if(ret < 0){
        qc_list_w_unlock(getterList->_getterList);
        return -1;
    }

    getter->_entry = listEntry;
    qc_list_w_unlock(getterList->_getterList);
    return 0;
}


QcGetter* qc_getterlist_pop(QcGetterList *getterList){
    QcGetter *getter;
    qc_list_w_lock(getterList->_getterList);
    getter = qc_list_pophead(getterList->_getterList);
    qc_list_w_unlock(getterList->_getterList);

    //qc_assert(getter);
    return getter;
}


int qc_getterlist_remove(QcGetterList *getterList, QcGetter *getter){
    if(NULL == getterList || NULL == getter){
        return -1;
    }

    QcListEntry *listEntry = getter->_entry;
    qc_list_w_lock(getterList->_getterList);
    int ret = qc_list_removeentry(getterList->_getterList, listEntry);
    qc_list_w_unlock(getterList->_getterList);

    qc_assert(ret == 0);
    return 0;
}

