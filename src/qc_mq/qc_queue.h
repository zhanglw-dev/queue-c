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

#ifndef H_QC_MQUEUE
#define H_QC_MQUEUE

#include "qc_error.h"
#include "qc_message.h"


#ifdef __cplusplus
extern "C" {
#endif


typedef struct __QcQueue QcQueue;


QcQueue* qc_queue_create(unsigned int limit, unsigned int priority_max, QcErr *err);

int qc_queue_destroy(QcQueue *queue);

unsigned int qc_queue_msgcount(QcQueue *queue);

int qc_queue_msgput(QcQueue *queue, QcMessage *message, int msec, QcErr *err);

QcMessage* qc_queue_msgget(QcQueue *queue, int msec, QcErr *err);

int qc_queue_forceput(QcQueue *queue, QcMessage *message, QcMessage **msg_popped, QcErr *err);


#ifdef __cplusplus
}
#endif


#endif /*H_QC_QUEUE*/
