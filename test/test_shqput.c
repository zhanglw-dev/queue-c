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

#include "qc_prelude.h"
#include "qc_shque.h"


int main()
{
    int ret;
    QcErr err;
    int idx;
    char* buff;
    int bufflen, msglen;
    QcShmQue *shmQue;
    char *msg = "hello world!";
    
    shmQue = qc_shqueue_attach("shm_1", "queue_2", &err);
    if(NULL == shmQue)
    {
        printf("qc_shqueue_attach failed, err=%s.\n", err.desc);
        exit(-1);
    }


    ret = qc_shqueue_push_begin(shmQue, &idx, &buff, &bufflen, &err);
    if(ret < 0)
    {
        printf("qc_shqueue_push_begin failed, err=%s.\n", err.desc);
        exit(-1);
    }

    msglen = strlen(msg);
    strncpy(buff, msg, msglen);

    ret = qc_shqueue_push_end(shmQue, idx, msglen, &err);
    if (ret < 0)
    {
        printf("qc_shqueue_push_end failed, err=%s.\n", err.desc);
        exit(-1);
    }

    printf("put message succeed, len=%d.\n", msglen);

    qc_shqueue_deattach(shmQue);

    exit(0);
}
