
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

