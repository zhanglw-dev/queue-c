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

#ifndef QC_PROTOCOL_H
#define QC_PROTOCOL_H

#define QC_PROTOCOL_MQ    55555
#define QC_PROTOCOL_VERSION 1

#define QC_TYPE_MSGPUT    120
#define QC_TYPE_MSGGET    121
#define QC_TYPE_REPLY     130

#pragma pack(push)
#pragma pack(1)


typedef struct {
	unsigned short protocol;
	unsigned short version;
	unsigned short type;
	unsigned int packsn;
	unsigned int body_len;
	char reserve[16];
}QcPrtclHead;


typedef struct {
	char qname[32];
	int wait_msec;
	unsigned short msg_prioriy;
	unsigned int msg_len;
}QcPrtclMsgPut;


typedef struct {
	char qname[32];
	int wait_msec;
}QcPrtclMsgGet;


typedef struct {
	int result;
	unsigned int msg_len;
}QcPrtclReply;


#pragma pack(pop)


void qc_prtcl_head_hton(QcPrtclHead *head);
void qc_prtcl_head_ntoh(QcPrtclHead *head);

void qc_prtcl_msgget_hton(QcPrtclMsgGet *msgget);
void qc_prtcl_msgget_ntoh(QcPrtclMsgGet *msgget);

void qc_prtcl_msgput_hton(QcPrtclMsgPut *msgput);
void qc_prtcl_msgput_ntoh(QcPrtclMsgPut *msgput);

void qc_prtcl_reply_hton(QcPrtclReply *reply);
void qc_prtcl_reply_ntoh(QcPrtclReply *reply);


#endif /*QC_PROTOCOL_H*/
