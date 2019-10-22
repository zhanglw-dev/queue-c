#ifndef QC_TABLE_FILE_H
#define QC_TABLE_FILE_H

#include "qc_error.h"
#include "qc_message.h"


#pragma pack(push)
#pragma pack(1)

//must as same as qc_message.c::__QcMessage !!!
typedef struct {
	int priority;
	int persist_id;
	int bufflen;
	char *buff;
}Qc_MsgRecord;

#pragma pack(pop)


typedef struct __QcQTblFile QcQTblFile;



QcQTblFile* qc_qtbl_file_open(int msgbuff_size, int msgcount_limit, const char* persit_filename, QcErr *err);

void qc_qtbl_file_close(QcQTblFile *qtbl);

int qc_qtbl_file_append(QcQTblFile *qtbl, Qc_MsgRecord *msgRecord, QcErr *err);

int qc_qtbl_file_remove(QcQTblFile *qtbl, Qc_MsgRecord *msgRecord, QcErr *err);

int qc_qtbl_file_fetch_ready(QcQTblFile *qtbl, QcErr *err);

int qc_qtbl_file_do_fetch(QcQTblFile *qtbl, Qc_MsgRecord *msgRecord, QcErr *err);


#endif  //QC_TABLE_FILE_H
