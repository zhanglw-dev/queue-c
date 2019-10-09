#include "qc_qdb_file.h"
#include "qc_file.h"

#define QC_QDB_PATHLEN  512


#pragma pack(push)
#pragma pack(1)

typedef struct{
	int ident;
	int version;
	int msgbuff_size;
	int msgcount_limit;
}QcQdbFileInfo;


typedef struct {
	int flag;       //0: no used, 1: used
	int priority;
	int bufflen;
}Qc_MsgInfo;

#pragma pack(pop)



struct __QcQdbFile {
	char filename[QC_QDB_PATHLEN + 1];
	QcQdbFileInfo *fileInfo;

	off_t head_offset;
	off_t body_offset;

	QcFile *file;
};



QcQdbFile* qc_qdb_file_open(int msgbuff_size, int msgcount_limit, const char* persist_filename, QcErr *err)
{
	qc_assert(msgbuff_size > 0 && msgcount_limit > 0 && persist_filename);
	qc_assert(strlen(persist_filename) <= QC_QDB_PATHLEN);

	QcQdbFile *qdb = (QcQdbFile*)malloc(sizeof(QcQdbFile));
	qc_assert(qdb);
	memset(qdb, 0, sizeof(QcQdbFile));

	strcpy(qdb->filename, persist_filename);
	qdb->file = NULL;
	qdb->fileInfo = (QcQdbFileInfo*)malloc(sizeof(QcQdbFileInfo));
	qc_assert(qdb->fileInfo);

	qdb->head_offset = sizeof(QcQdbFileInfo);
	qdb->body_offset = qdb->head_offset + sizeof(Qc_MsgInfo)*msgcount_limit;

	off_t filesize = qdb->body_offset + msgbuff_size*msgcount_limit;

	if (0 != qc_file_exist(persist_filename)) {
		//truncate file
		if (0 != qc_file_truncate(persist_filename, filesize)) {
			goto failed;
		}

		QcFile *file = qc_file_open(persist_filename, 0);
		if (!file) {
			goto failed;
		}

		qdb->file = file;

		//write head info
		size_t sz = qc_file_write(file, qdb->fileInfo, sizeof(QcQdbFileInfo));  //Todo: ×Ö½ÚÐò
		if (sz <= 0)
			goto failed;

		return qdb;
	}
	else {
		size_t fsz = qc_file_size(persist_filename);
		if (fsz != filesize)
			goto failed;

		QcFile *file = qc_file_open(persist_filename, 0);
		if (!file) {
			goto failed;
		}

		qdb->file = file;

		size_t sz = qc_file_read(file, qdb->fileInfo, sizeof(sizeof(QcQdbFileInfo)));  //Todo: ×Ö½ÚÐò
		if (sz <= 0)
			goto failed;
	}

failed:
	if (qdb->file)
		qc_file_close(qdb->file);
	qc_free(qdb->fileInfo);
	qc_free(qdb);

	return NULL;
}


void qc_qdb_file_close(QcQdbFile *qdb)
{
	if (qdb->file) qc_file_close(qdb->file);
	if(qdb->fileInfo) qc_free(qdb->fileInfo);
	qc_free(qdb);

	return;
}


int qc_qdb_file_append(QcQdbFile *qdb, Qc_MsgEqual *msgEqual, QcErr *err)
{
	int persist_id = msgEqual->persist_id;
	off_t offset_msghead = qdb->head_offset + persist_id * sizeof(Qc_MsgInfo);
	off_t offset_msgbody = qdb->body_offset + persist_id * (qdb->fileInfo->msgbuff_size);

	QcFile *file = qdb->file;
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


int qc_qdb_file_remove(QcQdbFile *qdb, Qc_MsgEqual *msgEqual, QcErr *err)
{
	int persist_id = msgEqual->persist_id;
	off_t offset_msghead = qdb->head_offset + persist_id * sizeof(Qc_MsgInfo);

	Qc_MsgInfo msgInfo;
	msgInfo.flag = 0;
	msgInfo.priority = 0;
	msgInfo.bufflen = 0;

	QcFile *file = qdb->file;
	qc_file_seek(file, offset_msghead, 0);
	qc_file_write(file, &msgInfo, sizeof(Qc_MsgInfo));

	qc_file_sync(file);

	return 0;
}


int qc_qdb_file_fetch_ready(QcQdbFile *qdb, QcErr *err)
{
	QcFile *file = qdb->file;
	qc_file_seek(file, qdb->head_offset, 0);
	return 0;
}


int qc_qdb_file_do_fetch(QcQdbFile *qdb, Qc_MsgEqual *msgEqual, QcErr *err)
{
	QcFile *file = qdb->file;
	off_t offset = qc_file_tell(file);

	//size_t filesize = qdb->body_offset + (qdb->fileInfo->msgbuff_size) * (qdb->fileInfo->msgcount_limit);
	Qc_MsgInfo msgInfo;

	while (1) {
		if (offset == qdb->head_offset)
			return 1;   //the end

		int persist_id = (offset - sizeof(QcQdbFileInfo))/ sizeof(Qc_MsgInfo);

		size_t hsz = qc_file_read(file, &msgInfo, sizeof(Qc_MsgInfo));
		if (hsz != sizeof(Qc_MsgInfo))
			return -1;

		offset += (off_t)hsz;

		if (0 == msgInfo.flag){
			continue;  //not in use, next..
		}
		else{
			int offset_buff = qdb->body_offset + persist_id * (qdb->fileInfo->msgbuff_size);
			qc_file_seek(file, offset_buff, 0);

			char *buff = (char*)malloc(qdb->fileInfo->msgbuff_size);
			size_t bsz = qc_file_read(file, buff, qdb->fileInfo->msgbuff_size);
			if (bsz != qdb->fileInfo->msgbuff_size)
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
