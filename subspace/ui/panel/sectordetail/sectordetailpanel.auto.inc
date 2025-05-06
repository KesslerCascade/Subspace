// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
SectorDetailPanel_ClassIf SectorDetailPanel_ClassIf_tmpl = {
    ._size = sizeof(SectorDetailPanel_ClassIf),
    ._parent = (ObjIface*)&Panel_ClassIf_tmpl,
};

static SectorDetailPanel_ClassIf _impl_SectorDetailPanel_SectorDetailPanel_ClassIf = {
    ._size = sizeof(SectorDetailPanel_ClassIf),
    ._implements = (ObjIface*)&SectorDetailPanel_ClassIf_tmpl,
    .make = (bool (*)(void*))SectorDetailPanel_make,
};

static ObjIface* _ifimpl_SectorDetailPanel[] = {
    (ObjIface*)&_impl_SectorDetailPanel_SectorDetailPanel_ClassIf,
    NULL
};

ObjClassInfo SectorDetailPanel_clsinfo = {
    .instsize = sizeof(SectorDetailPanel),
    .classif = (ObjIface*)&SectorDetailPanel_ClassIf_tmpl,
    .parent = &Panel_clsinfo,
    .ifimpl = _ifimpl_SectorDetailPanel,
};

// ==================== Auto-generated section ends ======================
