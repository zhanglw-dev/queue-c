/*
 * BSD 3-Clause License
 * 
 * Copyright (c) 2021, zhanglw (zhanglw366@163.com)
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

#ifndef QC_SHM_QUEUE_H
#define QC_SHM_QUEUE_H

#include "qc_prelude.h"
#include "qc_error.h"
#include "qc_shq_def.h"

#include "qc_shm.h"
#include "qc_shq_mem.h"
#include "qc_shq_conf.h"


#define QUEDATA_SAFE_MARGIN_SIZE 512



QcShmQue* shm_queue_init(QcShqMem *shmHdl, off_t *offset, QueConf *queConf, int que_num, int sn, QcErr *err);

void shm_queue_destroy(QcShmQue *shmQue);

QcShmQue* shm_queue_find(QcShm *qcShm, const char *que_name, QcErr *err);

void shm_queue_release(QcShmQue *shmQue);

int shm_queue_pull_begin(QcShmQue *shmQue, int wait_msec, int *idx, char **pp_buff, int *p_bufflen, QcErr *err);

int shm_queue_pull_end(QcShmQue *shmQue, int idx, QcErr *err);

//pre alloc from que for put
int shm_queue_push_begin(QcShmQue *shmQue, int *idx, char **pp_buff, int *p_bufflen, QcErr *err);

int shm_queue_push_end(QcShmQue *shmQue, int idx, int bufflen, QcErr *err);

#endif //QC_SHM_QUEUE_H
