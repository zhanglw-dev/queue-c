#include "shm_queue.h"



void my_pthread_mutex_lock(pthread_mutex_t *mutex)
{
    int ret;
    
    //printf("con_mutex lock +++ 111\n");
    ret = pthread_mutex_lock(mutex);
    //printf("con_mutex lock +++ 222\n");

    if(EOWNERDEAD == ret)
    {
        printf("xxxxxxxxxxxxxxxx mutex lock EOWNERDEAD. xxxxxxxxxxxxxxxxxx\n");
    }
    else if(ret != 0)
    {
        printf("xxxxxxxxxxxxxxxx mutex lock unkown(%d) failed. xxxxxxxxxxxxxxxxxx\n", ret);
    }
    return;
}


void my_pthread_mutex_unlock(pthread_mutex_t *mutex)
{
    int ret;
    //printf("con_mutex unlock --- 111\n");
    ret = pthread_mutex_unlock(mutex);
    //printf("con_mutex unlock --- 222\n");

    if(EOWNERDEAD == ret)
    {
        printf("xxxxxxxxxxxxxxxx mutex lock EOWNERDEAD. xxxxxxxxxxxxxxxxxx\n");
    }
    else if(ret != 0)
    {
        printf("xxxxxxxxxxxxxxxx mutex lock unkown(%d) failed. xxxxxxxxxxxxxxxxxx\n", ret);
    }
    return;
}


static int create_coreobjects(P_ShmQue *p_shmQue)
{
    int ret;

    ret = sem_init(&p_shmQue->sem, 1, 0);
    if(ret != 0)
        return -1;

    pthread_mutexattr_t mutex_attr;
    pthread_mutexattr_init(&mutex_attr);
#ifdef PTHREAD_MUTEX_ROBUST
    pthread_mutexattr_setrobust(&mutex_attr, PTHREAD_MUTEX_ROBUST);
#endif
    pthread_mutexattr_setpshared(&mutex_attr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&p_shmQue->mutex, &mutex_attr);

    return 0;
}


static void destroy_coreobjects(P_ShmQue *p_shmQue)
{
    //pthread_cond_destroy(&p_shmQue->cond);
    sem_destroy(&p_shmQue->sem);
    pthread_mutex_destroy(&p_shmQue->mutex);
}


int shm_queue_init(char* shm_addr, off_t *offset, QueConf *queConf, int que_num, int sn, QcErr *err)
{
    
    P_ShmQue *p_shmQue = (P_ShmQue*)(shm_addr + *offset);
    *offset += sizeof(P_ShmQue);

    strncpy(p_shmQue->quename, queConf->qname, QUENAME_MAXLEN);
    int que_size = queConf->queuesize;
    int msg_size = queConf->msgsize;

    p_shmQue->que_sn = sn;
    p_shmQue->que_size = que_size;
    p_shmQue->msg_size = msg_size;

    create_coreobjects(p_shmQue);

    //static list of used
    if(0 != qc_staticlist_init(&p_shmQue->lst_used, que_size, shm_addr + *offset))
    {
        return -1;
    }
    qc_staticlist_clear(&p_shmQue->lst_used);
    *offset += queConf->queuesize * sizeof(_StaticCell);

    //static list of free
    if(0 != qc_staticlist_init(&p_shmQue->lst_free, que_size, shm_addr + *offset))
    {
        return -1;
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
    
    return 0;
}


P_ShmQue* shm_queue_find(char* shm_addr, const char *que_name, QcErr *err)
{
    off_t offset;
    P_ShmQue *p_shmQue;

    offset = sizeof(P_Shmem);
    p_shmQue = (P_ShmQue*)(shm_addr + offset);

    while(1)
    {
        if(strcmp(p_shmQue->quename, que_name) == 0)
        {
            return p_shmQue;
        }

        if(-1 == p_shmQue->next_offset)
        {
            qc_seterr(err, -1, "que_name (%s) not found in shm.", que_name);
            return NULL;
        }
        p_shmQue = (P_ShmQue*)((char*)shm_addr + p_shmQue->next_offset);
    }
}


int shm_queue_get(char* shm_addr, P_ShmQue *p_shmQue, int wait_msec, int *idx, char **pp_buff, int *p_bufflen, QcErr *err)
{
    int index;
    int ret, count;
    off_t buf_offset;
    struct timespec ts;

    count = 0;

    if(wait_msec < 0)
    {
        //printf("sem wait start.\n");
        ret = sem_wait(&p_shmQue->sem);
        //printf("sem wait end.\n");
    }
    else
    {
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += wait_msec/1000;
        ts.tv_nsec += (wait_msec%1000)*1000000;

        //printf("sem timed wait start.\n");
        ret = sem_timedwait(&p_shmQue->sem, &ts);
        //printf("sem timed wait end.\n");
        if(ETIMEDOUT == ret)
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

	my_pthread_mutex_lock(&p_shmQue->mutex);

	index = qc_staticlist_get_head(&p_shmQue->lst_used);
	if (index < 0)
	{
		printf("error happend when get from queue->qc_staticlist_used.\n");
		my_pthread_mutex_unlock(&p_shmQue->mutex);
		return -1;
	}

    my_pthread_mutex_unlock(&p_shmQue->mutex);

    *idx = index;
    buf_offset = (sizeof(off_t) + p_shmQue->msg_size) * index;
    *pp_buff = (char*)shm_addr + p_shmQue->data_offset + buf_offset;
    *p_bufflen = *(int*)(*pp_buff);
    *pp_buff += sizeof(off_t);

    return 0;
}


int shm_queue_free(char* shm_addr, P_ShmQue *p_shmQue, int idx, QcErr *err)
{
    int index;

    my_pthread_mutex_lock(&p_shmQue->mutex);
	index = qc_staticlist_add_tail(&p_shmQue->lst_free, idx);
	qc_assert(index>=0);
    my_pthread_mutex_unlock(&p_shmQue->mutex);

    return 0;
}


int shm_queue_alloc(char* shm_addr, P_ShmQue *p_shmQue, int *idx, char **pp_buff, int *p_bufflen, QcErr *err)
{
    int index;
    off_t buf_offset;

    my_pthread_mutex_lock(&p_shmQue->mutex);

	if (0 == qc_staticlist_count(&p_shmQue->lst_free))
	{
		qc_seterr(err, -1, "alloc failed, no free unit in queue(%s).", p_shmQue->quename);
        my_pthread_mutex_unlock(&p_shmQue->mutex);
        return -1;
	}

    index = qc_staticlist_get_head(&p_shmQue->lst_free);
	qc_assert(index>=0);

    my_pthread_mutex_unlock(&p_shmQue->mutex);

    *idx = index;
    buf_offset = (sizeof(off_t) + p_shmQue->msg_size) * index;
    *pp_buff = shm_addr + p_shmQue->data_offset + buf_offset;
    *p_bufflen = p_shmQue->msg_size;
    *((int*)*pp_buff) = p_shmQue->msg_size;
    *pp_buff += sizeof(off_t);

    return 0;
}


int shm_queue_put(char* shm_addr, P_ShmQue *p_shmQue, int idx, char *buff, int bufflen, QcErr *err)
{
    int index;
    off_t buf_offset;
    char *p_buff;
    int *buff_len;

    my_pthread_mutex_lock(&p_shmQue->mutex);
	index = qc_staticlist_add_tail(&p_shmQue->lst_used, idx);
	qc_assert(index>=0);

    buf_offset = (sizeof(off_t) + p_shmQue->msg_size) * idx;
    p_buff = shm_addr + p_shmQue->data_offset + buf_offset;
    buff_len = (int*)p_buff;
    *buff_len = bufflen;
    p_buff += sizeof(off_t);

    sem_post(&p_shmQue->sem);

    //mem copy
    if(p_buff != buff)
    {
        if(bufflen > p_shmQue->msg_size)
        {
            printf("bufflen(%d) > (%d), not correct.", bufflen, p_shmQue->msg_size);
            my_pthread_mutex_unlock(&p_shmQue->mutex);
            return -1;
        }
        memcpy(p_buff, buff, bufflen);
    }

    my_pthread_mutex_unlock(&p_shmQue->mutex);

    return 0;
}