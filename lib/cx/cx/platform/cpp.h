#pragma once

// We don't really like C++, but sometimes it is a necessary evil when
// header files are pulled into unusual places.

#ifdef __cplusplus
#define CX_C extern "C"
#define CX_C_BEGIN extern "C"{
#define CX_C_END   }
#else
#define CX_C
#define CX_C_BEGIN
#define CX_C_END
#endif
