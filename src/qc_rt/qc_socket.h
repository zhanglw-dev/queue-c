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

#ifndef QCLIB_SOCKET_H
#define QCLIB_SOCKET_H

#include "qc_prelude.h"


typedef struct __QcSocket QcSocket;
typedef struct __QcPoll QcPoll;


#define QC_SOCK_NAGLE_ON  1
#define QC_SOCK_NAGLE_OFF 0


/**
Family:
AF_INET
AF_INET6

Type:
SOCK_STREAM
SOCK_DGRAM
SOCK_RAW
**/


/**
POLLERR    An error has occurred. 
POLLHUP    A stream-oriented connection was either disconnected or aborted. 
POLLNVAL   An invalid socket was used. 
POLLRDBAND Priority band (out-of-band) data may be read without blocking. 
POLLRDNORM Normal data may be read without blocking. 
POLLWRNORM Normal data may be written without blocking. 
**/

extern int sock_startuped;


#ifdef __cplusplus
extern "C" {
#endif


void qc_socket_startup();

void qc_socket_cleanup();

QcSocket* qc_socket_create(int family, int type, int protocol);

int qc_socket_close(QcSocket *socket);

int qc_socket_is_valid(QcSocket *socket);

/* onoff: 0 off, 1 on */
int qc_socket_nagle_onoff(QcSocket *socket, int onoff);

int qc_socket_getfd(QcSocket *socket);

int qc_tcp_bind(QcSocket *socket, const char* ip, int port);

int qc_tcp_listen(QcSocket *socket);

QcSocket* qc_tcp_accept(QcSocket *socket);

int qc_tcp_connect(QcSocket *socket, const char* ip, int port);

int qc_tcp_send(QcSocket *socket, char *sendbuf, int len);

int qc_tcp_recv(QcSocket *socket, char *recvbuf, int len);

int qc_tcp_tmsend(QcSocket *socket, char *sendbuf, int len, int nsec);

int qc_tcp_tmrecv(QcSocket *socket, char *sendbuf, int len, int nsec);

int qc_tcp_recvall(QcSocket *socket, char *recvbuf, int len);

/*----------------------------------------------------------------------------*/

QcPoll* qc_poll_alloc(unsigned long maxnum);

unsigned long qc_poll_append(QcPoll *poll, QcSocket *sock, short events);

int qc_poll_remove(QcPoll *poll, unsigned long index);

unsigned long qc_poll_getnum(QcPoll *poll);

int qc_poll_start(QcPoll *poll, int timeout);

short qc_poll_revents(QcPoll *poll, unsigned long index);

void qc_poll_free(QcPoll *poll);


#ifdef __cplusplus
}
#endif


#endif /* QCLIB_SOCKET_H */

