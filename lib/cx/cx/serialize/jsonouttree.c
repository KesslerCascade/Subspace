#include "jsonout.h"
#include "sbstring.h"

#include <cx/ssdtree/ssdnodes.h>
#include <cx/ssdtree/ssdtree.h>
#include <cx/string.h>

static void outVal(_Inout_ JSONOut *jo, stvar val, _Inout_ SSDLockState *lstate, _Inout_ bool *error);

static void outHashtable(_Inout_ JSONOut *jo, _In_ SSDNode *node, _Inout_ SSDLockState *lstate, _Inout_ bool *error)
{
    JSONParseEvent ev = { .etype = JSON_Object_Begin };
    if (!jsonOut(jo, &ev)) {
        *error = true;
        return;
    }

    SSDIterator *iter;
    for (iter = ssdnode_iterLocked(node, lstate); !*error && ssditeratorValid(iter); ssditeratorNext(iter)) {
        ev = (JSONParseEvent){ .etype = JSON_Object_Key };
        strDup(&ev.edata.strData, ssditeratorName(iter));
        if (!jsonOut(jo, &ev))
            *error = true;
        strDestroy(&ev.edata.strData);

        stvar *val = ssditeratorPtr(iter);
        if (val)
            outVal(jo, *val, lstate, error);
        else
            *error = true;
    }
    objRelease(&iter);

    ev = (JSONParseEvent){ .etype = JSON_Object_End };
    if (!jsonOut(jo, &ev))
        *error = true;
}

static void outArray(_Inout_ JSONOut *jo, _In_ SSDNode *node, _Inout_ SSDLockState *lstate, _Inout_ bool *error)
{
    JSONParseEvent ev = { .etype = JSON_Array_Begin };
    if (!jsonOut(jo, &ev)) {
        *error = true;
        return;
    }

    SSDIterator *iter;
    for (iter = ssdnode_iterLocked(node, lstate); !*error && ssditeratorValid(iter); ssditeratorNext(iter)) {
        stvar *val = ssditeratorPtr(iter);
        if (val)
            outVal(jo, *val, lstate, error);
        else
            *error = true;
    }
    objRelease(&iter);

    ev = (JSONParseEvent){ .etype = JSON_Array_End };
    if (!jsonOut(jo, &ev))
        *error = true;
}

static void outObject(_Inout_ JSONOut *jo, _In_ ObjInst *val, _Inout_ SSDLockState *lstate, _Inout_ bool *error)
{
    SSDNode *node = objDynCast(SSDNode, val);
    if (!node) {
        *error = true;
        return;
    }

    if (ssdnodeIsHashtable(node)) {
        outHashtable(jo, node, lstate, error);
    } else if (ssdnodeIsArray(node)) {
        outArray(jo, node, lstate, error);
    } else {
        // this is a single value node
        stvar *subval = ssdnodePtr(node, 0, NULL, lstate);
        if (subval)
            outVal(jo, *subval, lstate, error);
    }
}

static void outInt(_Inout_ JSONOut *jo, int64 val, _Inout_ bool *error)
{
    JSONParseEvent ev = { .etype = JSON_Int };
    ev.edata.intData = val;
    if (!jsonOut(jo, &ev))
        *error = true;
}

static void outFloat(_Inout_ JSONOut *jo, float64 val, _Inout_ bool *error)
{
    JSONParseEvent ev = { .etype = JSON_Float };
    ev.edata.floatData = val;
    if (!jsonOut(jo, &ev))
        *error = true;
}

static void outString(_Inout_ JSONOut *jo, _In_opt_ string val, _Inout_ bool *error)
{
    JSONParseEvent ev = { .etype = JSON_String };
    ev.edata.strData = val;
    if (!jsonOut(jo, &ev))
        *error = true;
}

static void outBool(_Inout_ JSONOut *jo, bool val, _Inout_ bool *error)
{
    JSONParseEvent ev = { .etype = val ? JSON_True : JSON_False };
    if (!jsonOut(jo, &ev))
        *error = true;
}

static void outNull(_Inout_ JSONOut *jo, _Inout_ bool *error)
{
    JSONParseEvent ev = { .etype = JSON_Null };
    if (!jsonOut(jo, &ev))
        *error = true;
}

static void outVal(_Inout_ JSONOut *jo, stvar val, _Inout_ SSDLockState *lstate, _Inout_ bool *error)
{
    switch (stGetId(val.type)) {
    case stTypeId(object):
        outObject(jo, val.data.st_object, lstate, error);
        break;
    case stTypeId(int8):
        outInt(jo, val.data.st_int8, error);
        break;
    case stTypeId(int16):
        outInt(jo, val.data.st_int16, error);
        break;
    case stTypeId(int32):
        outInt(jo, val.data.st_int32, error);
        break;
    case stTypeId(int64):
        outInt(jo, val.data.st_int64, error);
        break;
    case stTypeId(uint8):
        outInt(jo, val.data.st_uint8, error);
        break;
    case stTypeId(uint16):
        outInt(jo, val.data.st_uint16, error);
        break;
    case stTypeId(uint32):
        outInt(jo, val.data.st_uint32, error);
        break;
    case stTypeId(float32):
        outFloat(jo, val.data.st_float32, error);
        break;
    case stTypeId(float64):
        outFloat(jo, val.data.st_float64, error);
        break;
    case stTypeId(string):
        outString(jo, val.data.st_string, error);
        break;
    case stTypeId(bool):
        outBool(jo, val.data.st_bool, error);
        break;
    case stTypeId(none):
        outNull(jo, error);
        break;
    default:
        *error = true;
        break;
    }
}

_Use_decl_annotations_
bool _jsonOutTree(StreamBuffer *sb, SSDNode *tree, uint32 flags, SSDLockState *_ssdCurrentLockState)
{
    bool error = false;
    JSONOut *jo = jsonOutBegin(sb, flags);
    if (!jo)
        return false;

    ssdLockedTransaction(tree)
    {
        ssdLockRead(tree);
        outVal(jo, stvar(object, tree), _ssdCurrentLockState, &error);
    }
    jsonOutEnd(&jo);

    return !error;
}

_Use_decl_annotations_
bool _jsonTreeToString(string *out, SSDNode *tree, uint32 flags, SSDLockState *_ssdCurrentLockState)
{
    if (!out || !tree)
        return false;

    StreamBuffer *sb = sbufCreate(1024);
    if (!sbufStrCRegisterPush(sb, out)) {
        sbufRelease(&sb);
        return false;
    }

    bool ret = jsonOutTree(sb, tree, flags);
    sbufRelease(&sb);

    return ret;
}
