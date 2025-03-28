#pragma once

// This is the master include file for all CX-based components.
// It should be included by all CX header files.
//
// Changing this file will require EVERYTHING to be rebuilt, so do so sparingly.

#ifdef _WINBASE_
#error("Do not include cx.h after windows.h!");
#endif

#include <string.h>
#include <ctype.h>
#include <time.h>

#include <cx/platform/base.h>
#include <cx/platform/cpp.h>
#include <cx/stype/stype.h>
#include <cx/suid/suid.h>
#include <cx/xalloc/xalloc.h>

#ifdef __cplusplus
#include <cstdio>
#else
#include <stdio.h>
#endif
#include <stdlib.h>

