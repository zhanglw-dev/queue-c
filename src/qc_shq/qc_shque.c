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

#include "qc_shque.h"
#include "qc_shm.h"
#include "qc_shq_mem.h"


QcShmQue* shm_queue_init(QcShqMem *shqMem, off_t *offset, QueConf *queConf, int que_num, int sn, QcErr *err)
{
    
    P_ShmQue *p_shmQue = (P_ShmQue*)(qc_shm_getaddr(shqMem->qcShm) + *offset);
    *offset += sizeof(P_ShmQue);

    strncpy(p_shmQue->quename, queConf->qname, QUENAME_MAXLEN);
    int que_size = queConf->queuesize;
    int msg_size = queConf->msgsize;

    p_shmQue->que_sn = sn;
    p_shmQue->que_size = que_size;
    p_shmQue->msg_size = msg_size;
    sprintf(p_shmQue->semname_que, "%s__%s__%s", (char*)shqMem->shm_name, queConf->qname, "semque");
    sprintf(p_shmQue->semname_lst, "%s__%s__%s", (char*)shqMem->shm_name, queConf->qname, "semlst");

    //static list of used
    if(0 != qc_staticlist_init(&p_shmQue->lst_used, que_size, qc_shm_getaddr(shqMem->qcShm) + *offset))
    {
        return NULL;
    }
    qc_staticlist_clear(&p_shmQue->lst_used);
    *offset += queConf->queuesize * sizeof(_StaticCell);

    //static list of free
    if(0 != qc_staticlist_init(&p_shmQue->lst_free, que_size, qc_shm_getaddr(shqMem->qcShm) + *offset))
    {
        return NULL;
    }
    *offset += queConf->queuesize * sizeof(_StaticCell);

    //safe margin of top
    *offset += QUEDATA_SAFE_MARGIN_SIZE;

    //msg data region
    p_shmQue->data_offset = *offset;
    *offset += que_size*(sizeof(off_t)+msg_size);

    //safe margin of bottom
    *offset += QUEDATA_SAFE_MARGIN_SIZE;

    //printf("next_offset: %d\n", *offset);
    if((sn+1) == que_num)
        p_shmQue->next_offset = -1;
    else
        p_shmQue->next_offset = *offset;
    
	QcShmQue *shmQue = malloc(sizeof(QcShmQue));
	shmQue->p_shmQue = p_shmQue;
    shmQue->qcShm = shqMem->qcShm;

	//create sem
	shmQue->sem_que = qc_sem_create(p_shmQue->semname_que, 0, err);
	if (NULL == shmQue->sem_que)
	{
		free(shmQue);
		return NULL;
	}
	shmQue->sem_lst = qc_sem_create(p_shmQue->semname_lst, 1, err);
	if (NULL == shmQue->sem_lst)
	{
		qc_sem_destroy(shmQue->sem_que);
		free(shmQue);
		return NULL;
	}

    return shmQue;
}


void shm_queue_destroy(QcShmQue *shmQue)
{
	qc_sem_destroy(shmQue->sem_que);
	qc_sem_destroy(shmQue->sem_lst);
	qc_free(shmQue);
}


QcShmQue* shm_queue_find(QcShm *qcShm, const char *que_name, QcErr *err)
{
    off_t offset;
    P_ShmQue *p_shmQue;

    offset = sizeof(P_Shmem);
    p_shmQue = (P_ShmQue*)(qc_shm_getaddr(qcShm) + offset);

    while(1)
    {
        if(strcmp(p_shmQue->quename, que_name) == 0)
        {
            QcShmQue* shmQue = malloc(sizeof(QcShmQue));
            shmQue->p_shmQue = p_shmQue;

            shmQue->sem_que = qc_sem_open(p_shmQue->semname_que, err);
            if (NULL == shmQue->sem_que)
            {
                free(shmQue);
                return NULL;
            }

            shmQue->sem_lst = qc_sem_open(p_shmQue->semname_lst, err);
            if (NULL == shmQue->sem_lst)
            {
                qc_sem_destroy(shmQue->sem_que);
                free(shmQue);
                return NULL;
            }

            shmQue->qcShm = qcShm;
            return shmQue;
        }

        if(-1 == p_shmQue->next_offset)
        {
            qc_seterr(err, -1, "que_name (%s) not found in shm.", que_name);
            return NULL;
        }

        p_shmQue = (P_ShmQue*)((char*)qc_shm_getaddr(qcShm) + p_shmQue->next_offset);
    }
}


void shm_queue_release(QcShmQue *shmQue)
{
	qc_sem_close(shmQue->sem_que);
	qc_sem_close(shmQue->sem_lst);
	qc_free(shmQue);
}


int shm_queue_pull_begin(QcShmQue* shmQue, int wait_msec, int *idx, char **pp_buff, int *p_bufflen, QcErr *err)
{
    int index;
    int ret, count;
    off_t buf_offset;

    count = 0;

    if(wait_msec < 0)
    {
        //printf("sem wait start.\n");
        ret = qc_sem_wait(shmQue->sem_que, -1);
        //printf("sem wait end.\n");
    }
    else
    {
        //printf("sem timed wait start.\n");
        ret = qc_sem_wait(shmQue->sem_que, wait_msec);
        //printf("sem timed wait end.\n");
        if(QC_ERR_TIMEOUT == ret)
        {
            qc_seterr(err, -100, "time out");
            return -1;
        }
        else if(ret < 0)
        {
            qc_seterr(err, -1, "sem timed wait failed.");
            return -1;
        }
    }

    qc_sem_wait(shmQue->sem_lst, -1);

	index = qc_staticlist_get_head(&shmQue->p_shmQue->lst_used);
	if (index < 0)
	{
		printf("error happend when get from queue->qc_staticlist_used.\n");
        qc_sem_post(shmQue->sem_lst);
		return -1;
	}

    qc_sem_post(shmQue->sem_lst);

    *idx = index;
    buf_offset = (sizeof(off_t) + shmQue->p_shmQue->msg_size) * index;
    *pp_buff = (char*)qc_shm_getaddr(shmQue->qcShm) + shmQue->p_shmQue->data_offset + buf_offset;
    *p_bufflen = *(int*)(*pp_buff);
    *pp_buff += sizeof(off_t);

    return 0;
}


int shm_queue_pull_end(QcShmQue* shmQue, int idx, QcErr *err)
{
    int index;

    qc_sem_wait(shmQue->sem_lst, -1);
	index = qc_staticlist_add_tail(&shmQue->p_shmQue->lst_free, idx);
	qc_assert(index>=0);
    qc_sem_post(shmQue->sem_lst);

    return 0;
}


int shm_queue_push_begin(QcShmQue* shmQue, int *idx, char **pp_buff, int *p_bufflen, QcErr *err)
{
    int index;
    off_t buf_offset;

    qc_sem_wait(shmQue->sem_lst, -1);

	if (0 == qc_staticlist_count(&shmQue->p_shmQue->lst_free))
	{
		qc_seterr(err, -1, "alloc failed, no free unit in queue(%s).", shmQue->p_shmQue->quename);
        qc_sem_post(shmQue->sem_lst);
        return -1;
	}

    index = qc_staticlist_get_head(&shmQue->p_shmQue->lst_free);
	qc_assert(index>=0);

    qc_sem_post(shmQue->sem_lst);

    *idx = index;
    buf_offset = (sizeof(off_t) + shmQue->p_shmQue->msg_size) * index;
    *pp_buff = (char*)qc_shm_getaddr(shmQue->qcShm) + shmQue->p_shmQue->data_offset + buf_offset;
    *p_bufflen = shmQue->p_shmQue->msg_size;
    *((int*)*pp_buff) = shmQue->p_shmQue->msg_size;
    *pp_buff += sizeof(off_t);

    return 0;
}


int shm_queue_push_end(QcShmQue* shmQue, int idx, int bufflen, QcErr *err)
{
    int index;
    off_t buf_offset;
    char *p_buff;
    int *buff_len;

    qc_sem_wait(shmQue->sem_lst, -1);
	index = qc_staticlist_add_tail(&shmQue->p_shmQue->lst_used, idx);
	qc_assert(index>=0);

    buf_offset = (sizeof(off_t) + shmQue->p_shmQue->msg_size) * idx;
    p_buff = (char*)qc_shm_getaddr(shmQue->qcShm) + shmQue->p_shmQue->data_offset + buf_offset;
    buff_len = (int*)p_buff;
    *buff_len = bufflen;

    qc_sem_post(shmQue->sem_que);

    qc_sem_post(shmQue->sem_lst);

    return 0;
}
