#pragma once

#include <cx/digest/digest.h>
#include <cx/platform/base.h>
#include <cx/sys/hostid.h>

int32 hostIdPlatformInit(Digest* shactx);

// this function is not allowed to fail!!!
int32 hostIdPlatformInitFallback(Digest* shactx);

#if defined(_PLATFORM_WIN)
#include "cx/platform/win/win_hostid.h"
#elif defined (_PLATFORM_UNIX)
#include "cx/platform/unix/unix_hostid.h"
#elif defined (_PLATFORM_WASM)
#include "cx/platform/wasm/wasm_hostid.h"
#endif
