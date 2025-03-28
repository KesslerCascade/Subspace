#pragma once

#include <stdbool.h>

// Tightly packed function info structure containing information necessary to
// construct wrappers and hooks.

typedef enum { ARG_INT, ARG_PTR, ARG_STRUCT, ARG_FLOAT, ARG_DOUBLE, ARG_DUMMY } ArgType;

typedef struct ArgInfo {
    unsigned char size;
    unsigned char type : 3;
    unsigned char reg  : 5;
    bool stack         : 1;
} ArgInfo;

typedef struct FuncInfo {
    unsigned char nargs : 5;   // number of arguments to function
    bool stdcall        : 1;   // arguments are in stdcall order
    bool purecdecl      : 1;   // pure cdecl function; no register args
    bool vararg         : 1;   // variable number of arguments
    bool wasinlined     : 1;   // was inlined. no, really
    bool duplicates     : 1;   // more than one function with this name
    ArgInfo args[];
} FuncInfo;
