#include "settings_private.h"
#include <cx/fs/path.h>
#include <cx/fs/vfs.h>
#include <cx/serialize/jsonout.h>
#include <cx/serialize/sbfile.h>
#include <cx/ssdtree/ssdnodes.h>
#include <cx/ssdtree/ssdtree.h>
#include <cx/string.h>

// Customized version of jsonouttree that ignores bound settings which have a default value

static void outVal(JSONOut* jo, stvar val, SSDLockState* lstate, bool* error);

static void outHashtable(JSONOut* jo, SSDNode* node, SSDLockState* lstate, bool* error)
{
    JSONParseEvent ev = { .etype = JSON_Object_Begin };
    if (!jsonOut(jo, &ev)) {
        *error = true;
        return;
    }

    SettingsHashNode* shn = objDynCast(SettingsHashNode, node);

    SSDIterator* iter;
    htelem elem;
    for (iter = ssdnode_iterLocked(node, lstate); !*error && ssditeratorValid(iter);
         ssditeratorNext(iter)) {
        strref name = ssditeratorName(iter);
        stvar* val  = ssditeratorPtr(iter);

        if (shn && (elem = htFind(shn->binds, strref, name, none, NULL))) {
            SettingsBind* bind = hteValPtr(shn->binds, opaque, elem);
            // if this is a bound variable and it hasn't changed from the default, skip it
            if (!bind->userset && bind->type == val->type &&
                _stCmp(bind->type, NULL, bind->def, val->data, 0) == 0)
                continue;
        }

        // otherwise write to the JSON as usual
        ev = (JSONParseEvent) { .etype = JSON_Object_Key };
        strDup(&ev.edata.strData, name);
        if (!jsonOut(jo, &ev))
            *error = true;
        strDestroy(&ev.edata.strData);

        if (val)
            outVal(jo, *val, lstate, error);
        else
            *error = true;
    }
    objRelease(&iter);

    ev = (JSONParseEvent) { .etype = JSON_Object_End };
    if (!jsonOut(jo, &ev))
        *error = true;
}

static void outArray(JSONOut* jo, SSDNode* node, SSDLockState* lstate, bool* error)
{
    JSONParseEvent ev = { .etype = JSON_Array_Begin };
    if (!jsonOut(jo, &ev)) {
        *error = true;
        return;
    }

    SSDIterator* iter;
    for (iter = ssdnode_iterLocked(node, lstate); !*error && ssditeratorValid(iter);
         ssditeratorNext(iter)) {
        stvar* val = ssditeratorPtr(iter);
        if (val)
            outVal(jo, *val, lstate, error);
        else
            *error = true;
    }
    objRelease(&iter);

    ev = (JSONParseEvent) { .etype = JSON_Array_End };
    if (!jsonOut(jo, &ev))
        *error = true;
}

static void outObject(JSONOut* jo, ObjInst* val, SSDLockState* lstate, bool* error)
{
    SSDNode* node = objDynCast(SSDNode, val);
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
        stvar* subval = ssdnodePtr(node, 0, NULL, lstate);
        if (subval)
            outVal(jo, *subval, lstate, error);
    }
}

static void outInt(JSONOut* jo, int64 val, bool* error)
{
    JSONParseEvent ev = { .etype = JSON_Int };
    ev.edata.intData  = val;
    if (!jsonOut(jo, &ev))
        *error = true;
}

static void outFloat(JSONOut* jo, float64 val, bool* error)
{
    JSONParseEvent ev  = { .etype = JSON_Float };
    ev.edata.floatData = val;
    if (!jsonOut(jo, &ev))
        *error = true;
}

static void outString(JSONOut* jo, string val, bool* error)
{
    JSONParseEvent ev = { .etype = JSON_String };
    ev.edata.strData  = val;
    if (!jsonOut(jo, &ev))
        *error = true;
}

static void outBool(JSONOut* jo, bool val, bool* error)
{
    JSONParseEvent ev = { .etype = val ? JSON_True : JSON_False };
    if (!jsonOut(jo, &ev))
        *error = true;
}

static void outNull(JSONOut* jo, bool* error)
{
    JSONParseEvent ev = { .etype = JSON_Null };
    if (!jsonOut(jo, &ev))
        *error = true;
}

static void outVal(JSONOut* jo, stvar val, SSDLockState* lstate, bool* error)
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

bool _setsWriteTree(SSDNode* root, SettingsTree* tree, SSDLockState* lstate)
{
    devAssert(!strEmpty(tree->filename));

    string pathname = 0, tempname = 0;
    StreamBuffer* sb = NULL;
    VFSFile* file    = NULL;
    bool ret         = false;

    // make sure path exists
    pathParent(&pathname, tree->filename);
    if (!vfsCreateAll(tree->vfs, pathname))
        goto out;

    // write to temp file first
    pathAddExt(&tempname, tree->filename, _S"temp");
    file = vfsOpen(tree->vfs, tempname, FS_Overwrite);
    if (!file)
        goto out;

    sb = sbufCreate(1024);
    if (!sbufFileCRegisterPush(sb, file, false))
        goto out;

    JSONOut* jo = jsonOutBegin(sb, JSON_Pretty);
    if (!jo)
        goto out;

    bool error = false;
    outVal(jo, stvar(object, root), lstate, &error);
    jsonOutEnd(&jo);

    if (!error) {
        vfsClose(file);
        file = NULL;

        // only if successful, rename temp file into place
        vfsDelete(tree->vfs, tree->filename);
        ret = vfsRename(tree->vfs, tempname, tree->filename);
    }

out:
    sbufRelease(&sb);
    vfsClose(file);
    if (!ret)
        vfsDelete(tree->vfs, tempname);
    strDestroy(&pathname);
    strDestroy(&tempname);

    return ret;
}
