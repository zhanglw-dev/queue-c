
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

static QcFile *logf = NULL;
static QcMutex *log_mutex;
static char logbuff[1024];


void qc_log_init()
{
    logf = qc_file_open("log.txt", O_CREAT|O_WRONLY);
    if(NULL == logf)
        logf = (QcFile*)-1;

    log_mutex = qc_thread_mutex_create();
}


void _qc_error(char *srcfile, int srcline, const char *str)
{
    if(logf == NULL) qc_log_init();
    snprintf(logbuff, sizeof(logbuff), "error: [%s; line:%d; syserr:%d] ==> %s\n", srcfile, srcline, errno, str);
    if((int)logf != -1){
        qc_thread_mutex_lock(log_mutex);
        qc_file_write(logf, logbuff, strlen(logbuff)+1);
        qc_thread_mutex_unlock(log_mutex);
    }
    printf("%s", logbuff);
}


void _qc_perror(char *srcfile, int srcline, const char *fmt, ...)
{
    char vs[512];
    va_list vl;

    memset(vs, 0, sizeof(vs));

    va_start(vl, fmt);
    vsnprintf(vs, sizeof(vs), fmt, vl);
    _qc_error(srcfile, srcline, vs);
    va_end(vl);
}


void _qc_debug(char *srcfile, int srcline, int level, const char *str)
{
    if(logf == NULL) qc_log_init();
    snprintf(logbuff, sizeof(logbuff), "debug[%d]: [%s; line:%d] ==> %s\n", level, srcfile, srcline, str);
    if((int)logf != -1){
        qc_thread_mutex_lock(log_mutex);
        qc_file_write(logf, logbuff, strlen(logbuff)+1);
        qc_thread_mutex_unlock(log_mutex);
    }
    printf("%s", logbuff);
}


void _qc_pdebug(char *srcfile, int srcline, int level, const char *fmt, ...)
{
    char vs[512];
    va_list vl;

    memset(vs, 0, sizeof(vs));

    va_start(vl, fmt);
    vsnprintf(vs, sizeof(vs), fmt, vl);
    _qc_debug(srcfile, srcline, level, vs);
    va_end(vl);
}


void _qc_warn(const char *str)
{
    if(logf == NULL) qc_log_init();
    snprintf(logbuff, sizeof(logbuff), "warning ==> %s\n", str);
    if((int)logf != -1){
        qc_thread_mutex_lock(log_mutex);
        qc_file_write(logf, logbuff, strlen(logbuff)+1);
        qc_thread_mutex_unlock(log_mutex);
    }
    printf("%s", logbuff);
}


void _qc_pwarn(const char *fmt, ...)
{
    char vs[512];
    va_list vl;

    memset(vs, 0, sizeof(vs));

    va_start(vl, fmt);
    vsnprintf(vs, sizeof(vs), fmt, vl);
    _qc_warn(vs);
    va_end(vl);
}


void _qc_info(const char *str)
{
    if(logf == NULL) qc_log_init();
    snprintf(logbuff, sizeof(logbuff), "info ==> %s\n", str);
    if((int)logf != -1){
        qc_thread_mutex_lock(log_mutex);
        qc_file_write(logf, logbuff, strlen(logbuff)+1);
        qc_thread_mutex_unlock(log_mutex);
    }
    printf("%s", logbuff);    
}


void _qc_pinfo(const char *fmt, ...)
{
    char vs[512];
    va_list vl;

    memset(vs, 0, sizeof(vs));

    va_start(vl, fmt);
    vsnprintf(vs, sizeof(vs), fmt, vl);
    _qc_info(vs);
    va_end(vl);
}

