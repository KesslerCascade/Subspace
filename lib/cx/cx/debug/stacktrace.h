#pragma once

#include <cx/platform/base.h>

#if defined(_WIN32)
#include <cx/platform/win/win_stacktrace.h>
#else
//#include <cx/platform/unix/unix_stacktrace.h>
#endif
