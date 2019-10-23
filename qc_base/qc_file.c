
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

    free(file);
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
        qc_perror("file(%s) access failed", pathname);
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
