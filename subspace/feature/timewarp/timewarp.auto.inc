// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
TimeWarp_ClassIf TimeWarp_ClassIf_tmpl = {
    ._size = sizeof(TimeWarp_ClassIf),
    ._parent = (ObjIface*)&SubspaceFeature_ClassIf_tmpl,
};

static TimeWarp_ClassIf _impl_TimeWarp_TimeWarp_ClassIf = {
    ._size = sizeof(TimeWarp_ClassIf),
    ._implements = (ObjIface*)&TimeWarp_ClassIf_tmpl,
    .loadSettings = (void (*)(void*))TimeWarp_loadSettings,
};

static ObjIface* _ifimpl_TimeWarp[] = {
    (ObjIface*)&_impl_TimeWarp_TimeWarp_ClassIf,
    NULL
};

ObjClassInfo TimeWarp_clsinfo = {
    .instsize = sizeof(TimeWarp),
    .classif = (ObjIface*)&TimeWarp_ClassIf_tmpl,
    .parent = &SubspaceFeature_clsinfo,
    .ifimpl = _ifimpl_TimeWarp,
};

// ==================== Auto-generated section ends ======================
