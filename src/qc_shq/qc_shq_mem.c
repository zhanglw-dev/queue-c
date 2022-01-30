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
#include "qc_list.h"

#include "qc_shm.h"
#include "qc_log.h"
#include "qc_shq_mem.h"
#include "qc_shq_que.h"
#include "qc_shq_def.h"
#include "qc_shq_conf.h"


size_t calc_shmsize(QcList *queConfList, QcErr *err)
{
    size_t shm_size;

    shm_size = 0;
    shm_size += sizeof(P_Shmem);

    unsigned int count = qc_list_count(queConfList);
    shm_size += sizeof(P_ShmQue) * count;

    qc_list_enumbegin(queConfList);
    while(1)
    {
        QcListEntry *entry = qc_list_enumentry(queConfList);
        if(NULL == entry)
            break;

        QueConf *queConf = qc_list_data(entry);
        int que_size = queConf->queuesize;
        int msg_size = queConf->msgsize;

        shm_size += que_size * sizeof(_StaticCell) * 2;  //used_list and free_list
        shm_size += que_size * (sizeof(off_t) + msg_size);

        shm_size += QUEDATA_SAFE_MARGIN_SIZE * 2; //safe margin
    }

    return shm_size;
}


QcShm* create_shm(const char* shm_name, size_t shm_size, QcErr *err)
{
	QcShm *qcShm = qc_shm_create(shm_name, shm_size, err);
	if (NULL == qcShm)
		return NULL;

	return qcShm;
}


int init_shmhead(QcShqMem *shqMem, QcErr *err)
{
    P_Shmem *p_Shmem = (P_Shmem*)qc_shm_getaddr(shqMem->qcShm);
    p_Shmem->status = 0;
    p_Shmem->quenum = shqMem->que_num;

    shqMem->tmp_offset += sizeof(P_Shmem);

    return 0;
}


int init_all_shmque(QcShqMem *shqMem, QcList *queConfList, QcErr *err)
{
    qc_list_enumbegin(queConfList);
    for(unsigned int i=0; i<qc_list_count(queConfList); i++)
    {
        QcListEntry *entry = qc_list_enumentry(queConfList);
        if(NULL == entry)
            break;

        QueConf *queConf = qc_list_data(entry);

		qc_pinfo("  init shmque (%s) start...\n", queConf->qname);
		QcShmQue *shmQue = shm_queue_init(shqMem, &shqMem->tmp_offset, queConf, shqMem->que_num, i, err);
        if(NULL == shmQue)
        {
			qc_pinfo("  init shmque (%s) failed, err=%s\n", queConf->qname, err->desc);
            return -1;
        }

		qc_list_inserttail(shqMem->shmQueLst, shmQue);
        
		qc_pinfo("  init shmque (%s) succeed!\n", queConf->qname);
    }

    return 0;
}


QcShqMem* qc_shqmem_create(QcShmConf *shmConf, QcErr *err)
{
    QcShqMem *shqMem;
    qc_malloc(shqMem, sizeof(QcShqMem));
    
    memset(shqMem->shm_name, 0, QC_SHMNAME_MAXLEN);
    shqMem->tmp_offset = 0;
    shqMem->que_num = qc_list_count(shmConf->queConfList);

    strncpy((char*)shqMem->shm_name, shmConf->shmname, QC_SHMNAME_MAXLEN);

	shqMem->shmQueLst = qc_list_create(0);

    size_t shm_size = calc_shmsize(shmConf->queConfList, err);
    shqMem->shm_size = shm_size;

    QcShm *qcShm = create_shm(shqMem->shm_name, shm_size, err);
    if(NULL == qcShm)
    {
        return NULL;
    }

    shqMem->qcShm = qcShm;

    if(0 != init_shmhead(shqMem, err))
    {
        return NULL;
    }

    if(0 != init_all_shmque(shqMem, shmConf->queConfList, err))
    {
        return NULL;
    }

    //be sure final offset is right
    if(shqMem->shm_size != shqMem->tmp_offset)
    {
		qc_pinfo("offset (%d != %d) error, shm failed.\n", (int)shqMem->shm_size, (int)shqMem->tmp_offset);
        return NULL;
    }

    return shqMem;
}


int qc_shqmem_destroy(QcShqMem *shmHdl)
{
    if(qc_shm_destroy(shmHdl->qcShm) < 0)
    {
		qc_pinfo("shm_unlink (%s) failed.\n", (char*)shmHdl->shm_name);
        return -1;
    }

	qc_list_enumbegin(shmHdl->shmQueLst);
	for (unsigned int i = 0; i < qc_list_count(shmHdl->shmQueLst); i++)
	{
		QcListEntry *entry = qc_list_enumentry(shmHdl->shmQueLst);
		if (NULL == entry)
			break;

		QcShmQue *shmQue = qc_list_data(entry);
		shm_queue_release(shmQue);
	}

    return 0;
}
