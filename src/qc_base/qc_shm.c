#include "qc_shm.h"


struct __QcShm__ {
    char name[QC_SHMNAME_MAXLEN+1];
	char *shmaddr;
	size_t shmsize;
};



QcShm* qc_shm_create(const char *name, size_t shmsize, QcErr *err)
{
    int flags, opt, fd;
    mode_t perms;
    size_t size;
    char *addr;

    flags = O_CREAT|O_EXCL|O_RDWR;
    perms = S_IRUSR|S_IWUSR;

    //unlink shm first?
    shm_unlink(name);

    fd = shm_open(name, flags, perms);
    if(fd == -1)
    {
        qc_seterr(err, -1, "create shm failed, name : %s errno=%d\n", name, errno);
        return NULL;
    }

    if(ftruncate(fd, shmsize) == -1)
    {
        qc_seterr(err, -1, "ftruncate failed. errno=%d\n", errno);
        return NULL;
    }

    addr = mmap(NULL, shmsize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(addr == MAP_FAILED)
    {
        qc_seterr(err, -1, "mmap faied, errno=%d\n", errno);
        return NULL;
    }

    close(fd);

    QcShm *qcShm;
    qcShm = qc_malloc(qcShm, sizeof(QcShm));
    strncpy(qcShm->name, name, QC_SHMNAME_MAXLEN);
    qcShm->shmaddr = addr;
    qcShm->shmsize = shmsize;

    return qcShm;
}


int qc_shm_destroy(QcShm *qcShm)
{
    munmap(qcShm->shmaddr, qcShm->shmsize);
    shm_unlink(qcShm->name);
    return 0;
}


QcShm* qc_shm_open(const char *name, QcErr *err)
{
    int flags, opt, fd;
    mode_t mode;
    char *addr;
    struct stat sb;

    flags = O_RDWR;  //just open
    mode  = S_IRUSR|S_IWUSR;

    fd = shm_open(name, flags, mode);
    if(fd == -1)
    {
        qc_seterr(err, -1, "shm_open failed, name : %s errno=%d\n", name, errno);
        return NULL;
    }

    if(fstat(fd, &sb) == -1)
    {
        qc_seterr(err, -1, "fstat failed, errno=%d\n", errno);
        return NULL;
    }

    addr = mmap(NULL, sb.st_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if(addr == MAP_FAILED)
    {
        qc_seterr(err, -1, "mmap faied, errno=%d\n", errno);
        return NULL;
    }

    close(fd);

    QcShm *qcShm;
    qcShm = qc_malloc(qcShm, sizeof(QcShm));
    strncpy(qcShm->name, name, QC_SHMNAME_MAXLEN);
    qcShm->shmaddr = addr;
    qcShm->shmsize = sb.st_size;

    return qcShm;
}


int qc_shm_close(QcShm *qcShm)
{
    munmap(qcShm->shmaddr, qcShm->shmsize);
    return 0;
}


size_t qc_shm_getsize(QcShm *qcShm)
{
    return qcShm->shmsize;
}


char* qc_shm_getaddr(QcShm *qcShm)
{
    return qcShm->shmaddr;
}
