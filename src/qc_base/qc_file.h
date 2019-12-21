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

#ifndef QC_FILE_H
#define QC_FILE_H


#include "qc_prelude.h"


typedef struct __QcFile QcFile;



#ifdef __cplusplus
extern "C" {
#endif


/*oflag: O_CREAT O_EXCL O_TRUNC O_APPEND O_RDONLY O_WRONLY O_RDWR*/
QcFile* qc_file_open(const char *pathname, int oflag);

int qc_file_close(QcFile *file);

size_t qc_file_read(QcFile *file, void *buf, size_t nbytes);

size_t qc_file_write(QcFile *file, const void *buf, size_t nbytes);

int qc_file_sync(QcFile *file);

/*whence: SEEK_SET SEEK_CUR SEEK_END*/
off_t qc_file_seek(QcFile *file, off_t offset, int whence);

off_t qc_file_tell(QcFile *file);

int qc_file_exist(const char *pathname);

size_t qc_file_size(const char *pathname);

int qc_file_remove(const char *pathname);

int qc_file_rename(const char *oldname, const char *newname);

int qc_file_truncate(const char *pathname, off_t length);

/*---------------------------------------------------------------------*/


int qc_dir_exist(char *dirname);

int qc_dir_make(char *dirname);

int qc_dir_remove(char *dirname);

int qc_dir_rename(char *dirname, char *newname);

char* qc_dir_getcwd(char *buff, int maxlen);


#ifdef __cplusplus
}
#endif


#endif  /*QC_FILE_H*/
