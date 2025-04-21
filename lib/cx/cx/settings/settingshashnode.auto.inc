// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
SettingsHashNode_ClassIf SettingsHashNode_ClassIf_tmpl = {
    ._size = sizeof(SettingsHashNode_ClassIf),
    ._parent = (ObjIface*)&SSDHashNode_ClassIf_tmpl,
};

static SSDNodeIf _impl_SettingsHashNode_SSDNodeIf = {
    ._size = sizeof(SSDNodeIf),
    ._implements = (ObjIface*)&SSDNodeIf_tmpl,
    .get = (bool (*)(void*, int32, strref, stvar*, SSDLockState*))SettingsHashNode_get,
    .set = (bool (*)(void*, int32, strref, stvar, SSDLockState*))SettingsHashNode_set,
    .setC = (bool (*)(void*, int32, strref, stvar*, SSDLockState*))SettingsHashNode_setC,
    .remove = (bool (*)(void*, int32, strref, SSDLockState*))SettingsHashNode_remove,
};

static SettingsHashNode_ClassIf _impl_SettingsHashNode_SettingsHashNode_ClassIf = {
    ._size = sizeof(SettingsHashNode_ClassIf),
    ._implements = (ObjIface*)&SettingsHashNode_ClassIf_tmpl,
    .bind = (bool (*)(void*, strref, stype, void*, stgeneric, SSDLockState*))SettingsHashNode_bind,
    .checkBound = (void (*)(void*, strref, SSDLockState*))SettingsHashNode_checkBound,
    .checkAll = (void (*)(void*, SSDLockState*))SettingsHashNode_checkAll,
    .unbindAll = (void (*)(void*, SSDLockState*))SettingsHashNode_unbindAll,
    .get = (bool (*)(void*, int32, strref, stvar*, SSDLockState*))SettingsHashNode_get,
    .set = (bool (*)(void*, int32, strref, stvar, SSDLockState*))SettingsHashNode_set,
    .setC = (bool (*)(void*, int32, strref, stvar*, SSDLockState*))SettingsHashNode_setC,
    .remove = (bool (*)(void*, int32, strref, SSDLockState*))SettingsHashNode_remove,
};

static ObjIface* _ifimpl_SettingsHashNode[] = {
    (ObjIface*)&_impl_SettingsHashNode_SSDNodeIf,
    (ObjIface*)&_impl_SettingsHashNode_SettingsHashNode_ClassIf,
    NULL
};

ObjClassInfo SettingsHashNode_clsinfo = {
    .instsize = sizeof(SettingsHashNode),
    .classif = (ObjIface*)&SettingsHashNode_ClassIf_tmpl,
    .parent = &SSDHashNode_clsinfo,
    .init = (bool(*)(void*))SettingsHashNode_init,
    .destroy = (void(*)(void*))SettingsHashNode_destroy,
    .ifimpl = _ifimpl_SettingsHashNode,
};

// ==================== Auto-generated section ends ======================
