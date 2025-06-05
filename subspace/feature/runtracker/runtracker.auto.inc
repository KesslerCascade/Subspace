// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
RunTracker_ClassIf RunTracker_ClassIf_tmpl = {
    ._size = sizeof(RunTracker_ClassIf),
    ._parent = (ObjIface*)&SubspaceFeature_ClassIf_tmpl,
};

static RunTracker_ClassIf _impl_RunTracker_RunTracker_ClassIf = {
    ._size = sizeof(RunTracker_ClassIf),
    ._implements = (ObjIface*)&RunTracker_ClassIf_tmpl,
    .createSettingsPage = (SettingsPage* (*)(void*, SubspaceUI*))RunTracker_createSettingsPage,
    .applyDefaultSettings = (void (*)(void*))RunTracker_applyDefaultSettings,
    .enable = (void (*)(void*, bool))RunTracker_enable,
};

static ObjIface* _ifimpl_RunTracker[] = {
    (ObjIface*)&_impl_RunTracker_RunTracker_ClassIf,
    NULL
};

ObjClassInfo RunTracker_clsinfo = {
    .instsize = sizeof(RunTracker),
    .classif = (ObjIface*)&RunTracker_ClassIf_tmpl,
    .parent = &SubspaceFeature_clsinfo,
    .ifimpl = _ifimpl_RunTracker,
};

// ==================== Auto-generated section ends ======================
