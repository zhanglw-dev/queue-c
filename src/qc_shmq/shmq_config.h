#ifndef SHM_CONFIG_H
#define SHM_CONFIG_H

#include "qc_prelude.h"
#include "qc_error.h"
#include "qc_list.h"


typedef struct{
    char qname[32+1];
    int queuesize;
    off_t msgsize;
}QueConf;


typedef struct{
    char shmname[32+1];
    QcList *queConfList;
}ShmConf;


typedef struct {
    QcList *shmConfList;
}ShmQueConf;


ShmQueConf* read_config(const char* cfgfile, QcErr *err);


#endif  //SHM_CONFIG_H