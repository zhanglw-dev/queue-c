
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

#include "qc_file.h"
#include "qc_log.h"


struct __QcFile{
    int filedes;
};



QcFile* qc_file_open(const char *pathname, int oflag)
{
    QcFile *file;
    int mode;

    qc_assert(pathname);

    if(NULL == (file = malloc(sizeof(QcFile))))
    {
        qc_error("file hdl malloc failed");
        return NULL;
    }

    if(O_CREAT & oflag)
    {
        file->filedes = open(pathname, oflag, S_IRUSR|S_IWUSR|S_IXUSR);
    }
    else
    {
        file->filedes = open(pathname, oflag);
    }

    if(-1 == file->filedes)
    {
        qc_perror("file(pathname) open failed", pathname);
        return NULL;
    }

    return file;
}


int qc_file_close(QcFile *file)
{
    qc_assert(file);

    if(0 != close(file->filedes))
    {
        qc_error("file close failed");
        return -1;
    }

    qc_free(file);
    return 0;
}


size_t qc_file_read(QcFile *file, void *buf, size_t nbytes)
{
    size_t ssz;

    qc_assert(file);
    
    ssz = read(file->filedes, buf, nbytes);
    if(ssz < 0)
    {
        qc_error("file read failed");
        return -1;
    }

    return ssz;
}


size_t qc_file_write(QcFile *file, const void *buf, size_t nbytes)
{
    size_t ssz;

    qc_assert(file);
    
    ssz = write(file->filedes, buf, nbytes);
    if(ssz < 0)
    {
        qc_error("file write failed");
        return -1;
    }

    return ssz;
}


int qc_file_sync(QcFile *file)
{
    qc_assert(file);

    if(0 != fsync(file->filedes))
    {
        qc_error("file sync failed");
        return -1;
    }

    return 0;
}


off_t qc_file_seek(QcFile *file, off_t offset, int whence)
{
    off_t off;

    qc_assert(file);

    off = lseek(file->filedes, offset, whence);
    if(off < 0)
    {
        qc_error("file seek failed");
        return -1;
    }

    return off;
}


off_t qc_file_tell(QcFile *file)
{
    off_t off;

    qc_assert(file);

    off = lseek(file->filedes, 0, SEEK_CUR);
    if(off < 0)
    {
        qc_error("file tell failed");
        return -1;
    }

    return off;
}


int qc_file_exist(const char *pathname)
{
    qc_assert(pathname);

    if(0 != access(pathname, 0))
    {
        //qc_perror("file(%s) access failed", pathname);
        return -1;
    }

    return 0;
}


size_t qc_file_size(const char *pathname)
{
	size_t filesize = -1;
	struct stat statbuff;
	if (stat(pathname, &statbuff) < 0) {
		return filesize;
	}
	else {
		filesize = statbuff.st_size;
	}
	return filesize;
}


int qc_file_remove(const char *pathname)
{
    qc_assert(pathname);

    if (0 != remove(pathname))
    {
        qc_perror("file(%s) remove failed", pathname);
        return -1;
    }

    return 0;
}


int qc_file_rename(const char *oldname, const char *newname)
{
    qc_assert(oldname);
    qc_assert(newname);

    if (0 != rename(oldname, newname))
    {
        qc_perror("file rename(%s to %s) failed", oldname, newname);
        return -1;
    }

    return 0;
}


int qc_file_truncate(const char *pathname, off_t length)
{
    int fd = open(pathname, O_CREAT|O_WRONLY, S_IRUSR|S_IWUSR);
    if(fd <= 0){
        qc_perror("file (%s) create/open failed", pathname);
        return -1;
    }

    if(0 != ftruncate(fd, length))
    {
        qc_perror("file (%s) truncate failed", pathname);
        return -1;
    }

    close(fd);

    return 0;
}

/*-------------------------------------------------------------------*/


int qc_dir_exist(char *dirname)
{
    qc_assert(dirname);

    if(0 != access(dirname, 0))
    {
        qc_perror("dir(%s) access failed", dirname);
        return -1;
    }

    return 0;
}


int qc_dir_remove(char *dirname)
{
    qc_assert(dirname);

    if(0 != rmdir(dirname))
    {
        qc_perror("dir(%s) remove failed", dirname);
        return -1;
    }

    return 0;
}


int qc_dir_rename(char *dirname, char *newname)
{
    qc_assert(dirname);
    qc_assert(newname);

    if (0 != rename(dirname, newname))
    {
        qc_perror("dir rename(%s to %s) failed", dirname, newname);
        return -1;
    }

    return 0;
}


int qc_dir_make(char *dirname)
{
    qc_assert(dirname);

    if(0 != mkdir(dirname, 0))
    {
        qc_perror("dir(%s) make failed", dirname);
        return -1;
    }

    return 0;
}


char* qc_dir_getcwd(char *buff, int maxlen)
{
	return getcwd(buff, maxlen);
}
