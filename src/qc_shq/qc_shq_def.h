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

#ifndef SHMQ_DEF_H
#define SHMQ_DEF_H

#include "qc_prelude.h"
#include "qc_list.h"
#include "qc_sem.h"
#include "qc_shm.h"
#include "qc_thread.h"

#define QUENAME_MAXLEN 32


#pragma pack(push)
#pragma pack(1)


typedef struct {
	int status;
	unsigned int quenum;
}P_Shmem;


typedef struct{
    char quename[QUENAME_MAXLEN];
    int que_sn;
    int que_size;
    int msg_size;

    char semname_que[QC_SEMNAME_MAXLEN];
    char semname_lst[QC_SEMNAME_MAXLEN];

    QcStaticList lst_free;
	QcStaticList lst_used;

    off_t data_offset;
    off_t next_offset;
}P_ShmQue;


#pragma pack(pop)

typedef struct __QcShmQue__ {
    QcShm* qcShm;
    P_ShmQue* p_shmQue;
    QcSem* sem_que;
    QcSem* sem_lst;
}QcShmQue;

typedef struct _ShmQHdl_{
    int que_sn;
    char *shm_addr;
    P_ShmQue *p_shmQue;
}ShmQHdl;


#endif //SHMQ_DEF_H