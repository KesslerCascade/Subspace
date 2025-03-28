// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
TQDedicatedManager_ClassIf TQDedicatedManager_ClassIf_tmpl = {
    ._size = sizeof(TQDedicatedManager_ClassIf),
    ._parent = (ObjIface*)&TQThreadPoolManager_ClassIf_tmpl,
};

static TQDedicatedManager_ClassIf _impl_TQDedicatedManager_TQDedicatedManager_ClassIf = {
    ._size = sizeof(TQDedicatedManager_ClassIf),
    ._implements = (ObjIface*)&TQDedicatedManager_ClassIf_tmpl,
    .notify = (void (*)(void*, bool))TQDedicatedManager_notify,
    .start = (bool (*)(void*, TaskQueue*))TQDedicatedManager_start,
    .stop = (bool (*)(void*))TQDedicatedManager_stop,
};

static ObjIface* _ifimpl_TQDedicatedManager[] = {
    (ObjIface*)&_impl_TQDedicatedManager_TQDedicatedManager_ClassIf,
    NULL
};

ObjClassInfo TQDedicatedManager_clsinfo = {
    .instsize = sizeof(TQDedicatedManager),
    .classif = (ObjIface*)&TQDedicatedManager_ClassIf_tmpl,
    .parent = &TQThreadPoolManager_clsinfo,
    .init = (bool(*)(void*))TQDedicatedManager_init,
    .destroy = (void(*)(void*))TQDedicatedManager_destroy,
    .ifimpl = _ifimpl_TQDedicatedManager,
};

// ==================== Auto-generated section ends ======================
