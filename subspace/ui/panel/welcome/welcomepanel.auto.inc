// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
WelcomePanel_ClassIf WelcomePanel_ClassIf_tmpl = {
    ._size = sizeof(WelcomePanel_ClassIf),
    ._parent = (ObjIface*)&Panel_ClassIf_tmpl,
};

static WelcomePanel_ClassIf _impl_WelcomePanel_WelcomePanel_ClassIf = {
    ._size = sizeof(WelcomePanel_ClassIf),
    ._implements = (ObjIface*)&WelcomePanel_ClassIf_tmpl,
    .make = (bool (*)(void*))WelcomePanel_make,
};

static ObjIface* _ifimpl_WelcomePanel[] = {
    (ObjIface*)&_impl_WelcomePanel_WelcomePanel_ClassIf,
    NULL
};

ObjClassInfo WelcomePanel_clsinfo = {
    .instsize = sizeof(WelcomePanel),
    .classif = (ObjIface*)&WelcomePanel_ClassIf_tmpl,
    .parent = &Panel_clsinfo,
    .ifimpl = _ifimpl_WelcomePanel,
};

// ==================== Auto-generated section ends ======================
