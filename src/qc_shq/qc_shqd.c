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
#include "qc_shq_config.h"
#include "qc_shq_mem.h"
#include "qc_list.h"


QcList *_shmConfList;
QcList *_shmemHdlList;


static void sighandler(int sig)
{
    printf("received exit signal.\n");

    qc_list_enumbegin(_shmConfList);

    while(1)
    {
        QcListEntry* entry = qc_list_enumentry(_shmConfList);
        if(NULL == entry) 
            break;
        
        ShmConf *shmConf = (ShmConf*)qc_list_data(entry);

#ifndef IS_WINDOWS
        if(shm_unlink(shmConf->shmname) == -1)
        {
            fprintf(stderr, "shm_unlink (%s) failed, errno=%d\n", shmConf->shmname, errno);
            exit(-1);
        }
#endif
    }

    exit(0);
}


int main()
{
    QcErr err;
    char binpath[128], filepath[256];
    printf("shmqueue start...\n");

    char *cwd = getcwd(binpath, sizeof(binpath)-1);
    sprintf(filepath, "%s/../etc/shqd_demo.cfg", binpath);
    //printf("config file: %s\n", filepath);

    if(access(filepath, 0) != 0){
        printf("error: can not find config file:%s\n", filepath);
        exit(-1);
    }

    QcShqConf *shmQueConf = qc_shqd_read_config(filepath, &err);
    if(NULL == shmQueConf)
    {
        printf("read config failed, err=%s.\n", err.desc);
        exit(-1);
    }

    _shmConfList = shmQueConf->shmConfList;

    if(signal(SIGINT, sighandler) == SIG_ERR)
    {
        fprintf(stderr, "reg-signal failed, errno=%d\n", errno);
        exit(-1);
    }

    //create shm
    printf("creating all shm......\n");
    _shmemHdlList = qc_shqmem_createall(shmQueConf, &err);

    while(1)
    {
        sleep(1);
    }

    qc_list_destroy(shmQueConf->shmConfList);

    exit(0);
}
