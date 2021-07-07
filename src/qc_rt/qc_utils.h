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

#ifndef QC_UTILS_H
#define QC_UTILS_H

#include "qc_list.h"


#ifdef __cplusplus
extern "C" {
#endif

//////////////////////////////////////////////////////////////////////////////////
//                               Num pool
//////////////////////////////////////////////////////////////////////////////////

struct __QcNumPool {
	QcStaticList qc_freeList;
};

typedef struct __QcNumPool QcNumPool;


int qc_numpool_init(QcNumPool *numPool, int init_count);

void qc_numpool_release(QcNumPool *numPool);

int qc_numpool_count(QcNumPool *numPool);

int qc_numpool_freenum(QcNumPool *numPool);

int qc_numpool_usednum(QcNumPool *numPool);

int qc_numpool_get(QcNumPool *numPool);

int qc_numpool_put(QcNumPool *numPool, int idx);



//////////////////////////////////////////////////////////////////////////////////
//                               Unit Pool
//////////////////////////////////////////////////////////////////////////////////

struct __QcUnitPool {
	QcNumPool numPool;
	int unit_size;
	int unit_count;
	char *unit_buff;
};


typedef struct __QcUnitPool QcUnitPool;


int qc_unitpool_init(QcUnitPool *unitPool, int unit_size, int unit_num);

void qc_unitpool_release(QcUnitPool *unitPool);

void* qc_unitpool_get(QcUnitPool *unitPool, int *idx);

int qc_unitpool_put(QcUnitPool *unitPool, int idx);

int qc_unitpool_count(QcUnitPool *unitPool);

int qc_unitpool_usednum(QcUnitPool *unitPool);

int qc_unitpool_freenum(QcUnitPool *unitPool);

void* qc_unitpool_ptr_byindex(QcUnitPool *unitPool, int idx);



/*////////////////////////////////////////////////////////////////////////////////
//                               No-Block Queue
////////////////////////////////////////////////////////////////////////////////*/

typedef struct __QcNBlockQue QcNBlockQue;


QcNBlockQue* qc_nblockque_create(int limit, int with_rwlock);

void qc_nblockque_destroy(QcNBlockQue *queue);

/* 0: ok  QC_FULLED: can not put because full  -1: error */
int qc_nblockque_put(QcNBlockQue *queue, void *ptr);

int qc_nblockque_get(QcNBlockQue *queue, void **pptr);



/*////////////////////////////////////////////////////////////////////////////////
//                               Block Queue
////////////////////////////////////////////////////////////////////////////////*/

typedef struct __QcBlockQue QcBlockQue;


QcBlockQue* qc_blockque_create(int limit);

void qc_blockque_destroy(QcBlockQue *queue);

int qc_blockque_put(QcBlockQue *queue, void *buff);

void* qc_blockque_get(QcBlockQue *queue);


#ifdef __cplusplus
}
#endif



#endif /*QC_UTILS_H*/
