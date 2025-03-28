// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
TestIf1 TestIf1_tmpl = {
    ._size = sizeof(TestIf1),
};

TestIf2 TestIf2_tmpl = {
    ._size = sizeof(TestIf2),
    ._parent = (ObjIface*)&TestIf1_tmpl,
};

TestIf3 TestIf3_tmpl = {
    ._size = sizeof(TestIf3),
};

TestCls1_ClassIf TestCls1_ClassIf_tmpl = {
    ._size = sizeof(TestCls1_ClassIf),
};

TestCls2_ClassIf TestCls2_ClassIf_tmpl = {
    ._size = sizeof(TestCls2_ClassIf),
    ._parent = (ObjIface*)&TestCls1_ClassIf_tmpl,
};

TestCls3_ClassIf TestCls3_ClassIf_tmpl = {
    ._size = sizeof(TestCls3_ClassIf),
    ._parent = (ObjIface*)&TestCls2_ClassIf_tmpl,
};

TestCls4_ClassIf TestCls4_ClassIf_tmpl = {
    ._size = sizeof(TestCls4_ClassIf),
    ._parent = (ObjIface*)&TestCls3_ClassIf_tmpl,
};

TestCls4a_ClassIf TestCls4a_ClassIf_tmpl = {
    ._size = sizeof(TestCls4a_ClassIf),
    ._parent = (ObjIface*)&TestCls4_ClassIf_tmpl,
};

TestCls4b_ClassIf TestCls4b_ClassIf_tmpl = {
    ._size = sizeof(TestCls4b_ClassIf),
    ._parent = (ObjIface*)&TestCls4a_ClassIf_tmpl,
};

TestCls5_ClassIf TestCls5_ClassIf_tmpl = {
    ._size = sizeof(TestCls5_ClassIf),
    ._parent = (ObjIface*)&TestCls1_ClassIf_tmpl,
};

static TestIf1 _impl_TestCls1_TestIf1 = {
    ._size = sizeof(TestIf1),
    ._implements = (ObjIface*)&TestIf1_tmpl,
    .testfunc = (int (*)(void*))TestCls1_testfunc,
};

static TestCls1_ClassIf _impl_TestCls1_TestCls1_ClassIf = {
    ._size = sizeof(TestCls1_ClassIf),
    ._implements = (ObjIface*)&TestCls1_ClassIf_tmpl,
    .testfunc = (int (*)(void*))TestCls1_testfunc,
};

static ObjIface* _ifimpl_TestCls1[] = {
    (ObjIface*)&_impl_TestCls1_TestIf1,
    (ObjIface*)&_impl_TestCls1_TestCls1_ClassIf,
    NULL
};

ObjClassInfo TestCls1_clsinfo = {
    .instsize = sizeof(TestCls1),
    .classif = (ObjIface*)&TestCls1_ClassIf_tmpl,
    .ifimpl = _ifimpl_TestCls1,
};

static TestCls2_ClassIf _impl_TestCls2_TestCls2_ClassIf = {
    ._size = sizeof(TestCls2_ClassIf),
    ._implements = (ObjIface*)&TestCls2_ClassIf_tmpl,
};

static ObjIface* _ifimpl_TestCls2[] = {
    (ObjIface*)&_impl_TestCls2_TestCls2_ClassIf,
    NULL
};

ObjClassInfo TestCls2_clsinfo = {
    .instsize = sizeof(TestCls2),
    .classif = (ObjIface*)&TestCls2_ClassIf_tmpl,
    .parent = &TestCls1_clsinfo,
    .ifimpl = _ifimpl_TestCls2,
};

static TestIf2 _impl_TestCls3_TestIf2 = {
    ._size = sizeof(TestIf2),
    ._implements = (ObjIface*)&TestIf2_tmpl,
    .testfunc2 = (int (*)(void*))TestCls3_testfunc2,
};

static TestCls3_ClassIf _impl_TestCls3_TestCls3_ClassIf = {
    ._size = sizeof(TestCls3_ClassIf),
    ._implements = (ObjIface*)&TestCls3_ClassIf_tmpl,
    .testfunc2 = (int (*)(void*))TestCls3_testfunc2,
};

static ObjIface* _ifimpl_TestCls3[] = {
    (ObjIface*)&_impl_TestCls3_TestIf2,
    (ObjIface*)&_impl_TestCls3_TestCls3_ClassIf,
    NULL
};

ObjClassInfo TestCls3_clsinfo = {
    .instsize = sizeof(TestCls3),
    .classif = (ObjIface*)&TestCls3_ClassIf_tmpl,
    .parent = &TestCls2_clsinfo,
    .ifimpl = _ifimpl_TestCls3,
};

static TestIf2 _impl_TestCls4_TestIf2 = {
    ._size = sizeof(TestIf2),
    ._implements = (ObjIface*)&TestIf2_tmpl,
    .testfunc = (int (*)(void*))TestCls4_testfunc,
};

static TestCls4_ClassIf _impl_TestCls4_TestCls4_ClassIf = {
    ._size = sizeof(TestCls4_ClassIf),
    ._implements = (ObjIface*)&TestCls4_ClassIf_tmpl,
    .testfunc = (int (*)(void*))TestCls4_testfunc,
};

static ObjIface* _ifimpl_TestCls4[] = {
    (ObjIface*)&_impl_TestCls4_TestIf2,
    (ObjIface*)&_impl_TestCls4_TestCls4_ClassIf,
    NULL
};

ObjClassInfo TestCls4_clsinfo = {
    .instsize = sizeof(TestCls4),
    .classif = (ObjIface*)&TestCls4_ClassIf_tmpl,
    .parent = &TestCls3_clsinfo,
    .ifimpl = _ifimpl_TestCls4,
};

static TestIf3 _impl_TestCls4a_TestIf3 = {
    ._size = sizeof(TestIf3),
    ._implements = (ObjIface*)&TestIf3_tmpl,
};

static TestIf2 _impl_TestCls4a_TestIf2 = {
    ._size = sizeof(TestIf2),
    ._implements = (ObjIface*)&TestIf2_tmpl,
    .testfunc = (int (*)(void*))TestCls4a_testfunc,
};

static TestCls4a_ClassIf _impl_TestCls4a_TestCls4a_ClassIf = {
    ._size = sizeof(TestCls4a_ClassIf),
    ._implements = (ObjIface*)&TestCls4a_ClassIf_tmpl,
    .testfunc = (int (*)(void*))TestCls4a_testfunc,
};

static ObjIface* _ifimpl_TestCls4a[] = {
    (ObjIface*)&_impl_TestCls4a_TestIf3,
    (ObjIface*)&_impl_TestCls4a_TestIf2,
    (ObjIface*)&_impl_TestCls4a_TestCls4a_ClassIf,
    NULL
};

ObjClassInfo TestCls4a_clsinfo = {
    .instsize = sizeof(TestCls4a),
    .classif = (ObjIface*)&TestCls4a_ClassIf_tmpl,
    .parent = &TestCls4_clsinfo,
    ._abstract = true,
    .ifimpl = _ifimpl_TestCls4a,
};

static TestIf3 _impl_TestCls4b_TestIf3 = {
    ._size = sizeof(TestIf3),
    ._implements = (ObjIface*)&TestIf3_tmpl,
    .testfunc3 = (int (*)(void*))TestCls4b_testfunc3,
};

static TestCls4b_ClassIf _impl_TestCls4b_TestCls4b_ClassIf = {
    ._size = sizeof(TestCls4b_ClassIf),
    ._implements = (ObjIface*)&TestCls4b_ClassIf_tmpl,
    .testfunc3 = (int (*)(void*))TestCls4b_testfunc3,
};

static ObjIface* _ifimpl_TestCls4b[] = {
    (ObjIface*)&_impl_TestCls4b_TestIf3,
    (ObjIface*)&_impl_TestCls4b_TestCls4b_ClassIf,
    NULL
};

ObjClassInfo TestCls4b_clsinfo = {
    .instsize = sizeof(TestCls4b),
    .classif = (ObjIface*)&TestCls4b_ClassIf_tmpl,
    .parent = &TestCls4a_clsinfo,
    .ifimpl = _ifimpl_TestCls4b,
};

static Sortable _impl_TestCls5_Sortable = {
    ._size = sizeof(Sortable),
    ._implements = (ObjIface*)&Sortable_tmpl,
    .cmp = (intptr (*)(void*, void*, uint32))TestCls5_cmp,
};

static TestCls5_ClassIf _impl_TestCls5_TestCls5_ClassIf = {
    ._size = sizeof(TestCls5_ClassIf),
    ._implements = (ObjIface*)&TestCls5_ClassIf_tmpl,
    .cmp = (intptr (*)(void*, void*, uint32))TestCls5_cmp,
};

static ObjIface* _ifimpl_TestCls5[] = {
    (ObjIface*)&_impl_TestCls5_Sortable,
    (ObjIface*)&_impl_TestCls5_TestCls5_ClassIf,
    NULL
};

ObjClassInfo TestCls5_clsinfo = {
    .instsize = sizeof(TestCls5),
    .classif = (ObjIface*)&TestCls5_ClassIf_tmpl,
    .parent = &TestCls1_clsinfo,
    .ifimpl = _ifimpl_TestCls5,
};

// ==================== Auto-generated section ends ======================
