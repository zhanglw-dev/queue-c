#include "qc_getter_list.h"
#include "qc_log.h"
#include "qc_list.h"


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

