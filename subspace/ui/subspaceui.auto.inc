// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
SubspaceUI_ClassIf SubspaceUI_ClassIf_tmpl = {
    ._size = sizeof(SubspaceUI_ClassIf),
};

UINotifyDispatch_ClassIf UINotifyDispatch_ClassIf_tmpl = {
    ._size = sizeof(UINotifyDispatch_ClassIf),
    ._parent = (ObjIface*)&ComplexTask_ClassIf_tmpl,
};

static SubspaceUI_ClassIf _impl_SubspaceUI_SubspaceUI_ClassIf = {
    ._size = sizeof(SubspaceUI_ClassIf),
    ._implements = (ObjIface*)&SubspaceUI_ClassIf_tmpl,
    .initialize = (bool (*)(void*))SubspaceUI_initialize,
    .shutdown = (bool (*)(void*))SubspaceUI_shutdown,
    .start = (void (*)(void*))SubspaceUI_start,
    .stop = (void (*)(void*))SubspaceUI_stop,
    .update = (void (*)(void*))SubspaceUI_update,
    .updateMain = (void (*)(void*, strref))SubspaceUI_updateMain,
    .updateSettings = (void (*)(void*, strref))SubspaceUI_updateSettings,
    .listen = (bool (*)(void*, ObjInst*, uiNotifyCallback, strref))SubspaceUI_listen,
};

static ObjIface* _ifimpl_SubspaceUI[] = {
    (ObjIface*)&_impl_SubspaceUI_SubspaceUI_ClassIf,
    NULL
};

ObjClassInfo SubspaceUI_clsinfo = {
    .instsize = sizeof(SubspaceUI),
    .classif = (ObjIface*)&SubspaceUI_ClassIf_tmpl,
    .init = (bool(*)(void*))SubspaceUI_init,
    .destroy = (void(*)(void*))SubspaceUI_destroy,
    .ifimpl = _ifimpl_SubspaceUI,
};

static UINotifyDispatch_ClassIf _impl_UINotifyDispatch_UINotifyDispatch_ClassIf = {
    ._size = sizeof(UINotifyDispatch_ClassIf),
    ._implements = (ObjIface*)&UINotifyDispatch_ClassIf_tmpl,
    .run = (uint32 (*)(void*, TaskQueue*, TQWorker*, TaskControl*))UINotifyDispatch_run,
};

static ObjIface* _ifimpl_UINotifyDispatch[] = {
    (ObjIface*)&_impl_UINotifyDispatch_UINotifyDispatch_ClassIf,
    NULL
};

ObjClassInfo UINotifyDispatch_clsinfo = {
    .instsize = sizeof(UINotifyDispatch),
    .classif = (ObjIface*)&UINotifyDispatch_ClassIf_tmpl,
    .parent = &ComplexTask_clsinfo,
    .destroy = (void(*)(void*))UINotifyDispatch_destroy,
    .ifimpl = _ifimpl_UINotifyDispatch,
};

// ==================== Auto-generated section ends ======================
