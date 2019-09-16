
#include "qc_socket.h"
#include "qc_log.h"



int sock_startuped = 0;


struct __QcSocket{
	SOCKET sockid;
};


struct __QcPoll{
	unsigned long maxnum;
	unsigned long num;
	WSAPOLLFD *fdarray;
};



void qc_socket_startup()
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2,2), &wsaData);
    sock_startuped = 1;
}


void qc_socket_cleanup()
{
	WSACleanup();
    sock_startuped = 0;
}


QcSocket* qc_socket_create(int family, int type, int protocol)
{
    QcSocket *socket;

    if(!sock_startuped)
        qc_socket_startup();

    socket = malloc(sizeof(struct __QcSocket));
    if(NULL == socket)
	{
		qc_error("socket malloc failed");
        return NULL;
	}

    socket->sockid = WSASocket(family, type, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
    if(INVALID_SOCKET == socket->sockid)
    {
	    qc_error("socket create failed");
	    WSACleanup();
	    free(socket);
	    return NULL;
    }

    return socket;
}


int qc_socket_close(QcSocket *socket)
{
    qc_assert(socket);

	closesocket(socket->sockid);
	socket->sockid = INVALID_SOCKET;
	free(socket);
	
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
    ret = setsockopt(socket->sockid, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(flag));

    if (ret == -1) {
        qc_error("Couldn't setsockopt(TCP_NODELAY)\n");
        return -1;
    }

    return 0;
}


int qc_socket_getfd(QcSocket *socket)
{
	qc_assert(socket);

	return (int)socket->sockid;
}


int qc_tcp_bind(QcSocket *socket, char* ip, int port)
{
	SOCKADDR_IN ServerAddr;

	qc_assert(socket);
	qc_assert(ip);
	qc_assert(port>0);

	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(port);

	if(NULL == ip)
		ServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	else
		inet_pton(AF_INET, ip, &ServerAddr.sin_addr.s_addr);

	if (bind(socket->sockid, (SOCKADDR*)&ServerAddr, sizeof(ServerAddr)) == SOCKET_ERROR){
		qc_perror("socket bind() failed. ip=%s port=%d\n", ip, port);
		closesocket(socket->sockid);
		return -1;
	}

	return 0;
}


int qc_tcp_listen(QcSocket *socket)
{
    qc_assert(socket);

	if (listen(socket->sockid, SOMAXCONN ) == SOCKET_ERROR)
	{
		qc_error("socket listening failed.\n");
		return -1;
	}

	return 0;
}


QcSocket* qc_tcp_accept(QcSocket *socket)
{
	QcSocket *new_sock;
	SOCKADDR_IN Addr;
	int addr_len;

	qc_assert(socket);

	addr_len = sizeof(Addr);

	new_sock = malloc(sizeof(struct __QcSocket));
	if(NULL == new_sock)
	{
		return NULL;
	}

	new_sock->sockid = accept(socket->sockid, (SOCKADDR*)&Addr, &addr_len);
	if (INVALID_SOCKET == new_sock->sockid){
		qc_error("socket accept failed");
		closesocket(new_sock->sockid);
		free(new_sock);
		return NULL;
	}

	return new_sock;
}


int qc_tcp_connect(QcSocket *socket, char* ip, int port)
{
	SOCKADDR_IN ClientAddr;

	qc_assert(socket);
	qc_assert(ip);
	qc_assert(port>0);

	ClientAddr.sin_family = AF_INET;
	ClientAddr.sin_port = htons(port);
	inet_pton(AF_INET, ip, &ClientAddr.sin_addr.s_addr);

	if (connect(socket->sockid, (SOCKADDR*)&ClientAddr, sizeof(ClientAddr)) == SOCKET_ERROR)
	{
		qc_perror("socket connect failed. ip=%s port=%d\n", ip, port);
		return -1;
	}

	return 0;
}


int qc_tcp_send(QcSocket *socket, char *sendbuf, int len)
{
    int ret;

	qc_assert(socket);

    ret = send(socket->sockid, sendbuf, len, 0);
	if(ret == SOCKET_ERROR)
	{
        /* qc_error("socket send() failed"); */
        return -1;
    }

	return ret;
}


int qc_tcp_recv(QcSocket *socket, char *recvbuf, int len)
{
	int result;

	qc_assert(socket);

	result = recv(socket->sockid, recvbuf, len, 0);
    if (result > 0)
        return result;
    else if(0 == result)
		return 0; /* Connection closed */
    else
	{
        /* qc_error("socket recv failed"); */
        return -1;
	}
}


int qc_tcp_recvall(QcSocket *socket, char *recvbuf, int len)
{
    int ret, rcved=0, torcv=0;

    torcv = len;
    while(1)
    {
        ret = qc_tcp_recv(socket, recvbuf+rcved, torcv);
        if(0 == ret)
            return 0;
        else if(ret<0)
            return -1;

        rcved += ret;
        if(rcved < len)
        {
            torcv = len - rcved;
            continue;
        }
        else
        {
            return len;
        }
    }
}


int qc_tcp_tmsend(QcSocket *socket, char *sendbuf, int len, int nsec)
{
	DWORD rc = 0, err = 0, flags = 0, snd_types = 0;
	WSAOVERLAPPED SendOverlapped = {0};
	WSABUF DataBuff;

	qc_assert(socket);

    SendOverlapped.hEvent = WSACreateEvent();
    if(NULL == SendOverlapped.hEvent)
	{
        qc_error("WSACreateEvent failed");
		return -1;
	}

	DataBuff.len = len;
	DataBuff.buf = sendbuf;

    rc = WSASend(socket->sockid, &DataBuff, 1, &snd_types, 0, &SendOverlapped, NULL);
    if ( (rc == SOCKET_ERROR) && (WSA_IO_PENDING != (err = WSAGetLastError()))) {
        qc_error("WSASend failed");
        return -1;
    }

    rc = WSAWaitForMultipleEvents(1, &SendOverlapped.hEvent, TRUE, nsec, TRUE);
    if (rc == WSA_WAIT_FAILED) {
        qc_error("WSAWaitForMultipleEvents failed");
        return -1;
    }

    rc = WSAGetOverlappedResult(socket->sockid, &SendOverlapped, &snd_types, FALSE, &flags);
    if (rc == FALSE) {
        qc_error("WSAGetOverlappedResult failed");
        return -1;
    }

	return 0;
}


int qc_tcp_tmrecv(QcSocket *socket, char *recvbuf, int len, int nsec)
{
	DWORD rc = 0, err = 0, flags = 0, rev_types = 0;
	WSAOVERLAPPED RecvOverlapped = {0};
	WSABUF DataBuff;

	qc_assert(socket);

    RecvOverlapped.hEvent = WSACreateEvent();
    if(NULL == RecvOverlapped.hEvent)
	{
        qc_error("WSACreateEvent failed");
		return -1;
	}

	DataBuff.len = len;
	DataBuff.buf = recvbuf;

	rc = WSARecv(socket->sockid, &DataBuff, 1, &rev_types, &flags, &RecvOverlapped, NULL);
    if ( (rc == SOCKET_ERROR) && (WSA_IO_PENDING != (err = WSAGetLastError()))) {
        qc_error("WSARecv failed");
		return -1;
    }

    rc = WSAWaitForMultipleEvents(1, &RecvOverlapped.hEvent, TRUE, nsec, TRUE);
    if (rc == WSA_WAIT_FAILED) {
        qc_error("WSAWaitForMultipleEvents failed");
        return -1;
    }

    rc = WSAGetOverlappedResult(socket->sockid, &RecvOverlapped, &rev_types, FALSE, &flags);
    if (rc == FALSE) {
        qc_error("WSAGetOverlappedResult failed");
        return -1;
    }

	WSAResetEvent(RecvOverlapped.hEvent);

	return rev_types;
}


/*---------------------------------------------------------------------------------------------------------*/


QcPoll* qc_poll_alloc(unsigned long maxnum)
{
	QcPoll *poll;

    poll = malloc(sizeof(QcPoll));
	if(NULL == poll)
	{
        qc_error("poll malloc failed");
		return NULL;
	}
    memset(poll, 0, sizeof(QcPoll));

	if((poll->fdarray = malloc(sizeof(WSAPOLLFD)*maxnum)) == NULL)
	{
		qc_error("poll fdarray malloc failed");
		return NULL;
	}

	poll->maxnum = maxnum;
	poll->num = 0;

	return poll;
}


unsigned long qc_poll_append(QcPoll *poll, QcSocket *sock, short events)
{
	WSAPOLLFD *pollfd;
	unsigned long num = poll->num;
	unsigned long max = poll->maxnum;

	if(num >= max)
	{
		qc_perror("append num(%d) > maxnum limit(%n)", num, max);
		return -1;
	}

	poll->num++;
	pollfd = &poll->fdarray[poll->num - 1];
	pollfd->fd = qc_socket_getfd(sock);
	pollfd->events = events;

	return 0;
}


int qc_poll_remove(QcPoll *poll, unsigned long index)
{
	unsigned long i;

    qc_assert(poll);
	qc_assert(index>0);

	if(index > poll->num)
	{
		qc_perror("index(%d) > appended num(%d)", index, poll->num);
		return -1;
	}

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


int qc_poll_start(QcPoll *poll, int timeout)
{
	int ret;

	ret = WSAPoll(poll->fdarray, poll->num, timeout);
	if(SOCKET_ERROR == ret)
	{
		return -1;
	}

	return ret;
}


short qc_poll_revents(QcPoll *poll, unsigned long index)
{
	return poll->fdarray[index-1].revents;
}


void qc_poll_free(QcPoll *poll)
{
	free(poll->fdarray);
	free(poll);
}

