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
#include "qc_sem.h"


struct __QcSem__ {
	char name[QC_SEMNAME_MAXLEN + 1];
	HANDLE semHdl;
};


QcSem* qc_sem_create(const char* name, int initcount, QcErr* err)
{
	HANDLE semHdl;
	QcSem *sem;

	if (strlen(name) > QC_SEMNAME_MAXLEN)
	{
		qc_seterr(err, -1, "parameter name (%s) is too long.", name);
		return NULL;
	}

	semHdl = CreateSemaphore(NULL, initcount, 10000000, name);
	if (NULL == semHdl)
	{
		qc_seterr(err, -1, "create semaphore (name : %s) call failed.", name);
		return NULL;
	}

	if (ERROR_ALREADY_EXISTS == GetLastError())
	{
		qc_seterr(err, -1, "semaphore (name : %s) is already existed.", name);
		return NULL;
	}

	sem = malloc(sizeof(QcSem));
	strncpy((char*)sem->name, name, QC_SEMNAME_MAXLEN);
	sem->semHdl = semHdl;

	return sem;
}


int qc_sem_destroy(QcSem *sem)
{
	CloseHandle(sem->semHdl);
	qc_free(sem);
	return 0;
}


QcSem* qc_sem_open(const char *name, QcErr *err)
{
	HANDLE semHdl;
	QcSem *sem;

	if (strlen(name) > QC_SEMNAME_MAXLEN)
	{
		qc_seterr(err, -1, "parameter name (%s) is too long.", name);
		return NULL;
	}

	semHdl = OpenSemaphore(SEMAPHORE_ALL_ACCESS, TRUE, name);
	if (NULL == semHdl)
	{
		qc_seterr(err, -1, "open semaphore (name : %s) call failed.", name);
		return NULL;
	}

	qc_malloc(sem, sizeof(QcSem));
	strncpy((char*)sem->name, name, QC_SEMNAME_MAXLEN);
	sem->semHdl = semHdl;

	return sem;
}


int qc_sem_close(QcSem *sem)
{
	CloseHandle(sem->semHdl);
	qc_free(sem);
	return 0;
}


int qc_sem_wait(QcSem *sem, int wait_msec)
{
	DWORD dwResult;

	if (wait_msec < 0)
	{
		WaitForSingleObject(sem->semHdl, INFINITE);
	}
	else
	{
		dwResult = WaitForSingleObject(sem->semHdl, (DWORD)wait_msec);
		if (WAIT_TIMEOUT == dwResult)
		{
			return QC_ERR_TIMEOUT;
		}
	}

	return 0;
}


int qc_sem_post(QcSem *sem)
{
	BOOL ret;
	ret = ReleaseSemaphore(sem->semHdl, 1, NULL);
	if (FALSE == ret)
		return -1;
	return 0;
}

