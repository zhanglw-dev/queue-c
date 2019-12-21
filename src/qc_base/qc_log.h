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

#ifndef QC_LOG_H
#define QC_LOG_H

#include "qc_prelude.h"


#define DEBUG_LEVEL1 1
#define DEBUG_LEVEL2 2
#define DEBUG_LEVEL3 3
#define DEBUG_LEVEL4 4
#define DEBUG_LEVEL5 5
#define DEBUG_LEVEL6 6


#define qc_error(str) \
        _qc_error(__FILE__, __LINE__, (const char *)str)

#define qc_perror(fmt, ...) \
        _qc_perror(__FILE__, __LINE__, (const char *)(fmt), __VA_ARGS__)

#define qc_debug(level, str) \
        _qc_debug(__FILE__, __LINE__, level, (const char *)str)

#define qc_pdebug(level, fmt, ...) \
        _qc_pdebug(__FILE__, __LINE__, level, (const char *)(fmt), __VA_ARGS__)

#define qc_warn(str) \
        _qc_warn((const char *)str)

#define qc_pwarn(fmt, ...) \
        _qc_pwarn((const char *)(fmt), __VA_ARGS__)

#define qc_info(str) \
        _qc_info((const char *)str)

#define qc_pinfo(fmt, ...) \
        _qc_pinfo((const char *)(fmt), __VA_ARGS__)


void _qc_error(char *srcfile, int srcline, const char *str);

void _qc_perror(char *srcfile, int srcline, const char *fmt, ...);
  
void _qc_debug(char *srcfile, int srcline, int level, const char *str);

void _qc_pdebug(char *srcfile, int srcline, int level, const char *fmt, ...);

void _qc_warn(const char *str);

void _qc_pwarn(const char *fmt, ...);

void _qc_info(const char *str);

void _qc_pinfo(const char *fmt, ...);


#endif  /*QC_LOG_H*/