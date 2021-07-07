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

#ifndef H_QC_ERROR
#define H_QC_ERROR

#include "qc_prelude.h"

#define QC_ERR_BADPARAM  10
#define QC_ERR_BADLENGTH 11
#define QC_ERR_RUNTIME   20
#define QC_ERR_IO        21
#define QC_ERR_VERIFY    22
#define QC_ERR_SOCKET    30
#define QC_ERR_QUEUE_NOTEXIST 40
#define QC_ERR_INTERNAL 50
#define QC_ERR_TIMEOUT 52
#define QC_ERR_UNKOWN  99

#define QC_MAXSIZE_ERRDESC 255


typedef struct __QcErr{
    int  code;
    char desc[QC_MAXSIZE_ERRDESC+1];
}QcErr;



#ifdef __cplusplus
extern "C" {
#endif


int qc_seterr(QcErr *err, int errcode, const char *fmt, ...);


#ifdef __cplusplus
}
#endif


#endif /*H_QC_ERROR*/

