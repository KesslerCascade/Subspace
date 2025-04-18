#pragma once

#include <stdbool.h>
#include <stdint.h>

#ifdef WIN32
#include <winsock2.h>
#include <windows.h>
typedef SOCKET socket_t;
#else
#include <sys/socket.h>
typedef int socket_t;
#define closesocket close
#endif

bool netInit(void);
bool netSetNonblock(socket_t sock, bool nonblock);
bool netSetBuffers(socket_t sock, int sendbuf, int rcvbuf);
bool netClose(socket_t sock);
int netError(void);