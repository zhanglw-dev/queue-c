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

#ifndef QC_QSYSTEM_H
#define QC_QSYSTEM_H

#include "qc_queue.h"
#include "qc_error.h"


#ifdef __cplusplus
extern "C" {
#endif


typedef struct __QcQSystem QcQSystem;


QcQSystem* qc_qsys_create();

void qc_qsys_destory(QcQSystem *qSys);

int qc_qsys_addqueue(QcQSystem *qSys, const char *qname, QcQueue *queue, QcErr *err);

int qc_qsys_delqueue(QcQSystem *qSys, const char *qname, QcErr *err);

QcQueue* qc_qsys_queue_get(QcQSystem *qSys, const char *qname, QcErr *err);

int qc_qsys_putmsg(QcQSystem *qSys, const char *qname, QcMessage *message, int msec, QcErr *err);

QcMessage* qc_qsys_getmsg(QcQSystem *qSys, const char *qname, int msec, QcErr *err);


#ifdef __cplusplus
}
#endif


#endif //QC_QSYSTEM_H
