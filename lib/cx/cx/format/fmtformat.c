#include "format_private.h"
#include "cx/obj/objstdif.h"
#include "cx/utils/compare.h"
#include "formattable.h"

_Use_decl_annotations_
bool _fmtFindData(FMTContext *ctx)
{
    bool ret = !strEmpty(ctx->v.def);

    if (ctx->v.vtype == -1)
        return ret;

    bool isarray = ctx->v.arrayidx >= 0;
    bool ishash = !strEmpty(ctx->v.hashkey);
    bool usestartarg = (ctx->v.idx == -1 && !isarray && !ishash);
    int32 findinst = clamplow(ctx->v.idx, 1);
    int32 idx = usestartarg ? ctx->startarg[ctx->v.vtype] : 0;
    uint8 typeid = _fmtTypeIdMask[ctx->v.vtype][0];
    uint8 typemask = _fmtTypeIdMask[ctx->v.vtype][1];

    for (; findinst > 0; idx++) {
        stvar *arg = &ctx->args[idx];

        if (idx >= ctx->nargs) {
            ctx->v.vtype = -1;      // no data, don't try to format
            return ret;
        }

        if (isarray) {
            if (stGetId(arg->type) == stTypeId(sarray) &&
                (stGetId(saElemType(arg->data.st_sarray)) & typemask) == typeid)
                findinst--;
        } else if (ishash) {
            if (stGetId(arg->type) == stTypeId(hashtable) &&
                stGetId(htKeyType(arg->data.st_hashtable)) == stTypeId(string) &&
                (stGetId(htValType(arg->data.st_hashtable)) & typemask) == typeid)
                findinst--;
        } else if ((stGetId(arg->type) & typemask) == typeid) {
            findinst--;
        }
    }

    if (usestartarg)
        ctx->startarg[ctx->v.vtype] = idx;

    if (isarray) {
        sa_ref arr = ctx->args[idx - 1].data.st_sarray;
        if (ctx->v.arrayidx >= saSize(arr)) {
            ctx->v.vtype = -1;
            return ret;
        }

        ctx->v.type = saElemType(arr);
        ctx->v.data = (void*)((uintptr)arr.a + (size_t)saElemSize(arr)*ctx->v.arrayidx);
    } else if (ishash) {
        hashtable htbl = ctx->args[idx - 1].data.st_hashtable;
        htelem elem = htFind(htbl, string, ctx->v.hashkey, none, 0);
        if (!elem) {
            ctx->v.vtype = -1;
            return ret;
        }
        ctx->v.type = htValType(htbl);
        ctx->v.data = hteValPtr(htbl, opaque, elem);
    } else {
        ctx->v.type = ctx->args[idx - 1].type;
        ctx->v.data = stGenPtr(ctx->v.type, ctx->args[idx - 1].data);
    }

    if (typeid == stTypeId(object)) {
        // special handling for objects to ensure they have the right interface
        Formattable *fmtif = objInstIf(*(ObjInst**)ctx->v.data, Formattable);
        if (fmtif) {
            ctx->v.fmtdata[0] = (uintptr)fmtif;
        } else {
            // If an object doesn't implement Formattable, it might implement Convertible
            Convertible *cvtif = objInstIf(*(ObjInst **)ctx->v.data, Convertible);
            if (cvtif) {
                ctx->v.fmtdata[1] = (uintptr)cvtif;
            } else {
                ctx->v.vtype = -1;
                ctx->v.data = NULL;
                return ret;
            }
        }
    }
    return true;
}

static inline void fillPad(_Inout_ string *pad, int32 len)
{
    strClear(pad);
    uint8 *buf = strBuffer(pad, len);
    for (int32 i = 0; i < len; i++)
        buf[i] = ' ';
}

static void fmtApplyGenWidth(_Inout_ FMTVar *v, _Inout_ string *vstr, int32 width, uint32 flags)
{
    if (width <= 0 || strLen(*vstr) == width)
        return;

    if ((int32)strLen(*vstr) > width) {
        strSubStr(vstr, *vstr, 0, width);
        return;
    }

    int32 wdiff = width - strLen(*vstr);
    if (flags & FMTVar_Right) {
        fillPad(&v->tmp, wdiff);
        strPrepend(v->tmp, vstr);
    } else if (flags & FMTVar_Center) {
        int32 lpart = wdiff / 2;
        fillPad(&v->tmp, lpart);
        strPrepend(v->tmp, vstr);
        fillPad(&v->tmp, wdiff - lpart);
        strAppend(vstr, v->tmp);
    } else {
        // FMTVar_Left implied
        fillPad(&v->tmp, wdiff);
        strAppend(vstr, v->tmp);
    }
}

static void fmtApplyGenFlags(_Inout_ FMTContext *ctx, _Inout_ string *vstr)
{
    if (!(ctx->v.flags & FMTVar_NoGenCase)) {
        if (ctx->v.flags & FMTVar_Upper)
            strUpper(vstr);
        else if (ctx->v.flags & FMTVar_Lower)
            strLower(vstr);
    }

    if (!(ctx->v.flags & FMTVar_NoGenWidth))
        fmtApplyGenWidth(&ctx->v, vstr, ctx->v.width, ctx->v.flags);
}

_Use_decl_annotations_
void _fmtFormat(FMTContext *ctx)
{
    bool success = false;

    strClear(&ctx->tmp);
    if (ctx->v.vtype != -1 && _fmtTypeFormat[ctx->v.vtype])
        success = _fmtTypeFormat[ctx->v.vtype](&ctx->v, &ctx->tmp);

    if (!success) {
        // use the default value
        strDup(&ctx->tmp, ctx->v.def);
    }

    // apply generic formatting options
    fmtApplyGenFlags(ctx, &ctx->tmp);

    strAppend(ctx->dest, ctx->tmp);
}
