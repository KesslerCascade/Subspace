// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
TQManager_ClassIf TQManager_ClassIf_tmpl = {
    ._size = sizeof(TQManager_ClassIf),
};

static TQManager_ClassIf _impl_TQManager_TQManager_ClassIf = {
    ._size = sizeof(TQManager_ClassIf),
    ._implements = (ObjIface*)&TQManager_ClassIf_tmpl,
    .start = (bool (*)(void*, TaskQueue*))TQManager_start,
    .stop = (bool (*)(void*))TQManager_stop,
    .notify = (void (*)(void*, bool))TQManager_notify,
    .pretask = (void (*)(void*))TQManager_pretask,
    .tick = (int64 (*)(void*))TQManager_tick,
};

static ObjIface* _ifimpl_TQManager[] = {
    (ObjIface*)&_impl_TQManager_TQManager_ClassIf,
    NULL
};

ObjClassInfo TQManager_clsinfo = {
    .instsize = sizeof(TQManager),
    .classif = (ObjIface*)&TQManager_ClassIf_tmpl,
    .destroy = (void(*)(void*))TQManager_destroy,
    ._abstract = true,
    .ifimpl = _ifimpl_TQManager,
};

// ==================== Auto-generated section ends ======================
