#include "qc_qtbl_file.h"
#include "qc_file.h"
#include "qc_prelude.h"

#define QC_QTBL_PATHLEN  512


#pragma pack(push)
#pragma pack(1)

typedef struct{
	int ident;
	int version;
	int msgbuff_size;
	int msgcount_limit;
}Qc_QTblFileInfo;


typedef struct {
	int flag;       //0: unused, 1: used
	int priority;
	int bufflen;
}Qc_MsgInfo;

#pragma pack(pop)



struct __QcQTblFile {
	char filename[QC_QTBL_PATHLEN + 1];
	Qc_QTblFileInfo *fileInfo;

	off_t head_offset;
	off_t body_offset;

	QcFile *file;
};



void qc_tblfile_info_hton(Qc_QTblFileInfo *tblfileInfo)
{
	tblfileInfo->ident = htonl(tblfileInfo->ident);
	tblfileInfo->version = htonl(tblfileInfo->version);
	tblfileInfo->msgbuff_size = htonl(tblfileInfo->msgbuff_size);
	tblfileInfo->msgcount_limit = htonl(tblfileInfo->msgcount_limit);
}


void qc_tblfile_info_ntoh(Qc_QTblFileInfo *tblfileInfo)
{
	tblfileInfo->ident = ntohl(tblfileInfo->ident);
	tblfileInfo->version = ntohl(tblfileInfo->version);
	tblfileInfo->msgbuff_size = ntohl(tblfileInfo->msgbuff_size);
	tblfileInfo->msgcount_limit = ntohl(tblfileInfo->msgcount_limit);
}


void qc_msginfo_hton(Qc_MsgInfo *msgInfo)
{
	msgInfo->flag = htonl(msgInfo->flag);
	msgInfo->priority = htonl(msgInfo->priority);
	msgInfo->bufflen = htonl(msgInfo->bufflen);
}


void qc_msginfo_ntoh(Qc_MsgInfo *msgInfo)
{
	msgInfo->flag = ntohl(msgInfo->flag);
	msgInfo->priority = ntohl(msgInfo->priority);
	msgInfo->bufflen = ntohl(msgInfo->bufflen);
}


QcQTblFile* qc_qtbl_file_open(int msgbuff_size, int msgcount_limit, const char* table_filename, QcErr *err)
{
	qc_assert(msgbuff_size > 0 && msgcount_limit > 0 && table_filename);
	if (strlen(table_filename) > QC_QTBL_PATHLEN) {
		err->code = QC_ERR_BADLENGTH;
		strcpy(err->desc, "table name is too long.");
		return NULL;
	}

	QcQTblFile *qtbl;
	qc_malloc(qtbl, sizeof(QcQTblFile));

	strcpy(qtbl->filename, table_filename);
	qtbl->file = NULL;
	qc_malloc(qtbl->fileInfo, sizeof(Qc_QTblFileInfo));

	qtbl->fileInfo->ident = 0;
	qtbl->fileInfo->version = 1;
	qtbl->fileInfo->msgbuff_size = msgbuff_size;
	qtbl->fileInfo->msgcount_limit = msgcount_limit;

	qtbl->head_offset = sizeof(Qc_QTblFileInfo);
	qtbl->body_offset = qtbl->head_offset + sizeof(Qc_MsgInfo)*msgcount_limit;

	off_t filesize = qtbl->body_offset + msgbuff_size*msgcount_limit;

	if (0 != qc_file_exist(table_filename)) {
		//truncate file
		if (0 != qc_file_truncate(table_filename, filesize)) {
			goto failed;
		}

		QcFile *file = qc_file_open(table_filename, 0);
		if (!file) {
			goto failed;
		}

		qtbl->file = file;

		//write head info
		qc_tblfile_info_hton(qtbl->fileInfo);
		size_t sz = qc_file_write(file, qtbl->fileInfo, sizeof(Qc_QTblFileInfo));
		if (sz <= 0)
			goto failed;
	}
	else {
		size_t fsz = qc_file_size(table_filename);
		if (fsz != filesize)
			goto failed;

		QcFile *file = qc_file_open(table_filename, 0);
		if (!file) {
			goto failed;
		}

		qtbl->file = file;

		size_t sz = qc_file_read(file, qtbl->fileInfo, sizeof(sizeof(Qc_QTblFileInfo)));
		qc_tblfile_info_ntoh(qtbl->fileInfo);
		if (sz <= 0)
			goto failed;
	}
	return qtbl;

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
	qc_free(qtbl->fileInfo);
	qc_free(qtbl);

	return;
}


int qc_qtbl_file_append(QcQTblFile *qtbl, Qc_MsgRecord *msgRecord, QcErr *err)
{
	int persist_id = msgRecord->persist_id;
	off_t offset_msghead = qtbl->head_offset + persist_id*sizeof(Qc_MsgInfo);
	off_t offset_msgbody = qtbl->body_offset + persist_id*(qtbl->fileInfo->msgbuff_size);

	QcFile *file = qtbl->file;
	qc_file_seek(file, offset_msgbody, 0);
	qc_file_write(file, msgRecord->buff, msgRecord->bufflen);
	
	Qc_MsgInfo msgInfo;
	msgInfo.flag = 1;
	msgInfo.priority = msgRecord->priority;
	msgInfo.bufflen = msgRecord->bufflen;

	qc_file_seek(file, offset_msghead, 0);
	qc_msginfo_hton(&msgInfo);
	qc_file_write(file, &msgInfo, sizeof(Qc_MsgInfo));

	qc_file_sync(file);

	return 0;
}


int qc_qtbl_file_remove(QcQTblFile *qtbl, Qc_MsgRecord *msgRecord, QcErr *err)
{
	int persist_id = msgRecord->persist_id;
	off_t offset_msghead = qtbl->head_offset + persist_id * sizeof(Qc_MsgInfo);

	Qc_MsgInfo msgInfo;
	msgInfo.flag = 0;
	msgInfo.priority = 0;
	msgInfo.bufflen = 0;

	QcFile *file = qtbl->file;
	qc_file_seek(file, offset_msghead, 0);
	qc_msginfo_hton(&msgInfo);
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


int qc_qtbl_file_do_fetch(QcQTblFile *qtbl, Qc_MsgRecord *msgRecord, QcErr *err)
{
	QcFile *file = qtbl->file;
	off_t offset = qc_file_tell(file);

	//size_t filesize = qtbl->body_offset + (qtbl->fileInfo->msgbuff_size) * (qtbl->fileInfo->msgcount_limit);
	Qc_MsgInfo msgInfo;

	while (1) {
		if (offset == qtbl->body_offset)
			return 1;   //the end

		int persist_id = (offset - sizeof(Qc_QTblFileInfo))/ sizeof(Qc_MsgInfo);

		size_t hsz = qc_file_read(file, &msgInfo, sizeof(Qc_MsgInfo));
		qc_msginfo_ntoh(&msgInfo);
		if (hsz != sizeof(Qc_MsgInfo))
			return -1;

		offset += (off_t)hsz;

		if (0 == msgInfo.flag){
			continue;  //not in use, next..
		}
		else{
			int offset_buff = qtbl->body_offset + persist_id * (qtbl->fileInfo->msgbuff_size);
			qc_file_seek(file, offset_buff, 0);

			char *buff;
			qc_malloc(buff, msgInfo.bufflen);
			size_t bsz = qc_file_read(file, buff, msgInfo.bufflen);
			if (bsz != msgInfo.bufflen)
				return -1;

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
