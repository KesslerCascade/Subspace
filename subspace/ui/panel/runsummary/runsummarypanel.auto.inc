// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
RunSummaryPanel_ClassIf RunSummaryPanel_ClassIf_tmpl = {
    ._size = sizeof(RunSummaryPanel_ClassIf),
    ._parent = (ObjIface*)&Panel_ClassIf_tmpl,
};

static RunSummaryPanel_ClassIf _impl_RunSummaryPanel_RunSummaryPanel_ClassIf = {
    ._size = sizeof(RunSummaryPanel_ClassIf),
    ._implements = (ObjIface*)&RunSummaryPanel_ClassIf_tmpl,
    .make = (bool (*)(void*))RunSummaryPanel_make,
};

static ObjIface* _ifimpl_RunSummaryPanel[] = {
    (ObjIface*)&_impl_RunSummaryPanel_RunSummaryPanel_ClassIf,
    NULL
};

ObjClassInfo RunSummaryPanel_clsinfo = {
    .instsize = sizeof(RunSummaryPanel),
    .classif = (ObjIface*)&RunSummaryPanel_ClassIf_tmpl,
    .parent = &Panel_clsinfo,
    .ifimpl = _ifimpl_RunSummaryPanel,
};

// ==================== Auto-generated section ends ======================
