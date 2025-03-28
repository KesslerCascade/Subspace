#pragma once

#include <cx/sys/hostid.h>
#include <cx/platform/base.h>
#include <mbedtls/md.h>

int32 hostIdPlatformInit(mbedtls_md_context_t *shactx);

// this function is not allowed to fail!!!
int32 hostIdPlatformInitFallback(mbedtls_md_context_t *shactx);

#if defined(_PLATFORM_WIN)
#include "cx/platform/win/win_hostid.h"
#elif defined (_PLATFORM_UNIX)
#include "cx/platform/unix/unix_hostid.h"
#elif defined (_PLATFORM_WASM)
#include "cx/platform/wasm/wasm_hostid.h"
#endif
