// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
ConvertTestClass_ClassIf ConvertTestClass_ClassIf_tmpl = {
    ._size = sizeof(ConvertTestClass_ClassIf),
};

static Convertible _impl_ConvertTestClass_Convertible = {
    ._size = sizeof(Convertible),
    ._implements = (ObjIface*)&Convertible_tmpl,
    .convert = (bool (*)(void*, stype, stgeneric*, uint32))ConvertTestClass_convert,
};

static ConvertTestClass_ClassIf _impl_ConvertTestClass_ConvertTestClass_ClassIf = {
    ._size = sizeof(ConvertTestClass_ClassIf),
    ._implements = (ObjIface*)&ConvertTestClass_ClassIf_tmpl,
    .convert = (bool (*)(void*, stype, stgeneric*, uint32))ConvertTestClass_convert,
};

static ObjIface* _ifimpl_ConvertTestClass[] = {
    (ObjIface*)&_impl_ConvertTestClass_Convertible,
    (ObjIface*)&_impl_ConvertTestClass_ConvertTestClass_ClassIf,
    NULL
};

ObjClassInfo ConvertTestClass_clsinfo = {
    .instsize = sizeof(ConvertTestClass),
    .classif = (ObjIface*)&ConvertTestClass_ClassIf_tmpl,
    .destroy = (void(*)(void*))ConvertTestClass_destroy,
    .ifimpl = _ifimpl_ConvertTestClass,
};

// ==================== Auto-generated section ends ======================
