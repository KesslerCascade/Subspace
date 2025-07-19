// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
HullGraphPanel_ClassIf HullGraphPanel_ClassIf_tmpl = {
    ._size = sizeof(HullGraphPanel_ClassIf),
    ._parent = (ObjIface*)&Panel_ClassIf_tmpl,
};

static UINotifyListener _impl_HullGraphPanel_UINotifyListener = {
    ._size = sizeof(UINotifyListener),
    ._implements = (ObjIface*)&UINotifyListener_tmpl,
    .uiNotify = (void (*)(void*, strref, stvlist*))HullGraphPanel_uiNotify,
};

static HullGraphPanel_ClassIf _impl_HullGraphPanel_HullGraphPanel_ClassIf = {
    ._size = sizeof(HullGraphPanel_ClassIf),
    ._implements = (ObjIface*)&HullGraphPanel_ClassIf_tmpl,
    .clear = (void (*)(void*))HullGraphPanel_clear,
    .handleUpdate = (void (*)(void*, int64, bool))HullGraphPanel_handleUpdate,
    .make = (bool (*)(void*))HullGraphPanel_make,
    .uiNotify = (void (*)(void*, strref, stvlist*))HullGraphPanel_uiNotify,
};

static ObjIface* _ifimpl_HullGraphPanel[] = {
    (ObjIface*)&_impl_HullGraphPanel_UINotifyListener,
    (ObjIface*)&_impl_HullGraphPanel_HullGraphPanel_ClassIf,
    NULL
};

ObjClassInfo HullGraphPanel_clsinfo = {
    .instsize = sizeof(HullGraphPanel),
    .classif = (ObjIface*)&HullGraphPanel_ClassIf_tmpl,
    .parent = &Panel_clsinfo,
    .init = (bool(*)(void*))HullGraphPanel_init,
    .destroy = (void(*)(void*))HullGraphPanel_destroy,
    .ifimpl = _ifimpl_HullGraphPanel,
};

// ==================== Auto-generated section ends ======================
