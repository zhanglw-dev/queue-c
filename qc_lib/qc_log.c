
#include "qc_log.h"



void _qc_error(char *srcfile, int srcline, const char *str)
{
    printf("error: [%s; line:%d; syserr:%d] ==> %s\n", srcfile, srcline, errno, str);
}


void _qc_perror(char *srcfile, int srcline, const char *fmt, ...)
{
    char vs[1024];
    va_list vl;

    memset(vs, 0, sizeof(vs));
    printf("error: [%s; line:%d; syserr:%d] ==> ", srcfile, srcline, errno);

    va_start(vl, fmt);
    vsnprintf(vs, sizeof(vs), fmt, vl);
    printf("%s\n", vs);
    va_end(vl);
}


void _qc_debug(char *srcfile, int srcline, int level, const char *str)
{
    printf("debug[%d]: [%s; line:%d] ==> %s\n", level, srcfile, srcline, str);
}


void _qc_pdebug(char *srcfile, int srcline, int level, const char *fmt, ...)
{
    char vs[1024];
    va_list vl;
    static unsigned int refnum = 0;

    memset(vs, 0, sizeof(vs));
    printf("debug[%d, %d]: [%s; line:%d] ==> ", level, refnum, srcfile, srcline);

    va_start(vl, fmt);
    vsnprintf(vs, sizeof(vs), fmt, vl);
    printf("%s\n", vs);
    va_end(vl);

    refnum ++;
}


void _qc_warn(const char *str)
{
    printf("warning ==> %s\n", str);
}


void _qc_pwarn(const char *fmt, ...)
{
    char vs[1024];
    va_list vl;

    memset(vs, 0, sizeof(vs));

    va_start(vl, fmt);
    vsnprintf(vs, sizeof(vs), fmt, vl);
    printf("warning ==> %s\n", vs);
    va_end(vl);
}


void _qc_info(const char *str)
{
    printf("info ==> %s\n", str);
}


void _qc_pinfo(const char *fmt, ...)
{
    char vs[1024];
    va_list vl;

    memset(vs, 0, sizeof(vs));

    va_start(vl, fmt);
    vsnprintf(vs, sizeof(vs), fmt, vl);
    printf("info ==> %s\n", vs);
    va_end(vl);
}

