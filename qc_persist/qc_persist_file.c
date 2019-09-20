#include "qc_persist_file.h"
#include "qc_file.h"

#define QC_PERSIST_PATHLEN  512


#pragma pack(push)
#pragma pack(1)

typedef struct{
	int ident;
	int version;
	int msgbuff_size;
	int msgcount_limit;
}QcPersistFileInfo;


typedef struct {
	int flag;       //0: no used, 1: used
	int properties;
	int bufflen;
}Qc_MsgInfo;

#pragma pack(pop)



struct __QcPersistFile {
	char filename[QC_PERSIST_PATHLEN + 1];
	QcPersistFileInfo *fileInfo;

	off_t head_offset;
	off_t body_offset;

	QcFile *file;
};



QcPersistFile* qc_persist_file_open(int msgbuff_size, int msgcount_limit, const char* persist_filename, QcErr *err)
{
	qc_assert(msgbuff_size > 0 && msgcount_limit > 0 && persist_filename);
	qc_assert(strlen(persist_filename) <= QC_PERSIST_PATHLEN);

	QcPersistFile *persistFile = (QcPersistFile*)malloc(sizeof(QcPersistFile));
	qc_assert(persistFile);
	memset(persistFile, 0, sizeof(QcPersistFile));

	strcpy(persistFile->filename, persist_filename);
	persistFile->file = NULL;
	persistFile->fileInfo = (QcPersistFileInfo*)malloc(sizeof(QcPersistFileInfo));
	qc_assert(persistFile->fileInfo);

	persistFile->head_offset = sizeof(QcPersistFileInfo);
	persistFile->body_offset = persistFile->head_offset + sizeof(Qc_MsgInfo)*msgcount_limit;

	off_t filesize = persistFile->body_offset + msgbuff_size*msgcount_limit;

	if (0 != qc_file_exist(persist_filename)) {
		//truncate file
		if (0 != qc_file_truncate(persist_filename, filesize)) {
			goto failed;
		}

		QcFile *file = qc_file_open(persist_filename, 0);
		if (!file) {
			goto failed;
		}

		persistFile->file = file;

		//write head info
		size_t sz = qc_file_write(file, persistFile->fileInfo, sizeof(QcPersistFileInfo));  //Todo: ×Ö½ÚÐò
		if (sz <= 0)
			goto failed;

		return persistFile;
	}
	else {
		size_t fsz = qc_file_size(persist_filename);
		if (fsz != filesize)
			goto failed;

		QcFile *file = qc_file_open(persist_filename, 0);
		if (!file) {
			goto failed;
		}

		persistFile->file = file;

		size_t sz = qc_file_read(file, persistFile->fileInfo, sizeof(sizeof(QcPersistFileInfo)));  //Todo: ×Ö½ÚÐò
		if (sz <= 0)
			goto failed;
	}

failed:
	if (persistFile->file)
		qc_file_close(persistFile->file);
	qc_free(persistFile->fileInfo);
	qc_free(persistFile);

	return NULL;
}


void qc_persist_file_close(QcPersistFile *persistFile)
{
	if (persistFile->file) qc_file_close(persistFile->file);
	if(persistFile->fileInfo) qc_free(persistFile->fileInfo);
	qc_free(persistFile);

	return;
}


int qc_persist_file_append(QcPersistFile *persistFile, Qc_MsgEqual *msgEqual, QcErr *err)
{
	int persist_id = msgEqual->persist_id;
	off_t offset_msghead = persistFile->head_offset + persist_id * sizeof(Qc_MsgInfo);
	off_t offset_msgbody = persistFile->body_offset + persist_id * (persistFile->fileInfo->msgbuff_size);

	QcFile *file = persistFile->file;
	qc_file_seek(file, offset_msgbody, 0);
	qc_file_write(file, msgEqual->buff, msgEqual->bufflen);
	
	Qc_MsgInfo msgInfo;
	msgInfo.flag = 1;
	msgInfo.properties = msgEqual->priority;
	msgInfo.bufflen = msgEqual->bufflen;

	qc_file_seek(file, offset_msghead, 0);
	qc_file_write(file, &msgInfo, sizeof(Qc_MsgInfo));

	qc_file_sync(file);

	return 0;
}


int qc_persist_file_remove(QcPersistFile *persistFile, Qc_MsgEqual *msgEqual, QcErr *err)
{
	int persist_id = msgEqual->persist_id;
	off_t offset_msghead = persistFile->head_offset + persist_id * sizeof(Qc_MsgInfo);

	Qc_MsgInfo msgInfo;
	msgInfo.flag = 0;
	msgInfo.properties = 0;
	msgInfo.bufflen = 0;

	QcFile *file = persistFile->file;
	qc_file_seek(file, offset_msghead, 0);
	qc_file_write(file, &msgInfo, sizeof(Qc_MsgInfo));

	qc_file_sync(file);

	return 0;
}

