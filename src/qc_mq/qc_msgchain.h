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

#ifndef H_QC_MSGCHAIN
#define H_QC_MSGCHAIN

#include "qc_prelude.h"
#include "qc_message.h"
#include "qc_list.h"
#include "qc_utils.h"
#include "qc_thread.h"


typedef struct {
	QcList *msgList;
	int pop_counter;
}QcMsgBucket;


typedef struct{
    int count_limit;    //limit of message count
	int msg_count;      //current message count

    int bucket_count;
    QcMsgBucket **msgBuckets;
    int cursor_bucketsn;

	QcList *tempList;  //for blocking getter
}QcMsgChain;



#ifdef __cplusplus
extern "C" {
#endif


QcMsgBucket* qc_msgbucket_create();

int qc_msgbucket_destroy(QcMsgBucket *msgBucket);

//-------------------------------------------------------------------------------------

QcMsgChain* qc_msgchain_create(int count_limit, int priority_max);

int qc_msgchain_destroy(QcMsgChain *msgChain);

int qc_msgchain_msgcount(QcMsgChain *msgChain);

int qc_msgchain_pushmsg(QcMsgChain *msgChain, QcMessage *message);

QcMessage* qc_msgchain_popmsg(QcMsgChain *msgChain);

int qc_msgchain_forcepush(QcMsgChain *msgChain, QcMessage *message, QcMessage **msg_popped);

int qc_msgchain_puttemp(QcMsgChain *msgChain, QcMessage *message);  //for putter

QcMessage* qc_msgchain_gettemp(QcMsgChain *msgChain);  //for getter


#ifdef __cplusplus
}
#endif


#endif /*H_QC_MSGCHAIN*/
