#pragma once

#include <cx/cx.h>
#include "net.h"

#ifdef WIN32
#include <winsock2.h>
#include <windows.h>
#else
#include <sys/socket.h>
#endif