// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
RunTrackerPage_ClassIf RunTrackerPage_ClassIf_tmpl = {
    ._size = sizeof(RunTrackerPage_ClassIf),
    ._parent = (ObjIface*)&SettingsPage_ClassIf_tmpl,
};

static RunTrackerPage_ClassIf _impl_RunTrackerPage_RunTrackerPage_ClassIf = {
    ._size = sizeof(RunTrackerPage_ClassIf),
    ._implements = (ObjIface*)&RunTrackerPage_ClassIf_tmpl,
    .make = (bool (*)(void*, Ihandle*))RunTrackerPage_make,
    .update = (bool (*)(void*))RunTrackerPage_update,
};

static ObjIface* _ifimpl_RunTrackerPage[] = {
    (ObjIface*)&_impl_RunTrackerPage_RunTrackerPage_ClassIf,
    NULL
};

ObjClassInfo RunTrackerPage_clsinfo = {
    .instsize = sizeof(RunTrackerPage),
    .classif = (ObjIface*)&RunTrackerPage_ClassIf_tmpl,
    .parent = &SettingsPage_clsinfo,
    .ifimpl = _ifimpl_RunTrackerPage,
};

// ==================== Auto-generated section ends ======================
