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

#include "qc_log.h"
#include "qc_file.h"
#include "qc_thread.h"

static QcFile *_logf = NULL;
static QcMutex *_log_mutex;
static int mbytes = 10;
static int rollnum = 5;


//Todo: mbyes, rollnum
int qc_log_init(const char* logfile, int mbytes, int rollnum)
{
    _logf = qc_file_open(logfile, O_CREAT|O_WRONLY);
    if(NULL == _logf){
        _logf = (QcFile*)-1;
        return -1;
    }

    _log_mutex = qc_thread_mutex_create();
    return 0;
}


static void _qc_log_init()
{
    qc_log_init("qc.log", 10, 5);
}


static void _qc_log_write(char *logbuff)
{
    char buff[512];
    time_t t;
    struct tm * lt;
    time (&t);
    lt = localtime (&t);
    snprintf(buff, sizeof(buff), "[%4d/%2d/%2d %2d:%2d:%2d] %s",\
            lt->tm_year+1900, lt->tm_mon, lt->tm_mday, lt->tm_hour, lt->tm_min, lt->tm_sec, logbuff);

    int len = strlen(buff) + 1;
    qc_thread_mutex_lock(_log_mutex);
    qc_file_write(_logf, buff, len);
    qc_thread_mutex_unlock(_log_mutex);

    printf("%s", buff);
}


void _qc_error(char *srcfile, int srcline, const char *str)
{
    char logbuff[512];
    if(_logf == NULL) _qc_log_init();
    snprintf(logbuff, sizeof(logbuff), "error: [%s; line:%d; syserr:%d] ==> %s\n", srcfile, srcline, errno, str);
    if((int)_logf != -1){
        _qc_log_write(logbuff);
    }
}


void _qc_perror(char *srcfile, int srcline, const char *fmt, ...)
{
    char vs[512];
    va_list vl;

    va_start(vl, fmt);
    vsnprintf(vs, sizeof(vs), fmt, vl);
    _qc_error(srcfile, srcline, vs);
    va_end(vl);
}


void _qc_debug(char *srcfile, int srcline, int level, const char *str)
{
    char logbuff[512];
    if(_logf == NULL) _qc_log_init();
    snprintf(logbuff, sizeof(logbuff), "debug[%d]: [%s; line:%d] ==> %s\n", level, srcfile, srcline, str);
    if((int)_logf != -1){
        _qc_log_write(logbuff);
    }
}


void _qc_pdebug(char *srcfile, int srcline, int level, const char *fmt, ...)
{
    char vs[512];
    va_list vl;

    va_start(vl, fmt);
    vsnprintf(vs, sizeof(vs), fmt, vl);
    _qc_debug(srcfile, srcline, level, vs);
    va_end(vl);
}


void _qc_warn(const char *str)
{
    char logbuff[512];
    if(_logf == NULL) _qc_log_init();
    snprintf(logbuff, sizeof(logbuff), "warning ==> %s\n", str);
    if((int)_logf != -1){
        _qc_log_write(logbuff);
    }
}


void _qc_pwarn(const char *fmt, ...)
{
    char vs[512];
    va_list vl;

    va_start(vl, fmt);
    vsnprintf(vs, sizeof(vs), fmt, vl);
    _qc_warn(vs);
    va_end(vl);
}


void _qc_info(const char *str)
{
    char logbuff[512];
    if(_logf == NULL) _qc_log_init();
    snprintf(logbuff, sizeof(logbuff), "info ==> %s\n", str);
    if((int)_logf != -1){
        _qc_log_write(logbuff);
    }
}


void _qc_pinfo(const char *fmt, ...)
{
    char vs[512];
    va_list vl;

    va_start(vl, fmt);
    vsnprintf(vs, sizeof(vs), fmt, vl);
    _qc_info(vs);
    va_end(vl);
}

