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
#include "qc_shm.h"


struct __QcShm__ {
    char name[QC_SHMNAME_MAXLEN+1];
	char *shmaddr;
	size_t shmsize;
};



QcShm* qc_shm_create(const char *name, size_t shmsize, QcErr *err)
{
    int flags, opt, fd;
    mode_t perms;
    size_t size;
    char *addr;

    flags = O_CREAT|O_EXCL|O_RDWR;
    perms = S_IRUSR|S_IWUSR;

    //unlink shm first?
    shm_unlink(name);

    fd = shm_open(name, flags, perms);
    if(fd == -1)
    {
        qc_seterr(err, -1, "create shm failed, name : %s errno=%d\n", name, errno);
        return NULL;
    }

    if(ftruncate(fd, shmsize) == -1)
    {
        qc_seterr(err, -1, "ftruncate failed. errno=%d\n", errno);
        return NULL;
    }

    addr = mmap(NULL, shmsize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(addr == MAP_FAILED)
    {
        qc_seterr(err, -1, "mmap faied, errno=%d\n", errno);
        return NULL;
    }

    close(fd);

    QcShm *qcShm;
    qc_malloc(qcShm, sizeof(QcShm));
    strncpy(qcShm->name, name, QC_SHMNAME_MAXLEN);
    qcShm->shmaddr = addr;
    qcShm->shmsize = shmsize;

    return qcShm;
}


int qc_shm_destroy(QcShm *qcShm)
{
    munmap(qcShm->shmaddr, qcShm->shmsize);
    shm_unlink(qcShm->name);
    return 0;
}


QcShm* qc_shm_open(const char *name, QcErr *err)
{
    int flags, opt, fd;
    mode_t mode;
    char *addr;
    struct stat sb;

    flags = O_RDWR;  //just open
    mode  = S_IRUSR|S_IWUSR;

    fd = shm_open(name, flags, mode);
    if(fd == -1)
    {
        qc_seterr(err, -1, "shm_open failed, name : %s errno=%d\n", name, errno);
        return NULL;
    }

    if(fstat(fd, &sb) == -1)
    {
        qc_seterr(err, -1, "fstat failed, errno=%d\n", errno);
        return NULL;
    }

    addr = mmap(NULL, sb.st_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if(addr == MAP_FAILED)
    {
        qc_seterr(err, -1, "mmap faied, errno=%d\n", errno);
        return NULL;
    }

    close(fd);

    QcShm *qcShm;
    qc_malloc(qcShm, sizeof(QcShm));
    strncpy(qcShm->name, name, QC_SHMNAME_MAXLEN);
    qcShm->shmaddr = addr;
    qcShm->shmsize = sb.st_size;

    return qcShm;
}


int qc_shm_close(QcShm *qcShm)
{
    munmap(qcShm->shmaddr, qcShm->shmsize);
    return 0;
}


size_t qc_shm_getsize(QcShm *qcShm)
{
    return qcShm->shmsize;
}


char* qc_shm_getaddr(QcShm *qcShm)
{
    return qcShm->shmaddr;
}
