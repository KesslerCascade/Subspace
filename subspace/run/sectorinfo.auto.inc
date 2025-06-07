// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
SectorInfo_ClassIf SectorInfo_ClassIf_tmpl = {
    ._size = sizeof(SectorInfo_ClassIf),
};

static Sortable _impl_SectorInfo_Sortable = {
    ._size = sizeof(Sortable),
    ._implements = (ObjIface*)&Sortable_tmpl,
    .cmp = (intptr (*)(void*, void*, uint32))SectorInfo_cmp,
};

static SectorInfo_ClassIf _impl_SectorInfo_SectorInfo_ClassIf = {
    ._size = sizeof(SectorInfo_ClassIf),
    ._implements = (ObjIface*)&SectorInfo_ClassIf_tmpl,
    .cmp = (intptr (*)(void*, void*, uint32))SectorInfo_cmp,
};

static ObjIface* _ifimpl_SectorInfo[] = {
    (ObjIface*)&_impl_SectorInfo_Sortable,
    (ObjIface*)&_impl_SectorInfo_SectorInfo_ClassIf,
    NULL
};

ObjClassInfo SectorInfo_clsinfo = {
    .instsize = sizeof(SectorInfo),
    .classif = (ObjIface*)&SectorInfo_ClassIf_tmpl,
    .destroy = (void(*)(void*))SectorInfo_destroy,
    .ifimpl = _ifimpl_SectorInfo,
};

// ==================== Auto-generated section ends ======================
