/*
 * BSD 3-Clause License
 * 
 * Copyright (c) 2019, zhanglw (zhanglw366@163.com)
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

#include "qc_psist_file.h"
#include "qc_file.h"
#include "qc_prelude.h"

#define QC_PSIST_PATHLEN  512


#pragma pack(push)
#pragma pack(1)

typedef struct{
	int identity;
	int version;
	int msgbuff_size;
	int msgcount_limit;
}Qc_PsistFileInfo;


typedef struct {
	int flag;       //0: unused, 1: used
	int priority;
	int bufflen;
}Qc_MsgInfo;

#pragma pack(pop)



struct __QcPsistFile {
	char filename[QC_PSIST_PATHLEN + 1];
	Qc_PsistFileInfo *fileInfo;
	QcFile *file;
};


static void qc_psistfile_info_hton(Qc_PsistFileInfo *tblfileInfo)
{
	tblfileInfo->identity = htonl(tblfileInfo->identity);
	tblfileInfo->version = htonl(tblfileInfo->version);
	tblfileInfo->msgbuff_size = htonl(tblfileInfo->msgbuff_size);
	tblfileInfo->msgcount_limit = htonl(tblfileInfo->msgcount_limit);
}


static void qc_psistfile_info_ntoh(Qc_PsistFileInfo *tblfileInfo)
{
	tblfileInfo->identity = ntohl(tblfileInfo->identity);
	tblfileInfo->version = ntohl(tblfileInfo->version);
	tblfileInfo->msgbuff_size = ntohl(tblfileInfo->msgbuff_size);
	tblfileInfo->msgcount_limit = ntohl(tblfileInfo->msgcount_limit);
}


static void qc_msginfo_hton(Qc_MsgInfo *msgInfo)
{
	msgInfo->flag = htonl(msgInfo->flag);
	msgInfo->priority = htonl(msgInfo->priority);
	msgInfo->bufflen = htonl(msgInfo->bufflen);
}


static void qc_msginfo_ntoh(Qc_MsgInfo *msgInfo)
{
	msgInfo->flag = ntohl(msgInfo->flag);
	msgInfo->priority = ntohl(msgInfo->priority);
	msgInfo->bufflen = ntohl(msgInfo->bufflen);
}


static int qc_tblfileinfo_compare(Qc_PsistFileInfo *tblfileInfo1, Qc_PsistFileInfo *tblfileInfo2)
{
	if (tblfileInfo1->identity != tblfileInfo2->identity)
		return -1;
	if (tblfileInfo1->version != tblfileInfo2->version)
		return -1;
	if (tblfileInfo1->msgbuff_size != tblfileInfo2->msgbuff_size)
		return -1;
	if (tblfileInfo1->msgcount_limit != tblfileInfo2->msgcount_limit)
		return -1;

	return 0;
}


QcPsistFile* qc_psist_file_open(int msgbuff_size, int msgcount_limit, const char* table_filename, QcErr *err)
{
	qc_assert(msgbuff_size > 0 && msgcount_limit > 0 && table_filename);
	if (strlen(table_filename) > QC_PSIST_PATHLEN) {
		qc_seterr(err, QC_ERR_BADLENGTH, "table name is too long.");
		return NULL;
	}

	QcPsistFile *qtbl;
	qc_malloc(qtbl, sizeof(QcPsistFile));

	strcpy(qtbl->filename, table_filename);
	qtbl->file = NULL;
	qc_malloc(qtbl->fileInfo, sizeof(Qc_PsistFileInfo));

	qtbl->fileInfo->identity = 0;
	qtbl->fileInfo->version = 1;
	qtbl->fileInfo->msgbuff_size = msgbuff_size;
	qtbl->fileInfo->msgcount_limit = msgcount_limit;

	off_t filesize = sizeof(Qc_PsistFileInfo) + (sizeof(Qc_MsgInfo)+msgbuff_size)*msgcount_limit;

	if (0 != qc_file_exist(table_filename)) {
		//truncate file
		if (0 != qc_file_truncate(table_filename, filesize)) {
			qc_seterr(err, QC_ERR_IO, "truncate file(%s) failed.", table_filename);
			goto failed;
		}

		QcFile *file = qc_file_open(table_filename, O_CREAT|O_RDWR);
		if (!file) {
			qc_seterr(err, QC_ERR_IO, "open file(%s) failed.", table_filename);
			goto failed;
		}

		qtbl->file = file;

		//write head info
		qc_psistfile_info_hton(qtbl->fileInfo);
		size_t sz = qc_file_write(file, qtbl->fileInfo, sizeof(Qc_PsistFileInfo));
		if (sz <= 0){
			qc_seterr(err, QC_ERR_IO, "write file(%s) failed.", table_filename);
			goto failed;
		}
		qc_psistfile_info_hton(qtbl->fileInfo);   //used later
	}
	else {
		size_t fsz = qc_file_size(table_filename);
		if (fsz != filesize){
			qc_seterr(err, QC_ERR_IO, "get size of file(%s) failed.", table_filename);
			goto failed;
		}

		QcFile *file = qc_file_open(table_filename, O_CREAT|O_RDWR);
		if (!file) {
			qc_seterr(err, QC_ERR_IO, "open file(%s) failed.", table_filename);
			goto failed;
		}

		qtbl->file = file;

		Qc_PsistFileInfo tblFileInfo;
		size_t sz = qc_file_read(file, &tblFileInfo, sizeof(Qc_PsistFileInfo));
		if (sz <= 0){
			qc_seterr(err, QC_ERR_IO, "read file(%s) failed.", table_filename);
			goto failed;
		}
		qc_psistfile_info_ntoh(&tblFileInfo);

		if (0 != qc_tblfileinfo_compare(&tblFileInfo, qtbl->fileInfo)){
			qc_seterr(err, QC_ERR_VERIFY, "verify table file(%s) failed.", table_filename);
			goto failed;
		}
	}

	return qtbl;

failed:
	if (qtbl->file)
		qc_file_close(qtbl->file);
	qc_free(qtbl->fileInfo);
	qc_free(qtbl);

	return NULL;
}


void qc_psist_file_close(QcPsistFile *qtbl)
{
	if (qtbl->file) qc_file_close(qtbl->file);
	qc_free(qtbl->fileInfo);
	qc_free(qtbl);

	return;
}


int qc_psist_file_append(QcPsistFile *qtbl, Qc_MsgRecord *msgRecord, QcErr *err)
{
	size_t sz;
	int persist_id = msgRecord->persist_id;

	off_t offset_msghead = sizeof(Qc_PsistFileInfo) + persist_id*(sizeof(Qc_MsgInfo) + qtbl->fileInfo->msgbuff_size);
	off_t offset_msgbody = offset_msghead + sizeof(Qc_MsgInfo);

	QcFile *file = qtbl->file;
	qc_file_seek(file, offset_msgbody, 0);
	sz = qc_file_write(file, msgRecord->buff, msgRecord->bufflen);
	if (sz <= 0){
		qc_seterr(err, QC_ERR_IO,"write file[%s] failed.", qtbl->filename);
		return -1;
	}
	
	Qc_MsgInfo msgInfo;
	msgInfo.flag = 1;
	msgInfo.priority = msgRecord->priority;
	msgInfo.bufflen = msgRecord->bufflen;

	qc_file_seek(file, offset_msghead, 0);
	qc_msginfo_hton(&msgInfo);
	sz = qc_file_write(file, &msgInfo, sizeof(Qc_MsgInfo));
	if (sz <= 0){
		qc_seterr(err, QC_ERR_IO,"write file[%s] failed.", qtbl->filename);
		return -1;
	}

	//qc_file_sync(file);

	return 0;
}


int qc_psist_file_remove(QcPsistFile *qtbl, Qc_MsgRecord *msgRecord, QcErr *err)
{
	size_t sz;
	int persist_id = msgRecord->persist_id;
	off_t offset_msghead = sizeof(Qc_PsistFileInfo) + persist_id * (sizeof(Qc_MsgInfo) + qtbl->fileInfo->msgbuff_size);

	Qc_MsgInfo msgInfo;
	msgInfo.flag = 0;
	msgInfo.priority = 0;
	msgInfo.bufflen = 0;

	QcFile *file = qtbl->file;
	sz = qc_file_seek(file, offset_msghead, 0);
	if(sz < 0){
		qc_seterr(err, QC_ERR_IO,"seek file[%s] failed.", qtbl->filename);
		return -1;
	}
	qc_msginfo_hton(&msgInfo);
	sz = qc_file_write(file, &msgInfo, sizeof(Qc_MsgInfo));
	if(sz < 0){
		qc_seterr(err, QC_ERR_IO,"write file[%s] failed.", qtbl->filename);
		return -1;
	}

	//qc_file_sync(file);  //too slow
	return 0;
}


int qc_psist_file_fetch_ready(QcPsistFile *qtbl, QcErr *err)
{
	int sz;
	QcFile *file = qtbl->file;
	sz = qc_file_seek(file, sizeof(Qc_PsistFileInfo), 0);
	if(sz < 0){
		qc_seterr(err, QC_ERR_IO,"seek file[%s] failed.", qtbl->filename);
		return -1;
	}
	return 0;
}


int qc_psist_file_do_fetch(QcPsistFile *qtbl, Qc_MsgRecord *msgRecord, QcErr *err)
{
	QcFile *file = qtbl->file;
	off_t offset = qc_file_tell(file);

	//size_t filesize = qtbl->body_offset + (qtbl->fileInfo->msgbuff_size) * (qtbl->fileInfo->msgcount_limit);
	Qc_MsgInfo msgInfo;

	while (1) {
		if (offset == sizeof(Qc_PsistFileInfo) + (qtbl->fileInfo->msgcount_limit-1)*(sizeof(Qc_MsgInfo) + qtbl->fileInfo->msgbuff_size))
			return 1;   //the end

		int persist_id = (offset - sizeof(Qc_PsistFileInfo)) / (sizeof(Qc_MsgInfo) + qtbl->fileInfo->msgbuff_size);

		size_t hsz = qc_file_read(file, &msgInfo, sizeof(Qc_MsgInfo));
		qc_msginfo_ntoh(&msgInfo);
		if (hsz != sizeof(Qc_MsgInfo)){
			qc_seterr(err, QC_ERR_IO,"read file[%s] failed.", qtbl->filename);
			return -1;
		}

		offset += sizeof(Qc_MsgInfo) + qtbl->fileInfo->msgbuff_size;

		if (0 == msgInfo.flag){
			int bsz = qc_file_seek(file, qtbl->fileInfo->msgbuff_size, 1);
			if(bsz < 0){
				qc_seterr(err, QC_ERR_IO,"seek file[%s] failed.", qtbl->filename);
				return -1;
			}
			continue;  //not in use, next..
		}
		else{
			char *buff;
			qc_malloc(buff, msgInfo.bufflen);
			size_t bsz = qc_file_read(file, buff, msgInfo.bufflen);
			if (bsz != msgInfo.bufflen){
				qc_seterr(err, QC_ERR_IO,"read file[%s] failed.", qtbl->filename);
				return -1;				
			}

			msgRecord->persist_id = persist_id;
			msgRecord->priority = msgInfo.priority;
			msgRecord->bufflen = msgInfo.bufflen;
			msgRecord->buff = buff;
			break;
		}

	}

	qc_file_seek(file, offset, 0);
	return 0;
}
