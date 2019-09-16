
#include "qc_file.h"
#include "qc_log.h"


struct __QcFile{
	HANDLE hdl;
};




QcFile* qc_file_open(const char *pathname, int oflag)
{
	QcFile *File;
	DWORD dwAccess = 0; DWORD dwShard = 0;
	DWORD dwCreatDis = 0; DWORD dwFlagsAndAttrs = 0;
	LPSECURITY_ATTRIBUTES qcSecurity;
	LARGE_INTEGER off;
	HANDLE hTempFile;

	qc_assert(pathname);

	File = malloc(sizeof(struct __QcFile));
	if(NULL == File)
	{
		qc_error("file hdl malloc failed");
		return NULL;
	}

	//dwDesiredAccess 
	if(O_RDONLY & oflag) dwAccess = GENERIC_READ;
	if(O_WRONLY & oflag) dwAccess = GENERIC_WRITE;
	if(O_RDWR   & oflag) dwAccess = (GENERIC_READ|GENERIC_WRITE);

	//dwShareMode
	dwShard = (FILE_SHARE_READ|FILE_SHARE_WRITE);

	//qcSecurityAttributes
	qcSecurity = NULL;

	//dwCreationDisposition
	if(O_CREAT & oflag) dwCreatDis = OPEN_ALWAYS;
	if(O_EXCL  & oflag) dwCreatDis = CREATE_NEW;
	if(O_TRUNC & oflag) dwCreatDis = TRUNCATE_EXISTING;

	//dwFlagsAndAttributes
	dwFlagsAndAttrs = FILE_ATTRIBUTE_NORMAL;

	//hTemplateFile
	hTempFile = NULL;

	File->hdl = CreateFile((LPCTSTR)pathname, dwAccess, dwShard, \
		               qcSecurity, dwCreatDis, dwFlagsAndAttrs, hTempFile );

	if(INVALID_HANDLE_VALUE == File->hdl)
	{
		qc_perror("file(%s) open failed", pathname);
		free(File);
		return NULL;
	}

	if(O_APPEND & oflag)
	{
		GetFileSizeEx(File->hdl, &off);   
		SetFilePointerEx(File->hdl,off,0,FILE_BEGIN); 
	}

	return File;
}


int qc_file_close(QcFile *file)
{
	qc_assert(file);

	if(!CloseHandle(file->hdl))
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

	if(!ReadFile(file->hdl, (LPVOID)buf, (DWORD)nbytes, (LPDWORD)&ssz, NULL))
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

	if(!WriteFile(file->hdl, (CONST VOID*)buf, (DWORD)nbytes, (LPDWORD)&ssz, NULL))
	{
		qc_error("file write failed");
		return -1;
	}

	return ssz;
}


int qc_file_sync(QcFile *file)
{
	qc_assert(file);

	if(!FlushFileBuffers(file->hdl))
	{
		qc_error("file sync failed");
		return -1;
	}

	return 0;
}


off_t qc_file_seek(QcFile *file, off_t offset, int whence)
{
	LARGE_INTEGER off_2move;
	LARGE_INTEGER off_new;

	qc_assert(file);

	off_2move.QuadPart = offset;

	if(!SetFilePointerEx(file->hdl, off_2move, &off_new, (DWORD)whence))
	{
		qc_perror("file seek(offset=%d, whence=%d) failed", offset, whence);
		return -1;
	}

	return off_new.LowPart;
}


off_t qc_file_tell(QcFile *file)
{
	LARGE_INTEGER off2move;
	LARGE_INTEGER off;

	qc_assert(file);

	off2move.QuadPart = 0;

	if(!SetFilePointerEx(file->hdl, off2move, &off, FILE_CURRENT))
	{
		qc_error("file tell failed");
		return -1;
	}

	return (off_t)off.QuadPart;
}


int qc_file_exist(char *pathname)
{
	qc_assert(pathname);

	if(0 != _access_s(pathname, 0))
	{
		return -1;
	}

	return 0;
}


int qc_file_remove(const char *pathname)
{
	qc_assert(pathname);

     if (!DeleteFile((LPCWSTR)pathname))
	 {
		 qc_error("file(%s) remove failed");
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


/*--------------------------------------------------------------------------------------------------*/


int qc_dir_exist(char *dirname)
{
	qc_assert(dirname);

	if(0 != _access_s(dirname, 0))
	{
		qc_perror("dir(%s) asscess failed", dirname);
		return -1;
	}

	return 0;
}


int qc_dir_remove(char *dirname)
{
	qc_assert(dirname);

	if(!RemoveDirectory((LPCWSTR)dirname))
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

    if (!CreateDirectory((LPCWSTR)dirname, NULL))
	{
		qc_perror("dir(%s) make failed", dirname);
        return -1;
	}

	return 0;
}
