#ifndef QC_SHM_MEM_H
#define QC_SHM_MEM_H


#include "qc_list.h"
#include "qc_error.h"
#include "shmq_config.h"

#define SHM_NAME_MAXLEN 128


typedef struct __ShmemHdl__{
	char *shm_name[SHM_NAME_MAXLEN];
	char *shm_addr;

	int que_num;
	size_t shm_size;

	off_t tmp_offset;
}ShmemHdl;


ShmemHdl* shm_mem_create(const char* shm_name, ShmConf *shmConf, QcErr *err);

int shm_mem_destroy(ShmemHdl *shmHdl);

QcList* shm_mem_createall(ShmQueConf *shmQueConf, QcErr *err);

int shm_mem_destroyall(QcList *shmemHdlList);


#endif //QC_SHM_MEM_H