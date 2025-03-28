// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
Method_ClassIf Method_ClassIf_tmpl = {
    ._size = sizeof(Method_ClassIf),
};

Interface_ClassIf Interface_ClassIf_tmpl = {
    ._size = sizeof(Interface_ClassIf),
};

Member_ClassIf Member_ClassIf_tmpl = {
    ._size = sizeof(Member_ClassIf),
};

Class_ClassIf Class_ClassIf_tmpl = {
    ._size = sizeof(Class_ClassIf),
};

static ObjIface* _ifimpl_Param[] = {
    NULL
};

ObjClassInfo Param_clsinfo = {
    .instsize = sizeof(Param),
    .destroy = (void(*)(void*))Param_destroy,
    .ifimpl = _ifimpl_Param,
};

static Sortable _impl_Method_Sortable = {
    ._size = sizeof(Sortable),
    ._implements = (ObjIface*)&Sortable_tmpl,
    .cmp = (intptr (*)(void*, void*, uint32))Method_cmp,
};

static Method_ClassIf _impl_Method_Method_ClassIf = {
    ._size = sizeof(Method_ClassIf),
    ._implements = (ObjIface*)&Method_ClassIf_tmpl,
    .clone = (Method* (*)(void*))Method_clone,
    .cmp = (intptr (*)(void*, void*, uint32))Method_cmp,
};

static ObjIface* _ifimpl_Method[] = {
    (ObjIface*)&_impl_Method_Sortable,
    (ObjIface*)&_impl_Method_Method_ClassIf,
    NULL
};

ObjClassInfo Method_clsinfo = {
    .instsize = sizeof(Method),
    .classif = (ObjIface*)&Method_ClassIf_tmpl,
    .init = (bool(*)(void*))Method_init,
    .destroy = (void(*)(void*))Method_destroy,
    .ifimpl = _ifimpl_Method,
};

static Sortable _impl_Interface_Sortable = {
    ._size = sizeof(Sortable),
    ._implements = (ObjIface*)&Sortable_tmpl,
    .cmp = (intptr (*)(void*, void*, uint32))Interface_cmp,
};

static Interface_ClassIf _impl_Interface_Interface_ClassIf = {
    ._size = sizeof(Interface_ClassIf),
    ._implements = (ObjIface*)&Interface_ClassIf_tmpl,
    .cmp = (intptr (*)(void*, void*, uint32))Interface_cmp,
};

static ObjIface* _ifimpl_Interface[] = {
    (ObjIface*)&_impl_Interface_Sortable,
    (ObjIface*)&_impl_Interface_Interface_ClassIf,
    NULL
};

ObjClassInfo Interface_clsinfo = {
    .instsize = sizeof(Interface),
    .classif = (ObjIface*)&Interface_ClassIf_tmpl,
    .init = (bool(*)(void*))Interface_init,
    .destroy = (void(*)(void*))Interface_destroy,
    .ifimpl = _ifimpl_Interface,
};

static Sortable _impl_Member_Sortable = {
    ._size = sizeof(Sortable),
    ._implements = (ObjIface*)&Sortable_tmpl,
    .cmp = (intptr (*)(void*, void*, uint32))Member_cmp,
};

static Member_ClassIf _impl_Member_Member_ClassIf = {
    ._size = sizeof(Member_ClassIf),
    ._implements = (ObjIface*)&Member_ClassIf_tmpl,
    .cmp = (intptr (*)(void*, void*, uint32))Member_cmp,
};

static ObjIface* _ifimpl_Member[] = {
    (ObjIface*)&_impl_Member_Sortable,
    (ObjIface*)&_impl_Member_Member_ClassIf,
    NULL
};

ObjClassInfo Member_clsinfo = {
    .instsize = sizeof(Member),
    .classif = (ObjIface*)&Member_ClassIf_tmpl,
    .destroy = (void(*)(void*))Member_destroy,
    .ifimpl = _ifimpl_Member,
};

static Sortable _impl_Class_Sortable = {
    ._size = sizeof(Sortable),
    ._implements = (ObjIface*)&Sortable_tmpl,
    .cmp = (intptr (*)(void*, void*, uint32))Class_cmp,
};

static Class_ClassIf _impl_Class_Class_ClassIf = {
    ._size = sizeof(Class_ClassIf),
    ._implements = (ObjIface*)&Class_ClassIf_tmpl,
    .cmp = (intptr (*)(void*, void*, uint32))Class_cmp,
};

static ObjIface* _ifimpl_Class[] = {
    (ObjIface*)&_impl_Class_Sortable,
    (ObjIface*)&_impl_Class_Class_ClassIf,
    NULL
};

ObjClassInfo Class_clsinfo = {
    .instsize = sizeof(Class),
    .classif = (ObjIface*)&Class_ClassIf_tmpl,
    .init = (bool(*)(void*))Class_init,
    .destroy = (void(*)(void*))Class_destroy,
    .ifimpl = _ifimpl_Class,
};

static ObjIface* _ifimpl_ComplexArrayType[] = {
    NULL
};

ObjClassInfo ComplexArrayType_clsinfo = {
    .instsize = sizeof(ComplexArrayType),
    .destroy = (void(*)(void*))ComplexArrayType_destroy,
    .ifimpl = _ifimpl_ComplexArrayType,
};

// ==================== Auto-generated section ends ======================
