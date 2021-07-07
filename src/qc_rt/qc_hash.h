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
