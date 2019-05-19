#ifndef QCLIB_LIST_H
#define QCLIB_LIST_H

#ifdef __cplusplus
extern "C" {
#endif


/*////////////////////////////////////////////////////////////////////////////////
//                               Static List
////////////////////////////////////////////////////////////////////////////////*/

typedef struct __QcStaticList QcStaticList;


QcStaticList* qc_staticlist_create(int limit);
void qc_staticlist_destroy(QcStaticList *staticList);

int  qc_staticlist_count(QcStaticList *staticList);
void qc_staticlist_clear(QcStaticList *staticList);

int qc_staticlist_add_head(QcStaticList *staticList, int cell);
int qc_staticlist_add_tail(QcStaticList *staticList, int cell);
int qc_staticlist_add_at(QcStaticList *staticList, int previous_cell, int cell);

int qc_staticlist_get_head(QcStaticList *staticList);
int qc_staticlist_get_tail(QcStaticList *staticList);
int qc_staticlist_get_at(QcStaticList *staticList, int cell);

int qc_staticlist_peek_head(QcStaticList *staticList);
int qc_staticlist_peek_tail(QcStaticList *staticList);
int qc_staticlist_peek_precell(QcStaticList *staticList, int cell);
int qc_staticlist_peek_nextcell(QcStaticList *staticList, int cell);



/*////////////////////////////////////////////////////////////////////////////////
//                              Dynamic List
////////////////////////////////////////////////////////////////////////////////*/

typedef struct __QcListEntry QcListEntry;
typedef struct __QcList QcList;

typedef int (*QC_LIST_COMPAREFUNC)(void *data1, void *data2);

/* with_rwlock: 0->No; 1->Yes */
QcList* qc_list_create(int with_rwlock);
void qc_list_destroy(QcList *list);

unsigned int qc_list_count(QcList *list);

int qc_list_inserthead(QcList *list, void *data);
int qc_list_inserttail(QcList *list, void *data);
int qc_list_inserttail2(QcList *list, void *data, QcListEntry **pp_entry);

void* qc_list_pophead(QcList *list);
void* qc_list_poptail(QcList *list);

void  qc_list_enumbegin(QcList *list);
QcListEntry* qc_list_enumentry(QcList *list);

void* qc_list_data(QcListEntry *entry);
int qc_list_removeentry(QcList *list, QcListEntry *entry);

int qc_list_removefirst(QcList *list, void *data);

int qc_list_insertsort(QcList *list, QC_LIST_COMPAREFUNC CompareFunc);

void qc_list_w_lock(QcList *list);
void qc_list_w_unlock(QcList *list);
void qc_list_r_lock(QcList *list);
void qc_list_r_unlock(QcList *list);


#ifdef __cplusplus
}
#endif


#endif /*QCLIB_LIST_H*/
