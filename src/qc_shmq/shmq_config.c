#include "shmq_config.h"
#include "cJSON.h"



off_t calc_size_byte(const char *szstr)
{
    off_t val;
    char dig[20];

    memset(dig, 0, sizeof(dig));
    qc_assert(strlen(szstr) < sizeof(dig));
    
    strcpy(dig, szstr);

    int l = strlen(dig);
    char c = dig[l-1];
    dig[l-1] = '\0';

    if('k' == c || 'K' == c)
    {
        val = atoi(dig) * 1024;
    }
    else if('m' == c || 'M' == c)
    {
        val = atoi(dig) * 1024 * 1024;
    }

    return val;
}


int add_que(QcList *queConfList, cJSON *arr, QcErr *err)
{
    int ret;
    cJSON *item;
    QueConf *queConf;

    queConf = (QueConf*)malloc(sizeof(QueConf));
    memset(queConf, 0, sizeof(QueConf));

    if(item = cJSON_GetObjectItem(arr,"qname"))
    {
        //printf("    %s\n", item->valuestring);
        strncpy(queConf->qname, item->valuestring, sizeof(queConf->qname));
    }
    else
    {
        qc_seterr(err, -1, "no tag qname found.");
        return -1;
    }

    if(item = cJSON_GetObjectItem(arr,"queuesize"))
    {
        //printf("    %d\n", item->valueint);
        queConf->queuesize = item->valueint;
    }
    else
    {
        qc_seterr(err, -1, "no tag queuesize found.");
        return -1;
    }

    if(item = cJSON_GetObjectItem(arr,"msgsize"))
    {
        //printf("    %s\n", item->valuestring);
        queConf->msgsize = calc_size_byte(item->valuestring);
    }
    else
    {
        qc_seterr(err, -1, "no tag msgsize found.");
        return -1;
    }

    printf("  qname=%s, queuesize=%d, msgsize=%d\n", queConf->qname, queConf->queuesize, (int)queConf->msgsize);
    qc_list_inserttail(queConfList, queConf);

    return 0;
}


int add_shm(QcList *shmConfList, cJSON *arr, QcErr *err)
{
    int ret;
    cJSON *item;
    ShmConf *shmConf;

    shmConf = (ShmConf*)malloc(sizeof(ShmConf));
    memset(shmConf, 0, sizeof(ShmConf));
    shmConf->queConfList = qc_list_create(0);

    if(item = cJSON_GetObjectItem(arr,"shmname"))
    {
        //printf("%s\n", item->valuestring);
        strncpy(shmConf->shmname, item->valuestring, sizeof(shmConf->shmname));
    }
    else
    {
        qc_seterr(err, -1, "no tag shmname found.");
        return -1;
    }
    
    cJSON *item_queues = cJSON_GetObjectItem(arr, "queues");
    if(!item_queues)
    {
        qc_seterr(err, -1, "parse cfg failed, tag queues not found.");
        return -1;
    }

    int size = cJSON_GetArraySize(item_queues);
    if(size<=0)
    {
        qc_seterr(err, -1, "parse cfg failed, no queue item found in shm section.");
        return -1;
    }

    cJSON *arr_que = item_queues->child;
    cJSON* item_chile = NULL;

    for(int i = 0;i < size;++i)
    {
        ret = add_que(shmConf->queConfList, arr_que, err);
        if(ret < 0)
            return -1;

        arr_que = arr_que->next;
    }

    printf("shmname=%s\n", shmConf->shmname);
    qc_list_inserttail(shmConfList, shmConf);

    return 0;
}


ShmQueConf* read_config(const char* cfgfile, QcErr *err)
{
    int ret;
    char jsonbuff[4096];

    memset(jsonbuff, 0, sizeof(jsonbuff));
    FILE *fh = fopen(cfgfile, "r");
    if(NULL == fh)
    {
        qc_seterr(err, -1, "open config file(%s) failed.", cfgfile);
        return NULL;
    }

    fread(jsonbuff, sizeof(jsonbuff), 1, (FILE*)fh);
    //printf("%s\n", buff);
    fclose(fh);

    cJSON *root = cJSON_Parse(jsonbuff);
    if(!root)
    {
        qc_seterr(err, -1, "parse json failed.");
        return NULL;
    }

    //printf("%s\n", cJSON_Print(root));
    cJSON *item_shmems = cJSON_GetObjectItem(root, "shmems");
    if(!item_shmems)
    {
        qc_seterr(err, -1, "parse cfg failed, tag shemes not found.");
        return NULL;
    }

    //printf("%s\n", cJSON_Print(item));
    int size = cJSON_GetArraySize(item_shmems);
    if(size<=0)
    {
        qc_seterr(err, -1, "parse cfg failed, no shm item found.");
        return NULL;
    }

    ShmQueConf *shmQueConf = (ShmQueConf*)malloc(sizeof(ShmQueConf));
    memset(shmQueConf, 0, sizeof(ShmQueConf));
    shmQueConf->shmConfList = qc_list_create(0);

    cJSON *arr_shm = item_shmems->child;
    cJSON* item_chile = NULL;

    for(int i = 0;i < size;++i)
    {
        ret = add_shm(shmQueConf->shmConfList, arr_shm, err);
        if(ret < 0)
            return NULL;

        arr_shm = arr_shm->next;
    }

    cJSON_Delete(root);

    return shmQueConf;
}
