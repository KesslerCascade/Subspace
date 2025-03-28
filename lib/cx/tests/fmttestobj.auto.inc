// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
FmtTestClass_ClassIf FmtTestClass_ClassIf_tmpl = {
    ._size = sizeof(FmtTestClass_ClassIf),
};

FmtTestClass2_ClassIf FmtTestClass2_ClassIf_tmpl = {
    ._size = sizeof(FmtTestClass2_ClassIf),
};

static Formattable _impl_FmtTestClass_Formattable = {
    ._size = sizeof(Formattable),
    ._implements = (ObjIface*)&Formattable_tmpl,
    .format = (bool (*)(void*, FMTVar*, string*))FmtTestClass_format,
};

static FmtTestClass_ClassIf _impl_FmtTestClass_FmtTestClass_ClassIf = {
    ._size = sizeof(FmtTestClass_ClassIf),
    ._implements = (ObjIface*)&FmtTestClass_ClassIf_tmpl,
    .format = (bool (*)(void*, FMTVar*, string*))FmtTestClass_format,
};

static ObjIface* _ifimpl_FmtTestClass[] = {
    (ObjIface*)&_impl_FmtTestClass_Formattable,
    (ObjIface*)&_impl_FmtTestClass_FmtTestClass_ClassIf,
    NULL
};

ObjClassInfo FmtTestClass_clsinfo = {
    .instsize = sizeof(FmtTestClass),
    .classif = (ObjIface*)&FmtTestClass_ClassIf_tmpl,
    .destroy = (void(*)(void*))FmtTestClass_destroy,
    .ifimpl = _ifimpl_FmtTestClass,
};

static Convertible _impl_FmtTestClass2_Convertible = {
    ._size = sizeof(Convertible),
    ._implements = (ObjIface*)&Convertible_tmpl,
    .convert = (bool (*)(void*, stype, stgeneric*, uint32))FmtTestClass2_convert,
};

static FmtTestClass2_ClassIf _impl_FmtTestClass2_FmtTestClass2_ClassIf = {
    ._size = sizeof(FmtTestClass2_ClassIf),
    ._implements = (ObjIface*)&FmtTestClass2_ClassIf_tmpl,
    .convert = (bool (*)(void*, stype, stgeneric*, uint32))FmtTestClass2_convert,
};

static ObjIface* _ifimpl_FmtTestClass2[] = {
    (ObjIface*)&_impl_FmtTestClass2_Convertible,
    (ObjIface*)&_impl_FmtTestClass2_FmtTestClass2_ClassIf,
    NULL
};

ObjClassInfo FmtTestClass2_clsinfo = {
    .instsize = sizeof(FmtTestClass2),
    .classif = (ObjIface*)&FmtTestClass2_ClassIf_tmpl,
    .destroy = (void(*)(void*))FmtTestClass2_destroy,
    .ifimpl = _ifimpl_FmtTestClass2,
};

// ==================== Auto-generated section ends ======================
