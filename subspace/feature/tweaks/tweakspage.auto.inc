// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
TweaksPage_ClassIf TweaksPage_ClassIf_tmpl = {
    ._size = sizeof(TweaksPage_ClassIf),
    ._parent = (ObjIface*)&SettingsPage_ClassIf_tmpl,
};

static TweaksPage_ClassIf _impl_TweaksPage_TweaksPage_ClassIf = {
    ._size = sizeof(TweaksPage_ClassIf),
    ._implements = (ObjIface*)&TweaksPage_ClassIf_tmpl,
    .make = (bool (*)(void*, Ihandle*))TweaksPage_make,
    .update = (bool (*)(void*))TweaksPage_update,
};

static ObjIface* _ifimpl_TweaksPage[] = {
    (ObjIface*)&_impl_TweaksPage_TweaksPage_ClassIf,
    NULL
};

ObjClassInfo TweaksPage_clsinfo = {
    .instsize = sizeof(TweaksPage),
    .classif = (ObjIface*)&TweaksPage_ClassIf_tmpl,
    .parent = &SettingsPage_clsinfo,
    .destroy = (void(*)(void*))TweaksPage_destroy,
    .ifimpl = _ifimpl_TweaksPage,
};

// ==================== Auto-generated section ends ======================
