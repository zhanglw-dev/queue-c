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

static char _filename[QC_FILENAME_MAXLEN] = {0};
static QcFile *_logf = NULL;
static QcMutex *_log_mutex = NULL;
static off_t _kbytes = 10;
static int _rollnum = 5;
static off_t _curoffset = 0;
static int _debug_level = 0;


//Todo: mbyes, rollnum
int qc_log_init(const char* logfile, int debug_level, off_t kbytes, int rollnum)
{
    if(_kbytes <= 0)
    {
        qc_error("illegal parameters, kbytes should > 0");
        return -1;
    }

    if(_rollnum <= 0)
    {
        qc_error("illegal parameters, rollnum should > 0");
        return -1;
    }

    _kbytes = kbytes;
    _rollnum = rollnum;
    _curoffset = 0;
    _debug_level = debug_level;

    _logf = qc_file_open(logfile, O_CREAT|O_APPEND|O_WRONLY);
    if(NULL == _logf)
    {
        qc_perror("create/open log file(%s) failed.", logfile);
        return -1;
    }

    _curoffset = qc_file_tell(_logf);

    if(strlen(_filename) == 0)
        strncpy((char*)_filename, logfile, sizeof(_filename));

    if(NULL == _log_mutex)
        _log_mutex = qc_thread_mutex_create();

    return 0;
}


static int _rollfiles()
{
    char filename[QC_FILENAME_MAXLEN+16];      //16 for compile warnning
    char new_filename[QC_FILENAME_MAXLEN+32];  //32 for compile warnning

    qc_file_close(_logf);
    _logf = NULL;

    for(int i=_rollnum-1; i>0; i--)
    {
        snprintf((char*)filename, sizeof(filename), "%s_%d", _filename, i);

        if(0 == qc_file_exist(filename))
        {
            if(i == _rollnum-1)
            {
                if(0!=qc_file_remove(filename))
                {
                    printf("log rollfiles error, remove %s failed.\n", filename);
                    return -1;
                }
            }
            else
            {
                snprintf((char*)new_filename, sizeof(new_filename), "%s_%d", _filename, i+1);
                
                if(0!=qc_file_rename(filename, new_filename))
                {
                    printf("log rollfiles error, rename %s to %s failed.\n", filename, new_filename);
                    return -1;
                }
            }
        }
    }

    snprintf((char*)new_filename, sizeof(new_filename), "%s_%d", _filename, 1);

    if(0!=qc_file_rename(_filename, new_filename))
    {
        printf("log rollfiles error, rename %s to %s failed.\n", _filename, new_filename);
        return -1;
    }

    if(0 != qc_log_init(_filename, _debug_level, _kbytes, _rollnum))
    {
        printf("log rollfiles error, create %s failed.\n", _filename);
        return -1;
    }

    return 0;
}


static void _qc_log_write(char *logbuff)
{
    char buff[512];
    time_t t;
    struct tm * lt;
    size_t wtn;
    int len;

    time (&t);
    lt = localtime (&t);
    snprintf(buff, sizeof(buff), "[%4d/%2d/%2d %2d:%2d:%2d] %s\n",\
            lt->tm_year+1900, lt->tm_mon, lt->tm_mday, lt->tm_hour, lt->tm_min, lt->tm_sec, logbuff);

    len = (int)strlen(buff);
    qc_thread_mutex_lock(_log_mutex);

    if(_logf)
    {
        wtn = qc_file_write(_logf, buff, len);
        _curoffset += (int)wtn;

        if(_curoffset >= _kbytes*1024)
        {
            _rollfiles();
        }

    }

    qc_thread_mutex_unlock(_log_mutex);
}


void _qc_error(char *srcfile, int srcline, const char *str)
{
    char logbuff[512];
    snprintf(logbuff, sizeof(logbuff), "error: [%s; line:%d; syserr:%d] ==> %s", srcfile, srcline, errno, str);
    if(NULL != _logf){
        _qc_log_write(logbuff);
    }

    printf("%s\n", logbuff);
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
    snprintf(logbuff, sizeof(logbuff), "debug[level %d]: [%s; line:%d] ==> %s", level, srcfile, srcline, str);

    if(_debug_level >= level && NULL != _logf)
    {
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
    snprintf(logbuff, sizeof(logbuff), "warning ==> %s", str);
    if(NULL != _logf)
    {
        _qc_log_write(logbuff);
    }

    printf("%s\n", logbuff);
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
    snprintf(logbuff, sizeof(logbuff), "info ==> %s", str);
    if(NULL != _logf)
    {
        _qc_log_write(logbuff);
    }

    printf("%s\n", logbuff);
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
