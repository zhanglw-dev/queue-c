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
	HANDLE mapHdl;
	LPVOID lpdata;
	size_t shmsize;
};



QcShm* qc_shm_create(const char *name, size_t shmsize, QcErr *err)
{
	HANDLE mapHdl;
	LPVOID lpdata;
	QcShm *qcShm;

	if (strlen(name) > QC_SHMNAME_MAXLEN)
	{
		qc_seterr(err, -1, "parameter name (%s) is too long.", name);
		return NULL;
	}

	mapHdl = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE | SEC_COMMIT, 0, (DWORD)shmsize, name);
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

	qc_malloc(qcShm, sizeof(QcShm));
	qcShm->mapHdl = mapHdl;
	qcShm->lpdata = lpdata;
	qcShm->shmsize = shmsize;

	return qcShm;
}


int qc_shm_destroy(QcShm *qcShm)
{
	UnmapViewOfFile(qcShm->lpdata);
	CloseHandle(qcShm->mapHdl);
	return 0;
}


QcShm* qc_shm_open(const char *name, QcErr *err)
{
	HANDLE mapHdl;
	LPVOID lpdata;
	QcShm *qcShm;

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

	qc_malloc(qcShm, sizeof(QcShm));
	qcShm->mapHdl = mapHdl;
	qcShm->lpdata = lpdata;
	qcShm->shmsize = shmsize;

	return qcShm;
}


int qc_shm_close(QcShm *qcShm)
{
	UnmapViewOfFile(qcShm->lpdata);
	CloseHandle(qcShm->mapHdl);
	return 0;
}


size_t qc_shm_getsize(QcShm *qcShm)
{
	return qcShm->shmsize;
}


char* qc_shm_getaddr(QcShm *qcShm)
{
	return (char*)qcShm->lpdata;
}

