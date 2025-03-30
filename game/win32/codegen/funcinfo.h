#pragma once

#include <stdbool.h>

// Tightly packed function info structure containing information necessary to
// construct wrappers and hooks.

// Note: integer and pointer arguments / return types are functionally equivalent because
// they are all passed as a machine word on the stack or returned in EAX.
//
// Floating point values are pushed on the stack, but as return values are returned
// in ST(0).
//
// Structure return is not handled here. It's usually stack allocated by the caller and
// a pointer is passed as a hidden argument, either on the stack or sometimes in ECX for
// C++ functions that aren't class members. It should be declared as an extra argument
// and the return type set to void.

typedef enum { ARG_INT, ARG_PTR, ARG_STRUCT, ARG_FLOAT, ARG_DOUBLE, ARG_DUMMY } ArgType;
typedef enum { RET_INT, RET_PTR, RET_FLOAT32, RET_FLOAT64, RET_VOID } ReturnType;

typedef struct ArgInfo {
    unsigned char size;
    unsigned char type : 3;
    unsigned char reg  : 5;
    bool stack         : 1;
} ArgInfo;

typedef struct FuncInfo {
    unsigned char nargs   : 5;   // number of arguments to function
    unsigned char rettype : 3;   // return type of function
    bool stdcall          : 1;   // arguments are in stdcall order
    bool purecdecl        : 1;   // pure cdecl function; no register args
    bool vararg           : 1;   // variable number of arguments
    bool wasinlined       : 1;   // was inlined. no, really
    bool duplicates       : 1;   // more than one function with this name
    ArgInfo args[];
} FuncInfo;
