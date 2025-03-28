#include "objtestobj.h"
#include <cx/container/sarray.h>

#define TEST_FILE objtest
#define TEST_FUNCS objtest_funcs
#include "common.h"

static int test_iface()
{
    TestCls1 *cls1 = TestCls1_create();
    if (!cls1)
        return 1;

    cls1->data = 42;

    TestIf1 *ifptr = objInstIf(cls1, TestIf1);
    if (!ifptr)
        return 1;

    if (ifptr->testfunc(cls1) != 42)
        return 1;

    objRelease(&cls1);

    return 0;
}

static int test_inherit()
{
    TestCls2 *cls2 = TestCls2_create();
    if (!cls2)
        return 1;

    cls2->data = 42;

    TestIf1 *ifptr = objInstIf(cls2, TestIf1);
    if (!ifptr)
        return 1;

    if (ifptr->testfunc(cls2) != 42)
        return 1;

    objRelease(&cls2);

    return 0;
}

static int test_ifinherit()
{
    TestCls3 *cls3 = TestCls3_create();
    if (!cls3)
        return 1;

    cls3->data = 42;
    cls3->data2 = 69;

    TestIf2 *ifptr = objInstIf(cls3, TestIf2);
    if (!ifptr)
        return 1;

    if (ifptr->testfunc(cls3) != 42)
        return 1;
    if (ifptr->testfunc2(cls3) != 69)
        return 1;

    objRelease(&cls3);

    return 0;
}

static int test_override()
{
    TestCls4 *cls4 = TestCls4_create();
    if (!cls4)
        return 1;

    cls4->data = 42;
    cls4->data2 = 69;
    cls4->data3 = 35;

    TestIf2 *ifptr = objInstIf(cls4, TestIf2);
    if (!ifptr)
        return 1;

    if (ifptr->testfunc(cls4) != 35)
        return 1;
    if (ifptr->testfunc2(cls4) != 69)
        return 1;

    objRelease(&cls4);

    return 0;
}

static int test_abstract()
{
    TestCls4b *cls4 = TestCls4b_create();
    if (!cls4)
        return 1;

    cls4->data = 42;
    cls4->data2 = 69;
    cls4->data3 = 35;
    cls4->data4 = 71;
    cls4->data5 = 99;

    TestIf2 *ifptr = objInstIf(cls4, TestIf2);
    if (!ifptr)
        return 1;
    TestIf3 *ifptr3 = objInstIf(cls4, TestIf3);
    if (!ifptr3)
        return 1;

    if (ifptr->testfunc(cls4) != 71)
        return 1;
    if (ifptr->testfunc2(cls4) != 69)
        return 1;
    if (ifptr3->testfunc3(cls4) != 99)
        return 1;

    if (cls4->_->testfunc(cls4) != 71)
        return 1;
    if (cls4->_->testfunc2(cls4) != 69)
        return 1;
    if (cls4->_->testfunc3(cls4) != 99)
        return 1;

    objRelease(&cls4);

    return 0;
}

static TestCls4b *getptr(TestCls4b *ptr, int *counter)
{
    (*counter)++;
    return ptr;
}

static int test_cast()
{
    TestCls4b *cls4b = TestCls4b_create();

    // counter is to test for side effects from the macro evaluating the expression more than once
    int counter = 0;
    TestCls4a *cls4a = TestCls4a(getptr(cls4b, &counter));
    if ((uintptr)cls4a != (uintptr)cls4b || counter != 1)
        return 1;

    TestCls4 *cls4 = TestCls4(getptr(cls4b, &counter));
    if ((uintptr)cls4 != (uintptr)cls4a || counter != 2)
        return 1;

    TestCls3 *cls3 = TestCls3(getptr(cls4b, &counter));
    if ((uintptr)cls3 != (uintptr)cls4b || counter != 3)
        return 1;

    TestCls2 *cls2 = TestCls2(getptr(cls4b, &counter));
    if ((uintptr)cls2 != (uintptr)cls4b || counter != 4)
        return 1;

    TestCls1 *cls1 = TestCls1(getptr(cls4b, &counter));
    if ((uintptr)cls1 != (uintptr)cls4b || counter != 5)
        return 1;

    // Remove ifdefs for full test; this should fail to compile
#if 0
    TestCls1 *cls2a = TestCls2(cls1);
    if ((uintptr)cls2a != (uintptr)cls4b || counter != 5)
        return 1;
#endif

    ObjInst *obj = ObjInst(getptr(cls4b, &counter));
    if ((uintptr)obj != (uintptr)cls4b || counter != 6)
        return 1;

    objRelease(&cls4);
    return 0;
}

static int test_dyncast()
{
    TestCls4b *cls4 = TestCls4b_create();
    if (!cls4)
        return 1;

    cls4->data = 42;
    cls4->data2 = 69;
    cls4->data3 = 35;
    cls4->data4 = 71;
    cls4->data5 = 99;

    TestCls1 *cls1 = objDynCast(TestCls1, cls4);
    if (!cls1)
        return 1;

    if (cls1->data != 42)
        return 1;

    if (testcls1Testfunc(cls1) != 71)
        return 1;

    objRelease(&cls4);

    return 0;
}


static int test_obj_array()
{
    TestCls5 *cls5 = TestCls5_create();
    if (!cls5)
        return 1;

    cls5->data = 42;

    sa_TestCls5 arr;
    saInit(&arr, object, 10);

    for (int i = 0; i < 50; i++) {
        saPush(&arr, object, cls5);
    }

    if (atomicLoad(uintptr, &cls5->_ref, Acquire) != 51)
        return 1;

    TestIf1 *if1 = objInstIf(arr.a[49], TestIf1);
    if (!if1)
        return 1;
    if (if1->testfunc(arr.a[49]) != 42)
        return 1;

    saDestroy(&arr);

    if (atomicLoad(uintptr, &cls5->_ref, Acquire) != 1)
        return 1;

    saInit(&arr, object, 10);

    for (int i = 0; i < 50; i++) {
        saPush(&arr, object, cls5, SA_Unique);
    }

    if (atomicLoad(uintptr, &cls5->_ref, Acquire) != 2)
        return 1;

    saDestroy(&arr);
    objRelease(&cls5);

    return 0;
}

static int test_obj_weakref()
{
    TestCls4b *cls4 = TestCls4b_create();
    int ret = 0;
    if(!cls4)
        return 1;

    cls4->data = 12;
    cls4->data2 = 99;
    cls4->data3 = 15;
    cls4->data4 = 33;
    cls4->data5 = 73;

    TestCls3 *cls3 = TestCls3(cls4);
    Weak(TestCls3) *cls3w = objGetWeak(TestCls3, cls3);
    if(atomicLoad(uintptr, &cls4->_ref, Acquire) != 1)
        ret = 1;

    TestCls3 *cls3a = objAcquireFromWeak(TestCls3, cls3w);
    if(cls3a) {
        if(atomicLoad(uintptr, &cls3a->_ref, Acquire) != 2)
            ret = 1;

        if(testcls3Testfunc2(cls3) != 99)
            ret = 1;
    } else {
        ret = 1;
    }

    TestCls1 *cls1 = objAcquireFromWeak(TestCls1, cls3w);
    if(cls1) {
        if(atomicLoad(uintptr, &cls1->_ref, Acquire) != 3)
            ret = 1;

        if(cls1->data != 12 || testcls1Testfunc(cls1) != 33)
            ret = 1;
    } else {
        ret = 1;
    }

    TestCls4b *cls4a = objAcquireFromWeakDyn(TestCls4b, cls3w);
    if(cls4a) {
        if(atomicLoad(uintptr, &cls4a->_ref, Acquire) != 4)
            ret = 1;

        if(testcls4Testfunc(cls4a) != 33)
            ret = 1;
    } else {
        ret = 1;
    }

    // test weak references after object has been destroyed

    objRelease(&cls4a);
    objRelease(&cls1);
    objRelease(&cls3a);

    if(atomicLoad(uintptr, &cls4->_ref, Acquire) != 1)
        ret = 1;

    objRelease(&cls4);

    // these should all fail
    cls3a = objAcquireFromWeak(TestCls3, cls3w);
    if(cls3a)
        ret = 1;

    cls1 = objAcquireFromWeak(TestCls1, cls3w);
    if(cls1)
        ret = 1;

    cls4a = objAcquireFromWeakDyn(TestCls4b, cls3w);
    if(cls4a)
        ret = 1;

    objDestroyWeak(&cls3w);

    return ret;
}

testfunc objtest_funcs[] = {
    { "iface", test_iface },
    { "inherit", test_inherit },
    { "ifinherit", test_ifinherit },
    { "override", test_override },
    { "abstract", test_abstract },
    { "cast", test_cast },
    { "dyncast", test_dyncast },
    { "obj_array", test_obj_array },
    { "weakref", test_obj_weakref },
    { 0, 0 }
};
