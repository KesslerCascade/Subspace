// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
ScrapGraphPanel_ClassIf ScrapGraphPanel_ClassIf_tmpl = {
    ._size = sizeof(ScrapGraphPanel_ClassIf),
    ._parent = (ObjIface*)&Panel_ClassIf_tmpl,
};

static ScrapGraphPanel_ClassIf _impl_ScrapGraphPanel_ScrapGraphPanel_ClassIf = {
    ._size = sizeof(ScrapGraphPanel_ClassIf),
    ._implements = (ObjIface*)&ScrapGraphPanel_ClassIf_tmpl,
    .make = (bool (*)(void*))ScrapGraphPanel_make,
};

static ObjIface* _ifimpl_ScrapGraphPanel[] = {
    (ObjIface*)&_impl_ScrapGraphPanel_ScrapGraphPanel_ClassIf,
    NULL
};

ObjClassInfo ScrapGraphPanel_clsinfo = {
    .instsize = sizeof(ScrapGraphPanel),
    .classif = (ObjIface*)&ScrapGraphPanel_ClassIf_tmpl,
    .parent = &Panel_clsinfo,
    .ifimpl = _ifimpl_ScrapGraphPanel,
};

// ==================== Auto-generated section ends ======================
