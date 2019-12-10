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