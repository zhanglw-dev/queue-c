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
#include "qc_shq_conf.h"
#include "qc_shq_mem.h"
#include "qc_list.h"


QcShqMem* shqMem;

static void sighandler(int sig)
{
    printf("received exit signal.\n");

    qc_shqmem_destroy(shqMem);

    exit(0);
}


int main()
{
    QcErr err;

    if(signal(SIGINT, sighandler) == SIG_ERR)
    {
        fprintf(stderr, "reg-signal failed, errno=%d\n", errno);
        exit(-1);
    }

    //create shm
    printf("creating shm......\n");
    QcShmConf *shmConf = qc_create_shmem_config("shm_1");
    qc_assert(shmConf);

    qc_shm_config_addque(shmConf, "queue_1", 40, 1*1024*1024);
    qc_shm_config_addque(shmConf, "queue_2", 20, 2*1024*1024);
    qc_shm_config_addque(shmConf, "queue_3", 10, 4*1024*1024);

    shqMem = qc_shqmem_create(shmConf, &err);
    qc_assert(shqMem);

    while(1)
    {
        sleep(1);
    }

}
