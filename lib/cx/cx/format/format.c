#include "format_private.h"
#include "cx/string/string_private.h"

// literal strings with embedded length for maximum efficiency
string _fmtTypeNames[FMT_count] = {
    (string)"\xE1\xC1\x06""string",
    (string)"\xE1\xC1\x03""int",
    (string)"\xE1\xC1\x04""uint",
    (string)"\xE1\xC1\x05""float",
    (string)"\xE1\xC1\x03""ptr",
    (string)"\xE1\xC1\x04""suid",
    (string)"\xE1\xC1\x06""object",
};

uint8 _fmtTypeIdMask[FMT_count][2] = {
    { 0xe0, 0xff },     // string
    { 0x10, 0xf0 },     // int
    { 0x20, 0xf0 },     // uint
    { 0x30, 0xf0 },     // float
    { 0x40, 0xf0 },     // ptr
    { 0xe3, 0xff },     // suid
    { 0xe1, 0xff },     // object
};

bool(*_fmtTypeParseOpt[FMT_count])(FMTVar *v, strref opt) = {
    _fmtParseStringOpt,
    _fmtParseIntOpt,
    _fmtParseIntOpt,
    _fmtParseFloatOpt,
    _fmtParsePtrOpt,
    0,          // suid
    _fmtParseObjectOpt,
};

bool(*_fmtTypeParseFinalize[FMT_count])(FMTVar *v) = {
    0,          // string
    _fmtParseIntFinalize,
    _fmtParseIntFinalize,
    _fmtParseFloatFinalize,
    _fmtParsePtrFinalize,
    0,          // suid
    0,          // object
};

bool(*_fmtTypeFormat[FMT_count])(FMTVar *v, string *out) = {
    _fmtString,
    _fmtInt,
    _fmtInt,
    _fmtFloat,
    _fmtPtr,
    _fmtSUID,
    _fmtObject,
};

static void fmtVarReset(FMTVar *v)
{
    v->vtype = -1;
    v->idx = -1;
    v->width = -1;
    v->arrayidx = -1;
    v->flags = 0;
    v->data = 0;
    v->type = 0;
    memset(&v->fmtdata, 0, sizeof(v->fmtdata));
    saClear(&v->fmtopts);
    strClear(&v->var);
    strClear(&v->def);
    strClear(&v->hashkey);
}

static void fmtVarCreate(FMTVar *v)
{
}

static void fmtVarDestroy(FMTVar *v)
{
    saDestroy(&v->fmtopts);
    strDestroy(&v->tmp);
    strDestroy(&v->var);
    strDestroy(&v->def);
    strDestroy(&v->hashkey);
}

_Use_decl_annotations_
bool _strFormat(string *out, strref fmt, int n, stvar *args)
{
    if (!(out && fmt))
        return false;

    // pre-allocate space in output string
    // this is a ROUGH GUESS and string growth may still be necessary
    uint32 fmtlen = strLen(fmt);
    _strReset(out, fmtlen + (fmtlen >> 3));

    FMTContext ctx = { 0 };
    bool ret = false;
    ctx.flen = strLen(fmt);
    ctx.nargs = n;
    ctx.args = args;
    ctx.dest = out;
    fmtVarCreate(&ctx.v);

    // Crazy optimizations -- in performance profiling, strFormat spends a lot of time in
    // cStrLen. This is because most format strings are literals that don't have an
    // embedded length, so strFind ends up counting the characters every time it needs
    // to check the length.
    if (fmtlen < 64) {
        // for small ones we just copy it to the stack
        strTemp(&ctx.fmt, fmtlen);
        strDup(&ctx.fmt, fmt);
    } else {
        // For longer strings, we create a fake rope structure on the stack that wraps the
        // literal, so the string API knows the length up front. Kids don't try this at home!

        uint8 newhdr = STR_CX | STR_ALLOC | STR_STACK | STR_ROPE | STR_LEN32;
        ctx.fmt = (string)stackAlloc(_strOffStr(newhdr) + sizeof(str_ropedata));
        *(uint8 *)ctx.fmt = newhdr;
        ((uint8 *)ctx.fmt)[1] = 0xc1;        // magic string header
        _strSetLen(ctx.fmt, fmtlen);
        _strInitRef(ctx.fmt);
        str_ropedata *ropedata = _strRopeData(ctx.fmt);
        ropedata->depth = 0;
        ropedata->left.str = (string)fmt;
        ropedata->left.len = fmtlen;
        ropedata->left.off = 0;
        ropedata->right.str = NULL;
        ropedata->right.len = 0;
        ropedata->right.off = 0;
    }

    // For performance, strFormat tries very hard to avoid allocating a bunch of strings
    // on the heap. These temporary strings live in the stack of the strFormat() function
    // and passed down to the various parsing and formatting functions through the
    // context structure. They are reused for each variable that is parsed.
    strTemp(&ctx.v.var, 64);
    strTemp(&ctx.v.def, 64);
    strTemp(&ctx.v.hashkey, 32);
    strTemp(&ctx.v.tmp, 128);
    strTemp(&ctx.tmp, 128);

    // main format string scan loop
    for (;;) {
        fmtVarReset(&ctx.v);
        if (!_fmtExtractVar(&ctx))
            goto out;
        if (strEmpty(ctx.v.var))
            break;

        if (!_fmtParseVar(&ctx))
            goto out;

        if (!_fmtFindData(&ctx))
            goto out;

        // everything checks out, so do it!
        _fmtFormat(&ctx);
    }

    // add any text after the last variable
    if (ctx.vend < ctx.flen) {
        strSubStr(&ctx.v.tmp, fmt, ctx.vend, strEnd);
        strAppend(ctx.dest, ctx.v.tmp);
    }

    ret = true;

out:
    if (!ret) {
        strClear(ctx.dest);
    }
    fmtVarDestroy(&ctx.v);
    strDestroy(&ctx.tmp);
    return ret;
}
