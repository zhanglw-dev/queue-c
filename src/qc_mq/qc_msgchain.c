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

#include "qc_msgchain.h"
#include "qc_list.h"
#include "qc_thread.h"



QcMsgBucket* qc_msgbucket_create() {

	QcMsgBucket *msgBucket;
	qc_malloc(msgBucket, sizeof(QcMsgBucket));
	if (NULL == msgBucket) {
		return NULL;
	}
	QcList *msgList = qc_list_create(0);
	if (NULL == msgList) {
		qc_free(msgBucket);
		return NULL;
	}

	msgBucket->msgList = msgList;
	return msgBucket;
}


int qc_msgbucket_destroy(QcMsgBucket *msgBucket) {
	if (NULL == msgBucket) {
		return -1;
	}

	if (msgBucket->msgList) qc_list_destroy(msgBucket->msgList);
	qc_free(msgBucket);
	return 0;
}

//----------------------------------------------------------------------------------------------

QcMsgChain* qc_msgchain_create(int count_limit, int priority_maxlevel)
{
    if(count_limit <= 0){
        return NULL;
    }
    if(priority_maxlevel <= 0){
        return NULL;
    }

	QcMsgChain *msgChain;
	qc_malloc(msgChain, sizeof(QcMsgChain));
    if(NULL == msgChain){
        return NULL;
    }

    memset(msgChain, 0, sizeof(QcMsgChain));
    msgChain->count_limit  = count_limit;
	msgChain->msg_count = 0;
	msgChain->bucket_count = priority_maxlevel;
    msgChain->cursor_bucketsn = 0;

    qc_malloc(msgChain->msgBuckets, sizeof(QcMsgBucket*)*msgChain->bucket_count);
    if(NULL == msgChain->msgBuckets){
        goto failed;
    }
    memset(msgChain->msgBuckets, 0, sizeof(QcMsgBucket*)*msgChain->bucket_count);

    for(int i=0; i<msgChain->bucket_count; i++){
        QcMsgBucket *msgBucket = qc_msgbucket_create();
        if(NULL == msgBucket){
            goto failed;
        }
		msgBucket->pop_counter = 0;
		msgChain->msgBuckets[i] = msgBucket;
    }

	msgChain->tempList = qc_list_create(1);

    return msgChain;

failed:
    qc_msgchain_destroy(msgChain);
    return NULL;
}


int qc_msgchain_destroy(QcMsgChain *msgChain){

    if(NULL == msgChain)
        return -1;

    if(msgChain->msgBuckets){
        for(int i=0; i<msgChain->bucket_count; i++){
            if(msgChain->msgBuckets[i])
                qc_free(msgChain->msgBuckets[i]);
        }
        qc_free(msgChain->msgBuckets);
    }

    return 0;
}


int qc_msgchain_msgcount(QcMsgChain *msgChain)
{
	qc_assert(NULL != msgChain);
    return msgChain->msg_count;
}


int qc_msgchain_pushmsg(QcMsgChain *msgChain, QcMessage *message)
{
    qc_assert(msgChain!=NULL && message!=NULL);
	qc_assert(msgChain->msg_count < msgChain->count_limit);

    int bucket_sn = qc_message_priority(message)-1;
    qc_assert(bucket_sn >= 0 && bucket_sn <= (msgChain->bucket_count-1));

	QcMsgBucket *bucket = msgChain->msgBuckets[bucket_sn];
    qc_list_inserttail(bucket->msgList, message);
	msgChain->msg_count++;

    if(bucket_sn > msgChain->cursor_bucketsn)
        msgChain->cursor_bucketsn = bucket_sn;

    return 0;
}


QcMessage* qc_msgchain_popmsg(QcMsgChain *msgChain){

	qc_assert(NULL!=msgChain);
	qc_assert(msgChain->msg_count > 0);

    int bucket_sn = msgChain->cursor_bucketsn;

    QcMsgBucket *bucket;  //must before loop??

    //locate the cursor_bucketsn
    while(1){
        bucket = msgChain->msgBuckets[bucket_sn];
        if(qc_list_count(bucket->msgList) > 0){
            msgChain->cursor_bucketsn = bucket_sn;
            break;
        }
        bucket_sn--;
        qc_assert(bucket_sn>=0);
    }

    int cycle = 0;

try_loop:

    bucket = msgChain->msgBuckets[bucket_sn];
    int counter_max = bucket_sn+1;

    if(bucket->pop_counter < counter_max){
        //
        if(qc_list_count(bucket->msgList) > 0){
            QcMessage *message = qc_list_pophead(bucket->msgList);
            bucket->pop_counter++;
			msgChain->msg_count--;
            return message;
        }
        else{
            bucket_sn--;
            qc_assert(bucket_sn>=0);
            goto try_loop;
        }
    }
    else{
		//no message in other bucket...
		if (qc_list_count(msgChain->msgBuckets[bucket_sn]->msgList) == msgChain->msg_count) {
			QcMessage *message = qc_list_pophead(bucket->msgList);
			msgChain->msg_count--;
			return message;
		}

        bucket_sn--;
        if(bucket_sn < 0){
            bucket_sn = msgChain->bucket_count - 1;
            assert(cycle == 0);  //be sure just loop one cycle
            cycle++;
        }
        bucket->pop_counter = 0;
        goto try_loop;
    }

}


int qc_msgchain_forcepush(QcMsgChain *msgChain, QcMessage *message, QcMessage **msg_popped)
{
	int ret;
    qc_assert(msgChain!=NULL && message!=NULL);

	if (msgChain->msg_count < msgChain->count_limit) {
		ret = qc_msgchain_pushmsg(msgChain, message);
		if(msg_popped) *msg_popped = NULL;
		return ret;
	}

	for (int bucket_sn = 0; bucket_sn < msgChain->bucket_count; bucket_sn++) {
		QcMsgBucket *bucket = msgChain->msgBuckets[bucket_sn];
        if(qc_list_count(bucket->msgList) > 0){
            QcMessage *popmsg = qc_list_pophead(bucket->msgList);
            if(msg_popped){
                *msg_popped = popmsg;
            }
            else{
                qc_message_release(popmsg);
            }
			msgChain->msg_count--;
            ret = qc_msgchain_pushmsg(msgChain, message);
            return ret;
        }
	}

    return -1;
}


//for putter
int qc_msgchain_puttemp(QcMsgChain *msgChain, QcMessage *message)
{
	qc_list_w_lock(msgChain->tempList);
	qc_list_inserttail(msgChain->tempList, message);
	qc_list_w_unlock(msgChain->tempList);
	return 0;
}


//for getter
QcMessage* qc_msgchain_gettemp(QcMsgChain *msgChain)
{
	QcMessage *message = NULL;
	qc_list_w_lock(msgChain->tempList);
	message = qc_list_pophead(msgChain->tempList);
	qc_list_w_unlock(msgChain->tempList);

	return message;
}
