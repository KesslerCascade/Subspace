#include "netsocket.h"

#ifdef WIN32

bool netInit(void)
{
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;

    wVersionRequested = MAKEWORD(2, 2);
    err               = WSAStartup(wVersionRequested, &wsaData);
    return (err == 0);
}

bool netSetNonblock(socket_t sock, bool nonblock)
{
    int mode = nonblock ? 1 : 0;
    return (ioctlsocket(sock, FIONBIO, &mode) == 0);
}

bool netClose(socket_t sock)
{
    return closesocket(sock) == 0;
}

int netError(void)
{
    int err = WSAGetLastError();
    if (err == WSAEWOULDBLOCK)
        err = EWOULDBLOCK;
    return err;
}

#else

#include <fcntl.h>

bool netInit(void)
{
    return true;
}

bool netSetNonblock(socket_t sock, bool nonblock)
{
    int flags = fcntl(sock, F_GETFL, 0);
    if (flags == -1)
        return;
    flags = nonblock ? (flags | O_NONBLOCK) : (flags & ~O_NONBLOCK);
    return (fcntl(fd, F_SETFL, flags) == 0);
}

bool netClose(socket_t sock)
{
    return close(sock) == 0;
}

int netError(void)
{
    return errno;
}

#endif

bool netSetBuffers(socket_t sock, int sendbuf, int rcvbuf)
{
    return setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (char*)(uintptr_t)sendbuf, sizeof(int)) == 0 &&
        setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char*)(uintptr_t)rcvbuf, sizeof(int)) == 0;
}
