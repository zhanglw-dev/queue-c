#include "qc_qtbl_file.h"
#include "qc_file.h"

#define QC_QDB_PATHLEN  512


#pragma pack(push)
#pragma pack(1)

typedef struct{
	int ident;
	int version;
	int msgbuff_size;
	int msgcount_limit;
}QcQTblFileInfo;


typedef struct {
	int flag;       //0: no used, 1: used
	int priority;
	int bufflen;
}Qc_MsgInfo;

#pragma pack(pop)



struct __QcQTblFile {
	char filename[QC_QDB_PATHLEN + 1];
	QcQTblFileInfo *fileInfo;

	off_t head_offset;
	off_t body_offset;

	QcFile *file;
};



QcQTblFile* qc_qtbl_file_open(int msgbuff_size, int msgcount_limit, const char* persist_filename, QcErr *err)
{
	qc_assert(msgbuff_size > 0 && msgcount_limit > 0 && persist_filename);
	qc_assert(strlen(persist_filename) <= QC_QDB_PATHLEN);

	QcQTblFile *qtbl = (QcQTblFile*)malloc(sizeof(QcQTblFile));
	qc_assert(qtbl);
	memset(qtbl, 0, sizeof(QcQTblFile));

	strcpy(qtbl->filename, persist_filename);
	qtbl->file = NULL;
	qtbl->fileInfo = (QcQTblFileInfo*)malloc(sizeof(QcQTblFileInfo));
	qc_assert(qtbl->fileInfo);

	qtbl->head_offset = sizeof(QcQTblFileInfo);
	qtbl->body_offset = qtbl->head_offset + sizeof(Qc_MsgInfo)*msgcount_limit;

	off_t filesize = qtbl->body_offset + msgbuff_size*msgcount_limit;

	if (0 != qc_file_exist(persist_filename)) {
		//truncate file
		if (0 != qc_file_truncate(persist_filename, filesize)) {
			goto failed;
		}

		QcFile *file = qc_file_open(persist_filename, 0);
		if (!file) {
			goto failed;
		}

		qtbl->file = file;

		//write head info
		size_t sz = qc_file_write(file, qtbl->fileInfo, sizeof(QcQTblFileInfo));  //Todo: ×Ö½ÚÐò
		if (sz <= 0)
			goto failed;

		return qtbl;
	}
	else {
		size_t fsz = qc_file_size(persist_filename);
		if (fsz != filesize)
			goto failed;

		QcFile *file = qc_file_open(persist_filename, 0);
		if (!file) {
			goto failed;
		}

		qtbl->file = file;

		size_t sz = qc_file_read(file, qtbl->fileInfo, sizeof(sizeof(QcQTblFileInfo)));  //Todo: ×Ö½ÚÐò
		if (sz <= 0)
			goto failed;
	}

failed:
	if (qtbl->file)
		qc_file_close(qtbl->file);
	qc_free(qtbl->fileInfo);
	qc_free(qtbl);

	return NULL;
}


void qc_qtbl_file_close(QcQTblFile *qtbl)
{
	if (qtbl->file) qc_file_close(qtbl->file);
	if(qtbl->fileInfo) qc_free(qtbl->fileInfo);
	qc_free(qtbl);

	return;
}


int qc_qtbl_file_append(QcQTblFile *qtbl, Qc_MsgEqual *msgEqual, QcErr *err)
{
	int persist_id = msgEqual->persist_id;
	off_t offset_msghead = qtbl->head_offset + persist_id * sizeof(Qc_MsgInfo);
	off_t offset_msgbody = qtbl->body_offset + persist_id * (qtbl->fileInfo->msgbuff_size);

	QcFile *file = qtbl->file;
	qc_file_seek(file, offset_msgbody, 0);
	qc_file_write(file, msgEqual->buff, msgEqual->bufflen);
	
	Qc_MsgInfo msgInfo;
	msgInfo.flag = 1;
	msgInfo.priority = msgEqual->priority;
	msgInfo.bufflen = msgEqual->bufflen;

	qc_file_seek(file, offset_msghead, 0);
	qc_file_write(file, &msgInfo, sizeof(Qc_MsgInfo));

	qc_file_sync(file);

	return 0;
}


int qc_qtbl_file_remove(QcQTblFile *qtbl, Qc_MsgEqual *msgEqual, QcErr *err)
{
	int persist_id = msgEqual->persist_id;
	off_t offset_msghead = qtbl->head_offset + persist_id * sizeof(Qc_MsgInfo);

	Qc_MsgInfo msgInfo;
	msgInfo.flag = 0;
	msgInfo.priority = 0;
	msgInfo.bufflen = 0;

	QcFile *file = qtbl->file;
	qc_file_seek(file, offset_msghead, 0);
	qc_file_write(file, &msgInfo, sizeof(Qc_MsgInfo));

	qc_file_sync(file);

	return 0;
}


int qc_qtbl_file_fetch_ready(QcQTblFile *qtbl, QcErr *err)
{
	QcFile *file = qtbl->file;
	qc_file_seek(file, qtbl->head_offset, 0);
	return 0;
}


int qc_qtbl_file_do_fetch(QcQTblFile *qtbl, Qc_MsgEqual *msgEqual, QcErr *err)
{
	QcFile *file = qtbl->file;
	off_t offset = qc_file_tell(file);

	//size_t filesize = qtbl->body_offset + (qtbl->fileInfo->msgbuff_size) * (qtbl->fileInfo->msgcount_limit);
	Qc_MsgInfo msgInfo;

	while (1) {
		if (offset == qtbl->head_offset)
			return 1;   //the end

		int persist_id = (offset - sizeof(QcQTblFileInfo))/ sizeof(Qc_MsgInfo);

		size_t hsz = qc_file_read(file, &msgInfo, sizeof(Qc_MsgInfo));
		if (hsz != sizeof(Qc_MsgInfo))
			return -1;

		offset += (off_t)hsz;

		if (0 == msgInfo.flag){
			continue;  //not in use, next..
		}
		else{
			int offset_buff = qtbl->body_offset + persist_id * (qtbl->fileInfo->msgbuff_size);
			qc_file_seek(file, offset_buff, 0);

			char *buff = (char*)malloc(qtbl->fileInfo->msgbuff_size);
			size_t bsz = qc_file_read(file, buff, qtbl->fileInfo->msgbuff_size);
			if (bsz != qtbl->fileInfo->msgbuff_size)
				return -1;

			msgEqual->persist_id = persist_id;
			msgEqual->priority = msgInfo.priority;
			msgEqual->bufflen = msgInfo.bufflen;
			msgEqual->buff = buff;
			break;
		}
	}

	qc_file_seek(file, offset, 0);
	return 0;
}
