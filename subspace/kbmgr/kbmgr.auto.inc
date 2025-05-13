// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
KBMgr_ClassIf KBMgr_ClassIf_tmpl = {
    ._size = sizeof(KBMgr_ClassIf),
};

static KBMgr_ClassIf _impl_KBMgr_KBMgr_ClassIf = {
    ._size = sizeof(KBMgr_ClassIf),
    ._implements = (ObjIface*)&KBMgr_ClassIf_tmpl,
    .bind = (bool (*)(void*, strref, int))KBMgr_bind,
    .reg = (void (*)(void*, SubspaceFeature*, strref, int))KBMgr_reg,
    .sendAll = (void (*)(void*, ControlClient*))KBMgr_sendAll,
};

static ObjIface* _ifimpl_KBMgr[] = {
    (ObjIface*)&_impl_KBMgr_KBMgr_ClassIf,
    NULL
};

ObjClassInfo KBMgr_clsinfo = {
    .instsize = sizeof(KBMgr),
    .classif = (ObjIface*)&KBMgr_ClassIf_tmpl,
    .init = (bool(*)(void*))KBMgr_init,
    .destroy = (void(*)(void*))KBMgr_destroy,
    .ifimpl = _ifimpl_KBMgr,
};

// ==================== Auto-generated section ends ======================
