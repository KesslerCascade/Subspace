// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
ScrapGraphPanel_ClassIf ScrapGraphPanel_ClassIf_tmpl = {
    ._size = sizeof(ScrapGraphPanel_ClassIf),
    ._parent = (ObjIface*)&Panel_ClassIf_tmpl,
};

static UINotifyListener _impl_ScrapGraphPanel_UINotifyListener = {
    ._size = sizeof(UINotifyListener),
    ._implements = (ObjIface*)&UINotifyListener_tmpl,
    .uiNotify = (void (*)(void*, strref, stvlist*))ScrapGraphPanel_uiNotify,
};

static ScrapGraphPanel_ClassIf _impl_ScrapGraphPanel_ScrapGraphPanel_ClassIf = {
    ._size = sizeof(ScrapGraphPanel_ClassIf),
    ._implements = (ObjIface*)&ScrapGraphPanel_ClassIf_tmpl,
    .clear = (void (*)(void*))ScrapGraphPanel_clear,
    .handleUpdate = (void (*)(void*, int64, bool))ScrapGraphPanel_handleUpdate,
    .make = (bool (*)(void*))ScrapGraphPanel_make,
    .uiNotify = (void (*)(void*, strref, stvlist*))ScrapGraphPanel_uiNotify,
};

static ObjIface* _ifimpl_ScrapGraphPanel[] = {
    (ObjIface*)&_impl_ScrapGraphPanel_UINotifyListener,
    (ObjIface*)&_impl_ScrapGraphPanel_ScrapGraphPanel_ClassIf,
    NULL
};

ObjClassInfo ScrapGraphPanel_clsinfo = {
    .instsize = sizeof(ScrapGraphPanel),
    .classif = (ObjIface*)&ScrapGraphPanel_ClassIf_tmpl,
    .parent = &Panel_clsinfo,
    .init = (bool(*)(void*))ScrapGraphPanel_init,
    .destroy = (void(*)(void*))ScrapGraphPanel_destroy,
    .ifimpl = _ifimpl_ScrapGraphPanel,
};

// ==================== Auto-generated section ends ======================
