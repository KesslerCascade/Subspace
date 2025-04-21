// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
SettingsTree_ClassIf SettingsTree_ClassIf_tmpl = {
    ._size = sizeof(SettingsTree_ClassIf),
    ._parent = (ObjIface*)&SSDTree_ClassIf_tmpl,
};

static SettingsTree_ClassIf _impl_SettingsTree_SettingsTree_ClassIf = {
    ._size = sizeof(SettingsTree_ClassIf),
    ._implements = (ObjIface*)&SettingsTree_ClassIf_tmpl,
};

static ObjIface* _ifimpl_SettingsTree[] = {
    (ObjIface*)&_impl_SettingsTree_SettingsTree_ClassIf,
    NULL
};

ObjClassInfo SettingsTree_clsinfo = {
    .instsize = sizeof(SettingsTree),
    .classif = (ObjIface*)&SettingsTree_ClassIf_tmpl,
    .parent = &SSDTree_clsinfo,
    .init = (bool(*)(void*))SettingsTree_init,
    .destroy = (void(*)(void*))SettingsTree_destroy,
    .ifimpl = _ifimpl_SettingsTree,
};

// ==================== Auto-generated section ends ======================
