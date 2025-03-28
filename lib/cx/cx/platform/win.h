#pragma once

// For use when the Windows API is needed

#include <cx/cx.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WinSock2.h>
#include <mmsystem.h>

bool winMapLastError();
