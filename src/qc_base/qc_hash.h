#ifndef QCLIB_HASH_H
#define QCLIB_HASH_H

#ifdef __cplusplus
extern "C" {
#endif


/*////////////////////////////////////////////////////////////////////////////////
//                               Hash Table
////////////////////////////////////////////////////////////////////////////////*/


typedef struct __HashEntry HashEntry;
typedef struct __QcHashTbl QcHashTbl;

typedef unsigned int (*HASH_HASHFUNC)(void *key, unsigned int bucket_num);
typedef int  (*HASH_COMPAREFUNC)(void *key1, void *key2);
typedef void (*HASH_DESTROYFUNC)(void *key, void *data);


QcHashTbl* qc_hashtbl_create(unsigned int bucket_count, int with_rwlock, HASH_HASHFUNC HashFunc, \
                             HASH_COMPAREFUNC CompareFunc, HASH_DESTROYFUNC DestroyFunc);
void qc_hashtbl_destroy(QcHashTbl *hashtbl);

int qc_hashtbl_count(QcHashTbl *hashtbl);

int qc_hashtbl_insert(QcHashTbl *hashtbl, void *key, void *data);
int qc_hashtbl_delete(QcHashTbl *hashtbl, void *key);
void* qc_hashtbl_find(QcHashTbl *hashtbl, void *key);

void  qc_hashtbl_enumbegin(QcHashTbl *hashtbl);
void* qc_hashtbl_enumnext(QcHashTbl *hashtbl);

//unsigned int qc_hashint(void *key, unsigned int bucket_count);
unsigned int qc_hashstring(void *str, unsigned int bucket_count);
unsigned int qc_hashbin(void *key, unsigned int uLength, unsigned int bucket_count);
unsigned int qc_bincompare( void *str1, int str1_len, void *str2, int str2_len );
int qc_strcompare( void *str1, void *str2 );

void qc_hashtbl_w_lock(QcHashTbl *hashtbl);
void qc_hashtbl_w_unlock(QcHashTbl *hashtbl);
void qc_hashtbl_r_lock(QcHashTbl *hashtbl);
void qc_hashtbl_r_unlock(QcHashTbl *hashtbl);


#ifdef __cplusplus
}
#endif


#endif /*QCLIB_LIST_H*/
