
#include "qc_socket.h"
#include "qc_log.h"
#include "qc_prelude.h"


struct __QcSocket{
    int sockfd;
};


struct __QcPoll{
	unsigned long maxnum;
	unsigned long num;
	struct pollfd *fdarray;
};



/*just for MS Windows*/
void qc_socket_startup(){return;}
void qc_socket_cleanup(){return;}


QcSocket* qc_socket_create(int family, int type, int protocol)
{
    QcSocket *sock;

    qc_malloc(sock, sizeof(QcSocket));

    sock->sockfd = (int)socket(family, type, protocol);
    if(sock->sockfd < 0)
    {
        qc_error("socket create failed");
        qc_free(sock);
        return NULL;
    }

    return sock;
}


int qc_socket_close(QcSocket *socket)
{
    qc_assert(socket);

    if(0 != _close(socket->sockfd))
    {
        qc_error("socket destroy failed");
        return -1;
    }

    qc_free(socket);
    socket = NULL;  /*confirm it should be NULL*/
    return 0;
}


int qc_socket_is_valid(QcSocket *socket)
{
    return socket!=NULL?1:0;
}


int qc_socket_nagle_onoff(QcSocket *socket, int onoff)
{
    int flag, ret;

    qc_assert(socket);

    flag = onoff;
    ret = setsockopt(socket->sockfd, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(flag));

    if(ret == -1){
        qc_error("couldn't setsockopt(TCP_NODELAY)\n");
        return -1;
    }

    return 0;
}


int qc_socket_getfd(QcSocket *socket)
{
    qc_assert(socket);

    return socket->sockfd;
}


int qc_tcp_bind(QcSocket *socket, char* ip, int port)
{
    struct sockaddr_in servaddr;

    qc_assert(socket);
    qc_assert(ip);
    qc_assert(port>0);

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);

    if(NULL == ip || (0 == strcmp(ip, "127.0.0.1")))
        servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    else
        //servaddr.sin_addr.s_addr = inet_addr(ip);
	    inet_pton(AF_INET, ip, &servaddr.sin_addr.s_addr);

    if(0 != bind(socket->sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)))
    {
        qc_perror("socket bind failed (ip=%s port=%d)", ip, port);
        return -1;
    }

    return 0;
}


int qc_tcp_listen(QcSocket *socket)
{
    qc_assert(socket);

    if(0 != listen(socket->sockfd, SOMAXCONN))
    {
        qc_error("socket listen failed");
        return -1;
    }

    return 0;
}


QcSocket* qc_tcp_accept(QcSocket *socket)
{
    QcSocket *new_sock;
    struct sockaddr addr;
    int addr_len;

    qc_assert(socket);

    qc_malloc(new_sock, sizeof(QcSocket));

    addr_len = sizeof(addr);

    new_sock->sockfd = (int)accept(socket->sockfd, &addr, &addr_len);
    if(new_sock->sockfd < 0)
    {
        qc_error("socket accept failed");
        qc_free(new_sock);
        return NULL;
    }

    return new_sock;
}


int qc_tcp_connect(QcSocket *socket, char* ip, int port)
{
    struct sockaddr_in addr;

    qc_assert(socket);
    qc_assert(ip);
    qc_assert(port>0);

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    //addr.sin_addr.s_addr = inet_addr(ip);
	inet_pton(AF_INET, ip, &addr.sin_addr.s_addr);

    if(0 != connect(socket->sockfd, (struct sockaddr*)&addr, sizeof(addr)))
    {
        qc_perror("tcp connect failed (ip=%s, port=%d)", ip, port);
        return -1;
    }

    return 0;
}


int qc_tcp_send(QcSocket *socket, char *sendbuf, int len)
{
    size_t bytes;

    bytes = send(socket->sockfd, sendbuf, len, 0);
    if(bytes < 0)
    {
        qc_error("tcp send failed");
        return -1;
    }

    return (int)bytes;
}


int qc_tcp_recv(QcSocket *socket, char *recvbuf, int len)
{
    int bytes;

    bytes = (int)recv(socket->sockfd, recvbuf, len, 0);
    if(bytes < 0)
    {
        qc_error("tcp recv failed");
        return -1;
    }
    else if(0 == bytes)
        return 0;  /*Connection closed*/

    return bytes;
}


int qc_tcp_recvall(QcSocket *socket, char *recvbuf, int len)
{
    int bytes, rcved=0, torcv=0;

    torcv = len;
    while(1)
    {
        bytes = (int)recv(socket->sockfd, recvbuf+rcved, torcv, 0);
        if(bytes < 0)
        {
            qc_error("tcp recv(in recvall) failed");
            return -1;
        }
        else if(0 == bytes)
            return 0;  /*Connection closed*/

        rcved += bytes;
        if(rcved < len)
        {
            torcv = len - rcved;
            continue;
        }
        else
            return len;
    }
}


int qc_tcp_tmsend(QcSocket *socket, char *sendbuf, int len, int nsec)
{
    /*use select*/
    return 0;
}


int qc_tcp_tmrecv(QcSocket *socket, char *recvbuf, int len, int nsec)
{
    /*use select*/
    return 0;
}

/*---------------------------------------------------------------------------------------*/

QcPoll* qc_poll_alloc(unsigned long maxnum)
{
    QcPoll *poll;

    qc_assert(maxnum>0);

    qc_malloc(poll, sizeof(QcPoll));

    if(NULL == (poll->fdarray = malloc(sizeof(struct pollfd)*maxnum)))
    {
        qc_error("poll fdarray malloc failed");
        qc_free(poll);
        return NULL;
    }

    poll->maxnum = maxnum;
    poll->num = 0;
    return poll;
}


unsigned long qc_poll_append(QcPoll *poll, QcSocket *sock, short events)
{
    struct pollfd *poll_fd;

    qc_assert(poll);
    qc_assert(sock);
    qc_assert(events);

    if(poll->num >= poll->maxnum )
    {
	qc_perror("override sockets num(%d) in poll", poll->num);
        return -1;
    }

    poll->num++;
    poll_fd = &poll->fdarray[poll->num-1];
    poll_fd->fd = qc_socket_getfd(sock);
    poll_fd->events = events;

    return 0;
}


int qc_poll_remove(QcPoll *poll, unsigned long index)
{
    unsigned long i;

    qc_assert(poll);
    qc_assert(index>0);
    qc_assert(poll->num < index);

    for(i = index; i<poll->num; i++)
    {
        poll->fdarray[i-1].fd = poll->fdarray[i].fd;
        poll->fdarray[i-1].events  = poll->fdarray[i].events;
        poll->fdarray[i-1].revents = poll->fdarray[i].revents;
    }

    poll->num--;
    return 0;
}


unsigned long qc_poll_getnum(QcPoll *poll)
{
    return poll->num;
}


int qc_poll_start(QcPoll *poll_, int timeout)
{
    int n;

    n = poll(poll_->fdarray, poll_->num, timeout);
    if(n<0)
    {
        qc_error("poll failed");
        return -1;
    }

    return n;
}


short qc_poll_revents(QcPoll *poll, unsigned long index)
{
    return poll->fdarray[index-1].revents;
}


void qc_poll_free(QcPoll *poll)
{
    qc_free(poll->fdarray);
    qc_free(poll);
}

