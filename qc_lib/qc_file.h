#ifndef QCLIB_FILE_H
#define QCLIB_FILE_H


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

int qc_file_exist(char *pathname);

int qc_file_remove(const char *pathname);

int qc_file_rename(const char *oldname, const char *newname);


/*---------------------------------------------------------------------*/


int qc_dir_exist(char *dirname);

int qc_dir_make(char *dirname);

int qc_dir_remove(char *dirname);

int qc_dir_rename(char *dirname, char *newname);



#ifdef __cplusplus
}
#endif


#endif  /*QCLIB_FILE_H*/

