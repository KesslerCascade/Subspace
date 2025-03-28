#include "string_private.h"
#include "strtest.h"

#define MAX_REBALANCE_ITER 3            // really should only take 2

static void _strInitRope(_Inout_ strhandle o)
{
    strDestroy(o);

    // ropes are all STR_LEN32 to get the full header
    uint8 newhdr = STR_CX | STR_ALLOC | STR_ROPE | STR_LEN32;

    uint32 sz = _strOffStr(newhdr) + sizeof(str_ropedata);
    _Analysis_assume_(sz >= sizeof(str_ropedata));

    string ret = xaAlloc(sz);

    *(uint8*)ret = newhdr;
    ((uint8*)ret)[1] = 0xc1;        // magic string header
    _strBuffer(ret)[0] = 0;

    _strSetLen(ret, 0);
    _strInitRef(ret);

    *o = ret;
}

static void _strMkOptimalRoperef(_Inout_ str_roperef *out, _In_ strref s, uint32 off, uint32 len)
{
    // create a roperef, but if the input string is already a rope, try to see if we
    // can steal one of its source strings rather than referencing the rope node

    if (_strHdr(s) & STR_ROPE) {
        str_ropedata *data = _strRopeData(s);
        if (off + len <= data->left.len) {
            // can get this entirely from the left node, recurse as far down as possible
            _strMkOptimalRoperef(out, data->left.str, data->left.off + off, len);
            return;
        }
        if (data->right.str &&
            off >= data->left.len &&
            off - data->left.len + len <= data->right.len) {
            // can get this entirely from the right node, recurse as far down as possible
            _strMkOptimalRoperef(out, data->right.str, data->right.off + off - data->left.len, len);
            return;
        }
    }

    // nope, just create a ref to the input string
    strDup(&out->str, s);
    out->off = off;
    out->len = len;
}

static int _strRopeDepth(_In_opt_ strref s)
{
    if (!s || !(_strHdr(s) & STR_ROPE))
        return 0;
    str_ropedata *data = _strRopeData(s);

    return data->depth;
}

static void _strRotateLeft(_In_ strhandle_v top)
{
    string_v oldtop = *top;
    string_v newtop;
    str_ropedata *tdata = _strRopeData(oldtop);
    if (!(_strHdr(tdata->right.str) & STR_ROPE))
        return;                                 // nothing on the right side to rotate!

    // cdata is the right child, but ends up being moved to left child
    str_ropedata *cdata = _strRopeData(tdata->right.str);

    if (cdata->right.str) {
        // create a rope from our left node followed by the child's left node
        devAssert(tdata->left.str && cdata->left.str);
        string_v newleft = _strCreateRope((strref_v)tdata->left.str, tdata->left.off, tdata->left.len,
                                 (strref_v)cdata->left.str, cdata->left.off + tdata->right.off,
                                 min(cdata->left.len - tdata->right.off, tdata->right.len), true);

        // now create what will be the new root:
        // the new left node we just created followed by the child's right node
        newtop = _strCreateRope(newleft, 0, 0,
                                cdata->right.str, cdata->right.off,
                                min(tdata->right.len + tdata->right.off - cdata->left.len,
                                    cdata->right.len), false);
        strDestroy(&newleft);        // newtop grabbed a ref
    } else {
        // child only has a left tree somehow, this is a degenerate tree so just move the left node up
        // and delete the child
        devAssert(tdata->left.str && cdata->left.str);
        newtop = _strCreateRope((strref_v)tdata->left.str, tdata->left.off, tdata->left.len,
                                (strref_v)cdata->left.str, cdata->left.off + tdata->right.off,
                                min(cdata->left.len - tdata->right.off, tdata->right.len), false);
    }

    strDestroy(top);
    devAssert(newtop);
    *top = newtop;
}

static void _strRotateRight(_In_ strhandle_v top)
{
    string_v oldtop = *top;
    string_v newtop;
    string newright = NULL;
    str_ropedata *tdata = _strRopeData(oldtop);
    if (!(_strHdr(tdata->left.str) & STR_ROPE))
        return;                                 // nothing on the left side to rotate!

    // cdata is the left child, but ends up being moved to right child
    str_ropedata *cdata = _strRopeData(tdata->left.str);

    if (cdata->right.str) {
        // create a rope from the child's right node followed by our original right node
        newright = _strCreateRope(cdata->right.str, cdata->right.off,
                                  min(tdata->left.len + tdata->left.off - cdata->left.len,
                                      cdata->right.len),
                                  tdata->right.str, tdata->right.off, tdata->right.len, true);
        // now create what will be the new root:
        // the child's left node followed by the new right node we just created
        devAssert(cdata->left.str);
        newtop = _strCreateRope((strref_v)cdata->left.str, cdata->left.off + tdata->left.off,
                                min(cdata->left.len - tdata->left.off, tdata->left.len),
                                newright, 0, 0, false);
        strDestroy(&newright);       // newtop grabbed a ref
    } else {
        // child only has a left tree somehow, this is a degenerate tree so just move the left node up
        // and delete the child
        devAssert(cdata->left.str);
        newtop = _strCreateRope((strref_v)cdata->left.str, cdata->left.off + tdata->left.off,
                                min(cdata->left.len - tdata->left.off, tdata->left.len),
                                tdata->right.str, tdata->right.off, tdata->right.len, false);
    }

    strDestroy(top);
    devAssert(newtop);
    *top = newtop;
}

static void _strRebalanceRope(_In_ strhandle_v ptop)
{
    int i, bal, lastbal = 0;

    for (i = 0; i < MAX_REBALANCE_ITER; i++) {
        str_ropedata *tdata = _strRopeData(*ptop);
        bal = _strRopeDepth(tdata->right.str) - _strRopeDepth(tdata->left.str);

        if (bal == lastbal || bal == -lastbal)
            return;         // not making progress

        if (bal < -1)       // left heavy
            _strRotateRight(ptop);
        else if (bal > 1)   // right heavy
            _strRotateLeft(ptop);
        else
            return;         // balanced!

        lastbal = bal;
    }

    // TODO: warn / assert?
}

// create a new rope node
_Use_decl_annotations_
string_v _strCreateRope(strref_v left, uint32 left_off, uint32 left_len, strref right, uint32 right_off, uint32 right_len, bool balance)
{
    string ret = 0;
    uint8 encoding = STR_ENCODING_MASK;

    devAssert(left);

    // should this even be a rope?
    // nodes less than ROPE_MIN_SIZE can be created by leftovers on a join
    // but should get merged together during rebalancing
    left_len = left_len ? min(left_len, _strFastLen(left) - left_off) : _strFastLen(left) - left_off;
    if (right) {
        right_len = right_len ? min(right_len, _strFastLen(right) - right_off) : _strFastLen(right) - right_off;
        if (left_len + right_len < ROPE_MIN_SIZE * 2) {
            string ltemp = 0, rtemp = 0;
            if (left_off > 0 || left_len < _strFastLen(left))
                strSubStr(&ltemp, left, left_off, left_off + left_len);
            if (right_off > 0 || right_len < _strFastLen(right))
                strSubStr(&rtemp, right, right_off, right_off + right_len);

            _strConcatNoRope(&ret, ltemp ? ltemp : left, rtemp ? rtemp : right);
            strDestroy(&ltemp);
            strDestroy(&rtemp);
            return ret;
        }
    }

    _strInitRope(&ret);
    str_ropedata *data = _strRopeData(ret);
    memset(data, 0, sizeof(str_ropedata));

    _strMkOptimalRoperef(&data->left, left, left_off, left_len);
    encoding &= _strHdr(left) & STR_ENCODING_MASK;

    if (right) {
        _strMkOptimalRoperef(&data->right, right, right_off, right_len);
        encoding &= _strHdr(right) & STR_ENCODING_MASK;
    }

    data->depth = max(_strRopeDepth(data->left.str) + 1, _strRopeDepth(data->right.str) + 1);
    _strSetLen(ret, data->left.len + data->right.len);
    *_strHdrP(ret) |= encoding;

    // make sure we return balanced ropes
    if (balance)
        _strRebalanceRope(&ret);

    return ret;
}

// create a rope node with only half a rope -- this is mostly used for making substrings
_Use_decl_annotations_
string_v _strCreateRope1(strref_v s, uint32 off, uint32 len)
{
    string ret = 0;

    devAssert(s);

    // if left is a plain old string, don't do anything differently than normal
    if (!(_strHdr(s) & STR_ROPE))
        return _strCreateRope(s, off, len, NULL, 0, 0, false);

    // we're making a substring of an existing rope, here's where things get interesting
    str_ropedata *sdata = _strRopeData(s);

    _strInitRope(&ret);
    str_ropedata *data = _strRopeData(ret);
    memset(data, 0, sizeof(str_ropedata));

    if (off + len <= sdata->left.len ||
        off >= sdata->left.len ||
        !sdata->right.str) {
        // can get this entirely from left or right node of s
        _strMkOptimalRoperef(&data->left, s, off, len);
        data->depth = _strRopeDepth(data->left.str) + 1;
    } else {
        // requested segment spans both nodes, so clip them down as needed
        _strMkOptimalRoperef(&data->left, sdata->left.str, sdata->left.off + off,
                             sdata->left.len - off);
        _strMkOptimalRoperef(&data->right, sdata->right.str, sdata->right.off,
                             off + len - sdata->left.len);
        data->depth = max(_strRopeDepth(data->left.str) + 1, _strRopeDepth(data->right.str) + 1);
    }

    _strSetLen(ret, data->left.len + data->right.len);
    *_strHdrP(ret) |= _strHdr(s) & STR_ENCODING_MASK;

    return ret;
}

// quick and dirty rope node cloning for makeunique
_Use_decl_annotations_
string_v _strCloneRope(strref_v s)
{
    string ret = 0;

    _strInitRope(&ret);

    str_ropedata *sdata = _strRopeData(s);
    str_ropedata *data = _strRopeData(ret);
    memset(data, 0, sizeof(str_ropedata));

    data->depth = sdata->depth;
    strDup(&data->left.str, sdata->left.str);
    data->left.off = sdata->left.off;
    data->left.len = sdata->left.len;
    strDup(&data->right.str, sdata->right.str);
    data->right.off = sdata->right.off;
    data->right.len = sdata->right.len;
    _strSetLen(ret, data->left.len + data->right.len);

    return ret;
}

_Use_decl_annotations_
void _strDestroyRope(string_v s)
{
    str_ropedata *data = _strRopeData(s);

    strDestroy(&data->left.str);
    strDestroy(&data->right.str);
}

// copy bytes out of a rope
// as in _strFastCopy, we assume the caller has done the math
// and knows what they're doing
_Use_decl_annotations_
uint32 _strRopeFastCopy(strref_v s, uint32 off, uint8 * _Nonnull buf, uint32 bytes)
{
    uint32 leftcopy = 0, rightcopy = 0;
    str_ropedata *data = _strRopeData(s);

    if (off < data->left.len)
        leftcopy = _strFastCopy(data->left.str, off + data->left.off, buf, min(bytes, data->left.len - off));
    if (bytes == leftcopy)
        return leftcopy;            // all done from left side only!

    rightcopy = _strFastCopy(data->right.str, off + data->right.off + leftcopy - data->left.len,
                                buf + leftcopy, bytes - leftcopy);

    return leftcopy + rightcopy;    // should equal bytes
}

_Success_(return)
static bool _strRopeRealStrPart(_Inout_ str_roperef *_Nonnull ref, uint32 off, _Out_ string *_Nonnull rs, _Out_ uint32 *_Nonnull rsoff, _Out_ uint32 *_Nonnull rslen, _Out_ uint32 *_Nonnull rsstart, bool writable)
{
    // is it part of our ref?
    if (off >= ref->len)
        return false;

    // if ref string is a rope, recurse into it
    if (_strHdr(ref->str) & STR_ROPE)
        return _strRopeRealStr(&ref->str, ref->off + off, rs, rsoff, rslen, rsstart, writable);

    if (writable)
        _strMakeUnique(&ref->str, 0);

    // basic string, let's do it!
    *rs = ref->str;
    *rsoff = ref->off + off;
    *rslen = ref->len - off;
    *rsstart = ref->off;
    return true;
}

// get the actual string and offset of a particular offset within a rope.
// note that rs gets a borrowed reference put into it, so caller must dup
// if it wants to hold on to it and doesn't otherwise hold a ref!
_Success_(return)
bool _strRopeRealStr(_Inout_ strhandle_v s, uint32 off, _Out_ string *_Nonnull rs, _Out_ uint32 *_Nonnull rsoff, _Out_ uint32 *_Nonnull rslen, _Out_ uint32 *_Nonnull rsstart, bool writable)
{
    if (!(_strHdr(*s) & STR_ROPE))
        return false;

    if (writable)
        _strMakeUnique(s, 0);

    str_ropedata *data = _strRopeData(*s);
    if (_strRopeRealStrPart(&data->left, off, rs, rsoff, rslen, rsstart, writable))
        return true;
    if (_strRopeRealStrPart(&data->right, off - data->left.len, rs, rsoff, rslen, rsstart, writable))
        return true;

    return false;
}

_Use_decl_annotations_
int strTestRopeDepth(strref s)
{
    return _strRopeDepth(s);
}

_Use_decl_annotations_
bool strTestRopeNode(strhandle o, strref s, bool left)
{
    if (!s || !(_strHdr(s) & STR_ROPE))
        return false;
    str_ropedata *data = _strRopeData(s);

    if (left && data->left.str) {
        strDup(o, data->left.str);
        return true;
    }
    if (!left && data->right.str) {
        strDup(o, data->right.str);
        return true;
    }
    return false;
}