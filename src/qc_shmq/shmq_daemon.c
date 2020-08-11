#include "qc_prelude.h"

#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "shmq_config.h"
#include "shm_mem.h"
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

        if(shm_unlink(shmConf->shmname) == -1)
        {
            fprintf(stderr, "shm_unlink (%s) failed, errno=%d\n", shmConf->shmname, errno);
            exit(-1);
        }
    }

    exit(0);
}


int main()
{
    int ret;
    QcErr err;
    char binpath[128], filepath[256];
    printf("shmqueue start...\n");

    getcwd(binpath, sizeof(binpath)-1);
    sprintf(filepath, "%s/../etc/shmq.cfg", binpath);
    //printf("config file: %s\n", filepath);

    if(access(filepath, 0) != 0){
        printf("error: can not find config file:%s\n", filepath);
        exit(-1);
    }

    ShmQueConf *shmQueConf = read_config(filepath, &err);
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
    _shmemHdlList = shm_mem_createall(shmQueConf, &err);

    while(1)
    {
        sleep(1);
    }

    qc_list_destroy(shmQueConf->shmConfList);

    exit(0);
}
