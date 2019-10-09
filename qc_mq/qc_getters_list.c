#include "qc_getters_list.h"
#include "qc_log.h"
#include "qc_list.h"


//--------------------------------------------------------------------------------

QcGettersList* qc_getterslist_create(){
	QcGettersList *getterList;
	qc_malloc(getterList, sizeof(QcGettersList));
    if(NULL == getterList){
        return NULL;
    }

    QcList *list = qc_list_create(1);
    if(NULL == list){
        qc_free(list);
        return NULL;
    }

    getterList->_gettersList = list;
    return getterList;
}


int qc_getterslist_destroy(QcGettersList *getterList){
    if(NULL == getterList){
        return -1;
    }

    if(getterList->_gettersList){
        qc_list_destroy(getterList->_gettersList);
    }

    qc_free(getterList);

    return 0;
}


int qc_getterslist_push(QcGettersList *getterList, QcGetter *getter){
    if(NULL == getterList || NULL == getter){
        return -1;
    }

    QcListEntry *listEntry;

    qc_list_w_lock(getterList->_gettersList);
    int ret = qc_list_inserttail2(getterList->_gettersList, getter, &listEntry);
    if(ret < 0){
        qc_list_w_unlock(getterList->_gettersList);
        return -1;
    }

    getter->_entry = listEntry;
    qc_list_w_unlock(getterList->_gettersList);
    return 0;
}


QcGetter* qc_getterslist_pop(QcGettersList *getterList){
    QcGetter *getter;
    qc_list_w_lock(getterList->_gettersList);
    getter = qc_list_pophead(getterList->_gettersList);
    qc_list_w_unlock(getterList->_gettersList);

    //qc_assert(getter);
    return getter;
}


int qc_getterslist_remove(QcGettersList *getterList, QcGetter *getter){
    if(NULL == getterList || NULL == getter){
        return -1;
    }

    QcListEntry *listEntry = getter->_entry;
    qc_list_w_lock(getterList->_gettersList);
    int ret = qc_list_removeentry(getterList->_gettersList, listEntry);
    qc_list_w_unlock(getterList->_gettersList);

    qc_assert(ret == 0);
    return 0;
}

