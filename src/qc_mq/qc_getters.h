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

#ifndef H_QC_GETTER
#define H_QC_GETTER

#include "qc_thread.h"
#include "qc_error.h"
#include "qc_message.h"
#include "qc_list.h"
#include "qc_thread.h"
#include "qc_error.h"
#include "qc_message.h"


typedef struct {
	QcCondLock *condlock;
	QcCondition *cond;
	//QcMessage *message;
	int is_timedout;

	QcListEntry *_entry;
}QcGetter;


typedef struct {
	QcList *_gettersList;
}QcGettersList;



#ifdef __cplusplus
extern "C" {
#endif

QcGetter* qc_getter_create();

int qc_getter_destroy(QcGetter *getter);


QcGettersList* qc_getterslist_create();

int qc_getterslist_destroy(QcGettersList *getterList);

int qc_getterslist_push(QcGettersList *getterList, QcGetter *getter);

QcGetter* qc_getterslist_pop(QcGettersList *getterList);

int qc_getterslist_remove(QcGettersList *getterList, QcGetter *getter);

#ifdef __cplusplus
}
#endif


#endif  //H_QC_GETTER
