#include <stdio.h>
#include <cx/ssdtree.h>
#include <cx/string.h>
#include <cx/string/strtest.h>
#include <cx/container/foreach.h>

#include <cx/ssdtree/node/ssdarraynode.h>
#include <cx/ssdtree/node/ssdhashnode.h>

#define TEST_FILE ssdtest
#define TEST_FUNCS ssdtest_funcs
#include "common.h"

static int test_ssd_tree()
{
    int ret = 0;
    SSDNode *tree = ssdCreateHashtable();

    // basic test
    stvar outvar = { 0 };
    if (ssdGet(tree, _S"test", &outvar) || outvar.type != 0)
        ret = 1;

    ssdSet(tree, _S"l1/l2/l3/test1", true, stvar(int32, 1920));
    if (!ssdGet(tree, _S"l1/l2/l3/test1", &outvar) ||
        !stEq(outvar.type, stType(int32)) ||
        outvar.data.st_int32 != 1920)
        ret = 1;

    stvar *pval;
    ssdLockedTransaction(tree)
    {
        pval = ssdPtr(tree, _S"l1/l2/l3/test1");
        if (!pval ||
            !stEq(pval->type, stType(int32)) ||
            pval->data.st_int32 != 1920)
            ret = 1;
    }

    objRelease(&tree);

    // make sure copies are happening where they're supposed to
    string teststr = 0;
    strCopy(&teststr, _S"test123");
    tree = ssdCreateHashtable();

    stvar tempst = { .type = stType(string) };
    strDup(&tempst.data.st_string, teststr);
    if (strTestRefCount(teststr) != 2)
        ret = 1;

    ssdSetC(tree, _S"l1/l2/l3/test2", true, &tempst);
    if (strTestRefCount(teststr) != 2)
        ret = 1;

    // test getting an object node as a value
    if (!ssdGet(tree, _S"l1/l2/l3", &outvar) ||
        !stEq(outvar.type, stType(object)))
        ret = 1;
    stDestroy(stvar, &outvar);

    // this should get a copy of the string, increasing its ref count
    if (!ssdGet(tree, _S"l1/l2/l3/test2", &outvar) ||
        !stEq(outvar.type, stType(string)) ||
        !strEq(outvar.data.st_string, _S"test123") ||
        outvar.data.st_string != teststr ||
        strTestRefCount(teststr) != 3)
        ret = 1;

    stDestroy(stvar, &outvar);
    if (strTestRefCount(teststr) != 2)
        ret = 1;

    // this should get a pointer to the string, NOT increasing the ref count
    ssdLockedTransaction(tree)
    {
        pval = ssdPtr(tree, _S"l1/l2/l3/test2");
        if (!stEq(pval->type, stType(string)) ||
            !strEq(pval->data.st_string, _S"test123") ||
            pval->data.st_string != teststr ||
            strTestRefCount(teststr) != 2)
            ret = 1;

        // this should also get a pointer
        strref pstr = ssdStrRef(tree, _S"l1/l2/l3/test2");
        if (!pstr || !strEq(pstr, _S"test123")
            || pstr != teststr ||
            strTestRefCount(teststr) != 2)
            ret = 1;
    }
    objRelease(&tree);

    if (strTestRefCount(teststr) != 1)
        ret = 1;
    strDestroy(&teststr);

    return ret;
}

static int test_ssd_single()
{
    int ret = 0;
    SSDNode *tree = ssdCreateSingle();
    SSDLockState tstate;
    _ssdLockStateInit(&tstate);
    ssdnodeSet(tree, 0, NULL, stvar(int64, 200000), &tstate);
    _ssdLockEnd(tree, &tstate);

    stvar outvar = { 0 };
    if (!ssdGet(tree, _S"", &outvar) ||
        !stEq(outvar.type, stType(int64)) ||
        outvar.data.st_int32 != 200000)
        ret = 1;

    objRelease(&tree);

    string teststr = 0;
    strCopy(&teststr, _S"test123");
    tree = ssdCreateSingle();
    _ssdLockStateInit(&tstate);
    ssdnodeSet(tree, 0, NULL, stvar(string, teststr), &tstate);
    _ssdLockEnd(tree, &tstate);

    if (strTestRefCount(teststr) != 2)
        ret = 1;

    outvar = (stvar){0};
    if (!ssdGet(tree, _S"", &outvar) ||
        !stEq(outvar.type, stType(string)) ||
        !strEq(outvar.data.st_string, _S"test123") ||
        strTestRefCount(teststr) != 3)
        ret = 1;

    stDestroy(stvar, &outvar);

    objRelease(&tree);

    if (strTestRefCount(teststr) != 1)
        ret = 1;

    strDestroy(&teststr);

    return ret;
}

static int test_ssd_subtree()
{
    int ret = 0;
    SSDNode *tree = ssdCreateHashtable();

    stvar outvar = { 0 };
    string teststr = 0;
    string teststr2 = 0;
    strCopy(&teststr, _S"test123");

    ssdSet(tree, _S"l1/b1/l3/test1", true, stvar(int32, 10743));
    ssdSet(tree, _S"l1/b1/l3/test2", true, stvar(string, teststr));
    ssdSet(tree, _S"l1/b2/l3/test1", true, stvar(int32, 39294));
    ssdSet(tree, _S"l1/b2/l3/test2", true, stvar(string, teststr));

    SSDNode *tree2 = ssdCreateHashtable();
    ssdSet(tree2, _S"k1/aabb", true, stvar(int32, 1122));
    ssdSet(tree2, _S"k1/bbaa", true, stvar(int32, 2211));
    ssdSet(tree2, _S"k1/nums[0]", true, stvar(int32, 100));
    ssdSet(tree2, _S"k1/nums[1]", true, stvar(int32, 101));
    ssdSet(tree2, _S"k1/nums[2]", true, stvar(int32, 102));
    ssdSet(tree2, _S"k1/nums[3]", true, stvar(int32, 103));
    ssdSet(tree2, _S"k1/nums[4]", true, stvar(int32, 104));
    ssdSet(tree2, _S"k2/aabb", true, stvar(int32, 4488));
    ssdSet(tree2, _S"k2/bbaa", true, stvar(int32, 8844));
    ssdSet(tree2, _S"k2/nums[0]", true, stvar(int32, 200));
    ssdSet(tree2, _S"k2/nums[1]", true, stvar(int32, 201));
    ssdSet(tree2, _S"k2/nums[2]", true, stvar(int32, 202));
    ssdSet(tree2, _S"k2/nums[3]", true, stvar(int32, 203));
    ssdSet(tree2, _S"k2/nums[4]", true, stvar(int32, 204));
    ssdSet(tree2, _S"alt", true, stvar(string, _S"yes, alt"));
    // graft tree2 onto tree
    ssdGraft(tree, _S"grafted", tree2, 0);
    // and graft the subtrees a couple places
    ssdGraft(tree, _S"l1/b3", tree2, _S"k1");
    ssdGraft(tree, _S"l1/b4", tree2, _S"k2");
    // then destroy it
    objRelease(&tree2);

    if (strTestRefCount(teststr) != 3)
        ret = 1;

    SSDNode *subtree = ssdSubtree(tree, _S"l1/b2", false);
    if (!subtree) {
        ret = 1;
        goto out;
    }

    if (!ssdGet(subtree, _S"l3/test1", &outvar) ||
        !stEq(outvar.type, stType(int32)) ||
        outvar.data.st_int32 != 39294)
        ret = 1;

    if (!ssdGet(subtree, _S"l3/test2", &outvar) ||
        !stEq(outvar.type, stType(string)) ||
        !strEq(outvar.data.st_string, _S"test123") ||
        outvar.data.st_string != teststr ||
        strTestRefCount(teststr) != 4)
        ret = 1;

    stDestroy(stvar, &outvar);
    if (strTestRefCount(teststr) != 3)
        ret = 1;

    // test some various type conversions
    if (ssdVal(float64, tree, _S"l1/b1/l3/test1", 1829.5) != 10743)
        ret = 1;
    if (ssdVal(float64, tree, _S"l1/b1/l3/DOESNOTEXIST", 1829.5) != 1829.5)
        ret = 1;
    if (ssdVal(int64, tree, _S"l1/b1/l3/test1", 43434) != 10743)
        ret = 1;
    if (ssdVal(uint16, tree, _S"l1/b1/l3/test1", 332) != 10743)
        ret = 1;

    ssdStringOutD(tree, _S"l1/b1/l3/test1", &teststr2, _S"Default String");
    if (!strEq(teststr2, _S"10743"))
        ret = 1;
    ssdStringOutD(tree, _S"l1/b1/l3/DOESNOTEXIST", &teststr2, _S"Default String");
    if (!strEq(teststr2, _S"Default String"))
        ret = 1;

    // borrowed subtree should NOT increase refcount
    uintptr oldref = atomicLoad(uintptr, &subtree->_ref, Acquire);

    ssdLockedTransaction(tree)
    {
        SSDNode *btree = ssdSubtreeB(tree, _S"l1/b2");

        if (btree != subtree ||
            atomicLoad(uintptr, &btree->_ref, Acquire) != oldref)
            ret = 1;
        btree = NULL;

        // try object instance casting
        // this is effectively the same thing as ssdSubtreeB
        btree = ssdObjPtr(tree, _S"l1/b2", SSDNode);
        if (btree != subtree ||
            atomicLoad(uintptr, &btree->_ref, Acquire) != oldref)
            ret = 1;

        btree = ssdSubtreeB(tree, _S"l1/b2/l3");

        // try the iterator
        int icount = 0;
        foreach(ssd, oiter, idx, name, val, btree)
        {
            // this also checks insertion order retention
            if (icount == 0 &&
                !(strEq(name, _S"test1") &&
                  stvarIs(val, int32) && val->data.st_int32 == 39294))
                ret = 1;

            if (icount == 1 &&
                !(strEq(name, _S"test2") &&
                  strEq(stvarString(val), teststr)))
                ret = 1;
            icount++;
        }
        if (icount != 2)
            ret = 1;
    }

    // check for the grafted values
    if (ssdVal(int32, tree, _S"grafted/k1/aabb", -1) != 1122 ||
        ssdVal(int32, tree, _S"grafted/k1/bbaa", -1) != 2211 ||
        ssdVal(int32, tree, _S"grafted/k1/nums[1]", -1) != 101 ||
        ssdVal(int32, tree, _S"grafted/k2/aabb", -1) != 4488 ||
        ssdVal(int32, tree, _S"grafted/k2/bbaa", -1) != 8844 ||
        ssdVal(int32, tree, _S"grafted/k2/nums[3]", -1) != 203)
        ret = 1;
    if (!ssdGet(tree, _S"grafted/alt", &outvar) ||
        !strEq(stvarString(&outvar), _S"yes, alt"))
        ret = 1;
    stvarDestroy(&outvar);

    if (ssdVal(int32, tree, _S"l1/b3/aabb", -1) != 1122 ||
        ssdVal(int32, tree, _S"l1/b3/bbaa", -1) != 2211 ||
        ssdVal(int32, tree, _S"l1/b3/nums[0]", -1) != 100 ||
        ssdVal(int32, tree, _S"l1/b4/aabb", -1) != 4488 ||
        ssdVal(int32, tree, _S"l1/b4/bbaa", -1) != 8844 ||
        ssdVal(int32, tree, _S"l1/b4/nums[4]", -1) != 204)
        ret = 1;

    // releasing the main tree shouldn't affect the subtree
    objRelease(&tree);

    if (!ssdGet(subtree, _S"l3/test1", &outvar) ||
        !stEq(outvar.type, stType(int32)) ||
        outvar.data.st_int32 != 39294)
        ret = 1;

    // but it should drop the refcount of the string when the other branch was destroyed
    if (!ssdGet(subtree, _S"l3/test2", &outvar) ||
        !stEq(outvar.type, stType(string)) ||
        !strEq(outvar.data.st_string, _S"test123") ||
        outvar.data.st_string != teststr ||
        strTestRefCount(teststr) != 3)
        ret = 1;

    stDestroy(stvar, &outvar);
    if (strTestRefCount(teststr) != 2)
        ret = 1;

out:
    objRelease(&subtree);
    objRelease(&tree);

    if (strTestRefCount(teststr) != 1)
        ret = 1;
    strDestroy(&teststr);
    strDestroy(&teststr2);

    return ret;
}

static int test_ssd_array()
{
    int ret = 0;
    SSDNode *tree = ssdCreateHashtable();

    SSDLockState tlock;
    _ssdLockStateInit(&tlock);
    SSDNode *sub = ssdSubtree(tree, _S"test/arr", SSD_Create_Array);

    SSDNode *h1 = ssdtreeCreateNode(sub->tree, SSD_Create_Hashtable);
    ssdnodeSet(h1, SSD_ByName, _S"test1", stvar(int32, 1), &tlock);
    ssdnodeSet(sub, 0, NULL, stvar(object, h1), &tlock);
    objRelease(&h1);

    ssdnodeSet(sub, 1, NULL, stvar(int64, 128), &tlock);
    ssdnodeSet(sub, 2, NULL, stvar(float64, 5), &tlock);
    h1 = ssdtreeCreateNode(sub->tree, SSD_Create_Hashtable);
    ssdnodeSet(h1, SSD_ByName, _S"test2", stvar(strref, _S"it's a test"), &tlock);
    ssdnodeSet(sub, 3, NULL, stvar(object, h1), &tlock);
    objRelease(&h1);
    SSDNode *a1 = ssdtreeCreateNode(sub->tree, SSD_Create_Array);
    ssdnodeSet(a1, 0, NULL, stvar(int32, 101), &tlock);
    ssdnodeSet(a1, 1, NULL, stvar(int32, 102), &tlock);
    ssdnodeSet(a1, 2, NULL, stvar(int32, 103), &tlock);
    ssdnodeSet(sub, 4, NULL, stvar(object, a1), &tlock);
    objRelease(&a1);
    _ssdLockEnd(tree, &tlock);

    ssdLockedTransaction(tree)
    {
        stvar *out;
        out = ssdPtr(tree, _S"test/arr");
        if (!out || !stEq(out->type, stType(object)) || !objDynCast(SSDNode, out->data.st_object) ||
            !(ssdnodeIsArray(objDynCast(SSDNode, out->data.st_object))))
            ret = 1;

        out = ssdPtr(tree, _S"test/arr[0]/test1");
        if (!out || !stEq(out->type, stType(int32)) || out->data.st_int32 != 1)
            ret = 1;

        out = ssdPtr(tree, _S"test/arr[0]/test2");
        if (out)
            ret = 1;

        out = ssdPtr(tree, _S"test/arr[1]");
        if (!out || !stEq(out->type, stType(int64)) || out->data.st_int64 != 128)
            ret = 1;

        out = ssdPtr(tree, _S"test/arr[1]/test");
        if (out)
            ret = 1;

        out = ssdPtr(tree, _S"test/arr[2]");
        if (!out || !stEq(out->type, stType(float64)) || out->data.st_float64 != 5)
            ret = 1;

        out = ssdPtr(tree, _S"test/arr[2]/test");
        if (out)
            ret = 1;

        out = ssdPtr(tree, _S"test/arr[3]/test2");
        if (!out || !stEq(out->type, stType(string)) || !strEq(out->data.st_string, _S"it's a test"))
            ret = 1;

        out = ssdPtr(tree, _S"test/arr[3]/test1");
        if (out)
            ret = 1;

        out = ssdPtr(tree, _S"test/arr[4]/test1");
        if (out)
            ret = 1;

        out = ssdPtr(tree, _S"test/arr[4][0]");
        if (!out || !stEq(out->type, stType(int32)) || out->data.st_int32 != 101)
            ret = 1;

        out = ssdPtr(tree, _S"test/arr[4][1]");
        if (!out || !stEq(out->type, stType(int32)) || out->data.st_int32 != 102)
            ret = 1;

        out = ssdPtr(tree, _S"test/arr[4][2]");
        if (!out || !stEq(out->type, stType(int32)) || out->data.st_int32 != 103)
            ret = 1;

        sa_stvar arr1 = saInitNone;
        if (!ssdExportArray(tree, _S"test/arr", &arr1) ||
            saSize(arr1) != 5 ||
            !stvarIs(&arr1.a[2], float64) ||
            arr1.a[2].data.st_float64 != 5)
            ret = 1;

        // paste the array back in as a child of itself
        ssdImportArray(tree, _S"test/arr[5]", arr1);

        out = ssdPtr(tree, _S"test/arr[5][3]/test2");
        if (!out || !stEq(out->type, stType(string)) || !strEq(out->data.st_string, _S"it's a test"))
            ret = 1;

        saDestroy(&arr1);

        sa_int32 arr2 = saInitNone;
        if (!ssdExportTypedArray(tree, _S"test/arr[4]", int32, &arr2, true) ||
            saSize(arr2) != 3 ||
            arr2.a[0] != 101 ||
            arr2.a[1] != 102 ||
            arr2.a[2] != 103)
            ret = 1;

        ssdImportTypedArray(tree, _S"test/another", int32, arr2);
        saDestroy(&arr2);

        if (ssdVal(int32, tree, _S"test/another[1]", -1) != 102)
            ret = 1;

        // should fail with strict == true
        if (ssdExportTypedArray(tree, _S"test/arr", int32, &arr2, true) ||
            saSize(arr2) != 0)
            ret = 1;

        // should filter out everything except the one int64
        sa_int64 arr3 = saInitNone;
        if (!ssdExportTypedArray(tree, _S"test/arr", int64, &arr3, false) ||
            saSize(arr3) != 1 ||
            arr3.a[0] != 128)
            ret = 1;
        saDestroy(&arr3);
    }

    objRelease(&sub);
    objRelease(&tree);

    return ret;
}

testfunc ssdtest_funcs[] = {
    { "tree", test_ssd_tree },
    { "single", test_ssd_single },
    { "subtree", test_ssd_subtree },
    { "array", test_ssd_array },
    { 0, 0 }
};
