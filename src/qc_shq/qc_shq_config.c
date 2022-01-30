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

#include "qc_shq_config.h"


QcShmConf* qc_create_shmem_config(const char *shmname)
{
    qc_assert(strlen(shmname) < QC_SHMNAME_MAXLEN);

    QcShmConf *shmConf;
    qc_malloc(shmConf, sizeof(QcShmConf));
    strncpy(shmConf->shmname, shmname, QC_SHMNAME_MAXLEN);

    shmConf->queConfList = qc_list_create(0);
    qc_assert(shmConf->queConfList);

    return shmConf;
}


void qc_shm_config_addque(QcShmConf *shmConf, const char *qname, int queuesize, off_t msgsize)
{
    qc_assert(shmConf);
    qc_assert(strlen(qname) < QC_QUENAME_MAXLEN);
    qc_assert(queuesize>0);
    qc_assert(msgsize>0);

    QueConf *queConf;
    qc_malloc(queConf, sizeof(QueConf));
    strncpy(queConf->qname, qname, QC_QUENAME_MAXLEN);
    queConf->queuesize = queuesize;
    queConf->msgsize = msgsize;

    qc_list_inserttail(shmConf->queConfList, queConf);
}
