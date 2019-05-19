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

int qc_tcp_bind(QcSocket *socket, char* ip, int port);

int qc_tcp_listen(QcSocket *socket);

QcSocket* qc_tcp_accept(QcSocket *socket);

int qc_tcp_connect(QcSocket *socket, char* ip, int port);

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

