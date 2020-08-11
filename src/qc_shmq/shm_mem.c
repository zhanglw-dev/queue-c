#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "qc_list.h"

#include "shm_mem.h"
#include "shm_queue.h"
#include "shmq_config.h"


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


char* create_shm(const char* shm_name, size_t shm_size, QcErr *err)
{
    int flags, opt, fd;
    mode_t perms;
    size_t size;
    char *addr;

    flags = O_CREAT|O_EXCL|O_RDWR;
    perms = S_IRUSR|S_IWUSR;
    size  = shm_size;

    //unlink shm first?
    shm_unlink(shm_name);

    fd = shm_open(shm_name, flags, perms);
    if(fd == -1)
    {
        qc_seterr(err, -1, "shm_open failed, errno=%d\n", errno);
        return NULL;
    }

    if(ftruncate(fd, size) == -1)
    {
        qc_seterr(err, -1, "ftruncate failed. errno=%d\n", errno);
        return NULL;
    }

    addr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(addr == MAP_FAILED)
    {
        qc_seterr(err, -1, "mmap faied, errno=%d\n", errno);
        return NULL;
    }

    printf("create shm (%s) succeed!\n", shm_name);

    return addr;
}


int init_shmhead(ShmemHdl *shmemHdl, QcErr *err)
{
    P_Shmem *p_Shmem = (P_Shmem*)shmemHdl->shm_addr;
    p_Shmem->status = 0;
    p_Shmem->quenum = shmemHdl->que_num;

    shmemHdl->tmp_offset += sizeof(P_Shmem);

    return 0;
}


int init_all_shmque(ShmemHdl *shmemHdl, QcList *queConfList, QcErr *err)
{
    qc_list_enumbegin(queConfList);
    for(int i=0; i<qc_list_count(queConfList); i++)
    {
        QcListEntry *entry = qc_list_enumentry(queConfList);
        if(NULL == entry)
            break;

        QueConf *queConf = qc_list_data(entry);

        printf("  init shmque (%s) start...\n", queConf->qname);
        if(0 != shm_queue_init(shmemHdl->shm_addr, &shmemHdl->tmp_offset, queConf, shmemHdl->que_num, i, err))
        {
            printf("  init shmque (%s) failed, err=%s\n", queConf->qname, err->desc);
            return -1;
        }
        
        printf("  init shmque (%s) succeed!\n", queConf->qname);
    }

    return 0;
}


ShmemHdl* shm_mem_create(const char* shm_name, ShmConf *shmConf, QcErr *err)
{
    ShmemHdl *shmemHdl;
    qc_malloc(shmemHdl, sizeof(ShmemHdl));
    
    memset(shmemHdl->shm_name, 0, SHM_NAME_MAXLEN);
    shmemHdl->tmp_offset = 0;
    shmemHdl->que_num = qc_list_count(shmConf->queConfList);

    strncpy((char*)shmemHdl->shm_name, shm_name, SHM_NAME_MAXLEN);

    size_t shm_size = calc_shmsize(shmConf->queConfList, err);
    shmemHdl->shm_size = shm_size;

    char* shm_addr = create_shm(shm_name, shm_size, err);
    if(NULL == shm_addr)
    {
        return NULL;
    }

    shmemHdl->shm_addr = shm_addr;

    if(0 != init_shmhead(shmemHdl, err))
    {
        return NULL;
    }

    if(0 != init_all_shmque(shmemHdl, shmConf->queConfList, err))
    {
        return NULL;
    }

    //be sure final offset is right
    if(shmemHdl->shm_size != shmemHdl->tmp_offset)
    {
        printf("offset (%d != %d) error, shm failed.\n", (int)shmemHdl->shm_size, (int)shmemHdl->tmp_offset);
        return NULL;
    }

    return shmemHdl;
}


int shm_mem_destroy(ShmemHdl *shmHdl)
{
    if(shm_unlink((char*)shmHdl->shm_name) == -1)
    {
        printf("shm_unlink (%s) failed, errno=%d\n", (char*)shmHdl->shm_name, errno);
        return -1;
    }    
    return 0;
}


QcList* shm_mem_createall(ShmQueConf *shmQueConf, QcErr *err)
{
    QcList *shmemHdlList = qc_list_create(0);
    qc_list_enumbegin(shmQueConf->shmConfList);

    while(1)
    {
        QcListEntry* entry = qc_list_enumentry(shmQueConf->shmConfList);
        if(NULL == entry)
        {
            break;
        }
        ShmConf *shmConf = (ShmConf*)qc_list_data(entry);

        ShmemHdl *shmemHdl = shm_mem_create(shmConf->shmname, shmConf, err);
        if(NULL == shmemHdl)
        {
            printf("create shmem (%s) failed. err=%s\n", (char*)shmConf->shmname, err->desc);
            return NULL;
        }
        qc_list_inserttail(shmemHdlList, shmemHdl);
    }

    printf("all shmem created!\n");
    return shmemHdlList;
}


int shm_mem_destroyall(QcList *shmemHdlList)
{
    int ret;
    qc_list_enumbegin(shmemHdlList);

    while(1)
    {
        QcListEntry* entry = qc_list_enumentry(shmemHdlList);
        if(NULL == entry)
        {
            break;
        }

        ShmemHdl *shmemHdl = (ShmemHdl*)qc_list_data(entry);
        ret = shm_mem_destroy(shmemHdl);
        if(ret < 0)
        {
            printf("destroy shmem (%s) failed.\n", (char*)shmemHdl->shm_name);
        }
    }

    return 0;
}