// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
TQManualManager_ClassIf TQManualManager_ClassIf_tmpl = {
    ._size = sizeof(TQManualManager_ClassIf),
    ._parent = (ObjIface*)&TQManager_ClassIf_tmpl,
};

static TQManualManager_ClassIf _impl_TQManualManager_TQManualManager_ClassIf = {
    ._size = sizeof(TQManualManager_ClassIf),
    ._implements = (ObjIface*)&TQManualManager_ClassIf_tmpl,
    .notify = (void (*)(void*, bool))TQManualManager_notify,
    .tick = (int64 (*)(void*))TQManualManager_tick,
};

static ObjIface* _ifimpl_TQManualManager[] = {
    (ObjIface*)&_impl_TQManualManager_TQManualManager_ClassIf,
    NULL
};

ObjClassInfo TQManualManager_clsinfo = {
    .instsize = sizeof(TQManualManager),
    .classif = (ObjIface*)&TQManualManager_ClassIf_tmpl,
    .parent = &TQManager_clsinfo,
    .init = (bool(*)(void*))TQManualManager_init,
    .destroy = (void(*)(void*))TQManualManager_destroy,
    .ifimpl = _ifimpl_TQManualManager,
};

// ==================== Auto-generated section ends ======================
