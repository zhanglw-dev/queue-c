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

#include "qc_prelude.h"
#include "qc_protocol.h"


void qc_prtcl_head_hton(QcPrtclHead *head)
{
	head->protocol = htons(head->protocol);
	head->version = htons(head->version);
	head->type = htons(head->type);
	head->packsn = htonl(head->packsn);
	head->body_len = htonl(head->body_len);
}


void qc_prtcl_head_ntoh(QcPrtclHead *head)
{
	head->protocol = ntohs(head->protocol);
	head->version = ntohs(head->version);
	head->type = ntohs(head->type);
	head->packsn = ntohl(head->packsn);
	head->body_len = ntohl(head->body_len);
}


void qc_prtcl_msgput_hton(QcPrtclMsgPut *msgput)
{
	msgput->msg_prioriy = htons(msgput->msg_prioriy);
	msgput->wait_msec = htonl(msgput->wait_msec);
	msgput->msg_len = htonl(msgput->msg_len);
}


void qc_prtcl_msgput_ntoh(QcPrtclMsgPut *msgput)
{
	msgput->msg_prioriy = ntohs(msgput->msg_prioriy);
	msgput->wait_msec = ntohl(msgput->wait_msec);
	msgput->msg_len = ntohl(msgput->msg_len);
}


void qc_prtcl_msgget_hton(QcPrtclMsgGet *msgget)
{
	msgget->wait_msec = htonl(msgget->wait_msec);
}


void qc_prtcl_msgget_ntoh(QcPrtclMsgGet *msgget)
{
	msgget->wait_msec = ntohl(msgget->wait_msec);
}


void qc_prtcl_reply_hton(QcPrtclReply *reply)
{
	reply->result = htonl(reply->result);
	reply->msg_len = htonl(reply->msg_len);
}


void qc_prtcl_reply_ntoh(QcPrtclReply *reply)
{
	reply->result = ntohl(reply->result);
	reply->msg_len = ntohl(reply->msg_len);
}
