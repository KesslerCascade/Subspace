#pragma once

#include "format.h"
#include "cx/container.h"
#include "cx/string.h"

enum FORMAT_TYPES {
    FMT_string,
    FMT_int,
    FMT_uint,
    FMT_float,
    FMT_ptr,
    FMT_suid,
    FMT_object,
    FMT_count
};
extern string _fmtTypeNames[FMT_count];
extern uint8 _fmtTypeIdMask[FMT_count][2];
extern bool(*_fmtTypeParseOpt[FMT_count])(FMTVar *v, strref opt);
extern bool(*_fmtTypeParseFinalize[FMT_count])(FMTVar *v);
extern bool(*_fmtTypeFormat[FMT_count])(FMTVar *v, string *out);

typedef struct FMTContext {
    int32 nargs;
    stvar *args;
    int32 startarg[FMT_count];      // where to start searching for each type
    int32 arrayidx;                 // arrary index counter

    string tmp;                     // temporary storage for high-level operations
    string *dest;                   // output string so far

    string fmt;                     // format string parse state
    int32 vstart;
    int32 vend;
    int32 flen;

    FMTVar v;                       // current variable
} FMTContext;

bool _fmtExtractVar(_Inout_ FMTContext *ctx);
bool _fmtParseVar(_Inout_ FMTContext *ctx);
bool _fmtFindData(_Inout_ FMTContext *ctx);
void _fmtFormat(_Inout_ FMTContext *ctx);

// type-specific formatters
bool _fmtParseStringOpt(_Inout_ FMTVar *v, _In_ strref opt);
bool _fmtString(_Inout_ FMTVar *v, _Inout_ string *out);
bool _fmtParseIntOpt(_Inout_ FMTVar *v, _In_ strref opt);
bool _fmtParseIntFinalize(_Inout_ FMTVar *v);
bool _fmtInt(_Inout_ FMTVar *v, _Inout_ string *out);
bool _fmtParseFloatOpt(_Inout_ FMTVar *v, _In_ strref opt);
bool _fmtParseFloatFinalize(_Inout_ FMTVar *v);
bool _fmtFloat(_Inout_ FMTVar *v, _Inout_ string *out);
bool _fmtParsePtrOpt(_Inout_ FMTVar *v, _In_ strref opt);
bool _fmtParsePtrFinalize(_Inout_ FMTVar *v);
bool _fmtParseObjectOpt(_Inout_ FMTVar *v, _In_ strref opt);
bool _fmtPtr(_Inout_ FMTVar *v, _Inout_ string *out);
bool _fmtSUID(_Inout_ FMTVar *v, _Inout_ string *out);
bool _fmtObject(_Inout_ FMTVar *v, _Inout_ string *out);
