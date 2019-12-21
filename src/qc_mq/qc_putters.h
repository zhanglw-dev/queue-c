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

#ifndef H_QC_PUTTER
#define H_QC_PUTTER

#include "qc_thread.h"
#include "qc_message.h"
#include "qc_list.h"


typedef struct __QcPutter {
	QcCondLock *condlock;
	QcCondition *cond;
	QcMessage *message;
	int is_timedout;
	int priority;
	QcListEntry *_entry;
}QcPutter;


typedef struct {
	QcList *puttersChain;
	int pop_counter;
}QcPutBucket;


typedef struct {
	QcPutBucket **putBuckets;
	int bucket_count;
	int cursor_bucketsn;
	int putter_count;
}QcPuttersChain;



#ifdef __cplusplus
extern "C" {
#endif

QcPutter* qc_putter_create();

int qc_putter_destroy(QcPutter *putter);

//----------------------------------------------------------------------
QcPutBucket* qc_putbucket_create();

int qc_putbucket_destroy(QcPutBucket *putBucket);

//----------------------------------------------------------------------
QcPuttersChain* qc_putterschain_create(int bucket_count);

int qc_putterschain_destroy(QcPuttersChain *puttersChain);

int qc_putterschain_push(QcPuttersChain *puttersChain, QcPutter *putter);

QcPutter* qc_putterschain_pop(QcPuttersChain *puttersChain);

int qc_putterschain_remove(QcPuttersChain *puttersChain, QcPutter *putter);


#ifdef __cplusplus
}
#endif


#endif  //H_QC_PUTTER
