#include "jsonparse.h"
#include "sbstring.h"

#include <cx/ssdtree/ssdnodes.h>
#include <cx/ssdtree/ssdtree.h>
#include <cx/string.h>

typedef struct JSONTreeState {
    SSDTree *tree;
    SSDLockState lstate;
    SSDNode *root;
    SSDNode *cur;
    sa_SSDNode nodestack;
    bool error;
} JSONTreeState;

static void setValDirect(_Inout_ JSONTreeState *jts, _In_ JSONParseContext *ctx, stvar val)
{
    switch (ctx->ctype) {
    case JSON_Object:
        ssdnodeSet(jts->cur, SSD_ByName, ctx->cdata.curKey, val, &jts->lstate);
        break;
    case JSON_Array:
        ssdnodeSet(jts->cur, ctx->cdata.curIdx, NULL, val, &jts->lstate);
        break;
    default:
        break;
    }
}

static void pushObject(_Inout_ JSONTreeState *jts, _In_ JSONParseContext *ctx)
{
    if (!jts->root) {
        jts->root = ssdCreateCustom(SSD_Create_Hashtable, jts->tree);
        jts->cur = objAcquire(jts->root);
    } else {
        SSDNode *node = ssdtreeCreateNode(jts->root->tree, SSD_Create_Hashtable);
        setValDirect(jts, ctx, stvar(object, node));
        saPushC(&jts->nodestack, object, &jts->cur);
        jts->cur = node;
    }
}

static void pushArray(_Inout_ JSONTreeState *jts, _In_ JSONParseContext *ctx)
{
    if (!jts->root) {
        jts->root = ssdCreateCustom(SSD_Create_Array, jts->tree);
        jts->cur = objAcquire(jts->root);
    } else {
        SSDNode *node = ssdtreeCreateNode(jts->root->tree, SSD_Create_Array);
        setValDirect(jts, ctx, stvar(object, node));
        saPushC(&jts->nodestack, object, &jts->cur);
        jts->cur = node;
    }
}

static void popNode(_Inout_ JSONTreeState *jts)
{
    objRelease(&jts->cur);
    jts->cur = saPopPtr(&jts->nodestack);
}

static void setVal(_Inout_ JSONTreeState *jts, _In_ JSONParseContext *ctx, stvar val)
{
    if (!jts->root) {
        jts->root = ssdCreateCustom(SSD_Create_Single, jts->tree);
        jts->cur = jts->root;
        ssdnodeSet(jts->root, 0, NULL, val, &jts->lstate);
    }

    devAssert(jts->cur);

    setValDirect(jts, ctx, val);
}

void jsonTreeCB(_In_ JSONParseEvent *ev, _Inout_opt_ void *userdata)
{
    if (!userdata)
        return;

    JSONTreeState *jts = (JSONTreeState *)userdata;
    switch (ev->etype) {
    case JSON_Object_Begin:
        pushObject(jts, ev->ctx);
        break;
    case JSON_Object_End:
        popNode(jts);
        break;
    case JSON_Array_Begin:
        pushArray(jts, ev->ctx);
        break;
    case JSON_Array_End:
        popNode(jts);
        break;
    case JSON_String:
        setVal(jts, ev->ctx, stvar(string, ev->edata.strData));
        break;
    case JSON_Int:
        setVal(jts, ev->ctx, stvar(int64, ev->edata.intData));
        break;
    case JSON_Float:
        setVal(jts, ev->ctx, stvar(float64, ev->edata.floatData));
        break;
    case JSON_True:
        setVal(jts, ev->ctx, stvar(bool, true));
        break;
    case JSON_False:
        setVal(jts, ev->ctx, stvar(bool, false));
        break;
    case JSON_Null:
        setVal(jts, ev->ctx, stvNone);
        break;
    case JSON_Error:
        jts->error = true;
        break;
    default:
        break;
    }
}

_Use_decl_annotations_
SSDNode *jsonParseTreeCustom(StreamBuffer *sb, SSDTree *tree)
{
    SSDNode *ret = NULL;
    JSONTreeState jts = { .tree = tree };
    saInit(&jts.nodestack, object, 8);
    _ssdLockStateInit(&jts.lstate);

    bool success = jsonParse(sb, jsonTreeCB, &jts);
    if (success && !jts.error && jts.root) {
        ret = objAcquire(jts.root);
    }
    _ssdLockEnd(jts.root, &jts.lstate);

    saDestroy(&jts.nodestack);
    objRelease(&jts.root);
    return ret;
}

_Use_decl_annotations_
SSDNode *jsonParseTree(StreamBuffer *sb)
{
    return jsonParseTreeCustom(sb, NULL);
}

_Use_decl_annotations_
SSDNode *jsonTreeFromString(strref str)
{
    StreamBuffer *sb = sbufCreate(128);
    if (!sbufStrPRegisterPull(sb, str)) {
        sbufRelease(&sb);
        return NULL;
    }

    SSDNode *ret = jsonParseTreeCustom(sb, NULL);
    sbufRelease(&sb);
    return ret;
}
