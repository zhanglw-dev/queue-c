#include "qc_prelude.h"
#include "qc_shm.h"


#define QC_SHMNAME_MAXLEN 256

struct __QcShm__ {
	HANDLE mapHdl;
	LPVOID lpdata;
	size_t shmsize;
};


QcShm* qc_shm_create(const char *name, size_t shmsize, QcErr *err)
{
	HANDLE mapHdl;
	LPVOID lpdata;
	QcShm *shm;

	if (strlen(name) > QC_SHMNAME_MAXLEN)
	{
		qc_seterr(err, -1, "parameter name (%s) is too long.", name);
		return NULL;
	}

	mapHdl = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE | SEC_COMMIT, 0, shmsize, name);
	if (NULL == mapHdl)
	{
		qc_seterr(err, -1, "create filemapping (name : %s) failed.", name);
		return NULL;
	}

	lpdata = MapViewOfFile(mapHdl, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
	if (NULL == lpdata)
	{
		qc_seterr(err, -1, "map file (name : %s) failed.", name);
		CloseHandle(mapHdl);
		return NULL;
	}

	qc_malloc(shm, sizeof(QcShm));
	shm->mapHdl = mapHdl;
	shm->lpdata = lpdata;
	shm->shmsize = shmsize;

	return shm;
}


int qc_shm_destroy(QcShm *shm)
{
	UnmapViewOfFile(shm->lpdata);
	CloseHandle(shm->mapHdl);
	return 0;
}


QcShm* qc_shm_open(const char *name, QcErr *err)
{
	HANDLE mapHdl;
	LPVOID lpdata;
	QcShm *shm;

	if (strlen(name) > QC_SHMNAME_MAXLEN)
	{
		qc_seterr(err, -1, "parameter name (%s) is too long.", name);
		return NULL;
	}

	mapHdl = OpenFileMappingA(FILE_MAP_READ | FILE_MAP_WRITE, FALSE, name);
	if (NULL == mapHdl)
	{
		qc_seterr(err, -1, "open filemapping (name : %s) failed.", name);
		return NULL;
	}

	lpdata = MapViewOfFile(mapHdl, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
	if (NULL == lpdata)
	{
		qc_seterr(err, -1, "map file (name : %s) failed.", name);
		CloseHandle(mapHdl);
		return NULL;
	}

	MEMORY_BASIC_INFORMATION lBuffer;
	SIZE_T shmsize = VirtualQuery(lpdata, &lBuffer, sizeof(lBuffer));

	qc_malloc(shm, sizeof(QcShm));
	shm->mapHdl = mapHdl;
	shm->lpdata = lpdata;
	shm->shmsize = shmsize;

	return shm;
}


int qc_shm_close(QcShm *shm)
{
	UnmapViewOfFile(shm->lpdata);
	CloseHandle(shm->mapHdl);
	return 0;
}


size_t qc_shm_getsize(QcShm *shm)
{
	return shm->shmsize;
}


char* qc_shm_getaddr(QcShm *shm)
{
	return (char*)shm->lpdata;
}

