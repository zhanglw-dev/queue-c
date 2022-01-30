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

#include "qc_shq_def.h"
#include "qc_shq_que.h"


QcShmQue* qc_shqueue_attach(const char *shm_name, const char *que_name, QcErr *err)
{
    QcShm* qcShm;
    QcShmQue* qcShmQue;

    qcShm = qc_shm_open(shm_name, err);
    if (NULL == qcShm)
    {
        return NULL;
    }

    qcShmQue = shm_queue_find(qcShm, que_name, err);
    if(NULL == qcShmQue)
    {
        qc_shm_close(qcShm);
        return NULL;
    }

    return qcShmQue;
}


void qc_shqueue_deattach(QcShmQue *shmQue)
{
    shm_queue_release(shmQue);
}


int qc_shqueue_pull_begin(QcShmQue *shmQue, int wait_msec, int *idx, char **pp_buff, int *p_bufflen, QcErr *err)
{
    return shm_queue_pull_begin(shmQue, wait_msec, idx, pp_buff, p_bufflen, err);
}


int qc_shqueue_pull_end(QcShmQue *shmQue, int idx, QcErr *err)
{
    return shm_queue_pull_end(shmQue, idx, err);
}


int qc_shqueue_push_begin(QcShmQue *shmQue, int *idx, char **pp_buff, int *p_bufflen, QcErr *err)
{
    return shm_queue_push_begin(shmQue, idx, pp_buff, p_bufflen, err);
}


int qc_shqueue_push_end(QcShmQue *shmQue, int idx, int bufflen, QcErr *err)
{
    return shm_queue_push_end(shmQue, idx, bufflen, err);
}
