// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
TimeWarpPage_ClassIf TimeWarpPage_ClassIf_tmpl = {
    ._size = sizeof(TimeWarpPage_ClassIf),
    ._parent = (ObjIface*)&SettingsPage_ClassIf_tmpl,
};

static TimeWarpPage_ClassIf _impl_TimeWarpPage_TimeWarpPage_ClassIf = {
    ._size = sizeof(TimeWarpPage_ClassIf),
    ._implements = (ObjIface*)&TimeWarpPage_ClassIf_tmpl,
    .make = (bool (*)(void*, Ihandle*))TimeWarpPage_make,
    .update = (bool (*)(void*))TimeWarpPage_update,
};

static ObjIface* _ifimpl_TimeWarpPage[] = {
    (ObjIface*)&_impl_TimeWarpPage_TimeWarpPage_ClassIf,
    NULL
};

ObjClassInfo TimeWarpPage_clsinfo = {
    .instsize = sizeof(TimeWarpPage),
    .classif = (ObjIface*)&TimeWarpPage_ClassIf_tmpl,
    .parent = &SettingsPage_clsinfo,
    .destroy = (void(*)(void*))TimeWarpPage_destroy,
    .ifimpl = _ifimpl_TimeWarpPage,
};

// ==================== Auto-generated section ends ======================
