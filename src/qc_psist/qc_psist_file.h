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


typedef struct __QcPsistFile QcPsistFile;



QcPsistFile* qc_psist_file_open(int msgbuff_size, int msgcount_limit, const char* persit_filename, QcErr *err);

void qc_psist_file_close(QcPsistFile *qtbl);

int qc_psist_file_append(QcPsistFile *qtbl, Qc_MsgRecord *msgRecord, QcErr *err);

int qc_psist_file_remove(QcPsistFile *qtbl, Qc_MsgRecord *msgRecord, QcErr *err);

int qc_psist_file_fetch_ready(QcPsistFile *qtbl, QcErr *err);

int qc_psist_file_do_fetch(QcPsistFile *qtbl, Qc_MsgRecord *msgRecord, QcErr *err);


#endif  //QC_TABLE_FILE_H
