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

#include "qc_prelude.h"
#include "qc_message.h"


#pragma pack(push)
#pragma pack(1)

//must as same as qc_persist_file.h::Qc_MsgRecord !!!
struct __QcMessage {
	int priority;
	int persist_id;
	int bufflen;
	char *buff;
};

#pragma pack(pop)



QcMessage* qc_message_create(const char *buff, int len, int do_allocbuff){
    QcMessage *message;

    qc_malloc(message, sizeof(QcMessage));
    if(NULL == message)
        return NULL;
    
    if(NULL == (char *)buff) len = 0;

    if(do_allocbuff){
        qc_malloc(message->buff, len);
        if(NULL == message->buff){
            qc_free(message);
            return NULL;
        }
        memcpy(message->buff, buff, len);
    }
    else{
        message->buff = (char *)buff;
    }

    message->bufflen = len;
    message->priority = 1;

    return message;
}


void qc_message_release(QcMessage *message, int do_freebuff){
    if(NULL == message)
        return;

    if(do_freebuff && message->buff) 
		qc_free(message->buff);

    qc_free(message);

    return;
}


char* qc_message_buff(QcMessage *message){
    qc_assert(message);
    return message->buff;
}


int qc_message_bufflen(QcMessage *message){
    qc_assert(message);    
    return message->bufflen;
}


void qc_message_setpriority(QcMessage *message, int priority){
    qc_assert(message);
    qc_assert(priority>=0)

    message->priority = priority;
}


int qc_message_priority(QcMessage *message){
    qc_assert(message);
    return message->priority;
}

