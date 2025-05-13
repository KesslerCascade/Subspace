// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
KeybindsPage_ClassIf KeybindsPage_ClassIf_tmpl = {
    ._size = sizeof(KeybindsPage_ClassIf),
    ._parent = (ObjIface*)&SettingsPage_ClassIf_tmpl,
};

static KeybindsPage_ClassIf _impl_KeybindsPage_KeybindsPage_ClassIf = {
    ._size = sizeof(KeybindsPage_ClassIf),
    ._implements = (ObjIface*)&KeybindsPage_ClassIf_tmpl,
    .make = (bool (*)(void*, Ihandle*))KeybindsPage_make,
    .update = (bool (*)(void*))KeybindsPage_update,
};

static ObjIface* _ifimpl_KeybindsPage[] = {
    (ObjIface*)&_impl_KeybindsPage_KeybindsPage_ClassIf,
    NULL
};

ObjClassInfo KeybindsPage_clsinfo = {
    .instsize = sizeof(KeybindsPage),
    .classif = (ObjIface*)&KeybindsPage_ClassIf_tmpl,
    .parent = &SettingsPage_clsinfo,
    .init = (bool(*)(void*))KeybindsPage_init,
    .ifimpl = _ifimpl_KeybindsPage,
};

// ==================== Auto-generated section ends ======================
