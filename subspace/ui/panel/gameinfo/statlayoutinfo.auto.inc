// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
StatLayoutInfo_ClassIf StatLayoutInfo_ClassIf_tmpl = {
    ._size = sizeof(StatLayoutInfo_ClassIf),
};

static StatLayoutInfo_ClassIf _impl_StatLayoutInfo_StatLayoutInfo_ClassIf = {
    ._size = sizeof(StatLayoutInfo_ClassIf),
    ._implements = (ObjIface*)&StatLayoutInfo_ClassIf_tmpl,
    .update = (void (*)(void*, Ihandle*))StatLayoutInfo_update,
};

static ObjIface* _ifimpl_StatLayoutInfo[] = {
    (ObjIface*)&_impl_StatLayoutInfo_StatLayoutInfo_ClassIf,
    NULL
};

ObjClassInfo StatLayoutInfo_clsinfo = {
    .instsize = sizeof(StatLayoutInfo),
    .classif = (ObjIface*)&StatLayoutInfo_ClassIf_tmpl,
    .destroy = (void(*)(void*))StatLayoutInfo_destroy,
    .ifimpl = _ifimpl_StatLayoutInfo,
};

// ==================== Auto-generated section ends ======================
