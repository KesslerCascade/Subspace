// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
ControlServer_ClassIf ControlServer_ClassIf_tmpl = {
    ._size = sizeof(ControlServer_ClassIf),
};

static ControlServer_ClassIf _impl_ControlServer_ControlServer_ClassIf = {
    ._size = sizeof(ControlServer_ClassIf),
    ._implements = (ObjIface*)&ControlServer_ClassIf_tmpl,
    .start = (bool (*)(void*))ControlServer_start,
    .stop = (void (*)(void*))ControlServer_stop,
    .notify = (void (*)(void*))ControlServer_notify,
    .registerHandler = (bool (*)(void*, strref, controlservercb_t, bool, TaskResource*))ControlServer_registerHandler,
    .getHandler = (ControlHandler* (*)(void*, strref))ControlServer_getHandler,
    .port = (int (*)(void*))ControlServer_port,
};

static ObjIface* _ifimpl_ControlServer[] = {
    (ObjIface*)&_impl_ControlServer_ControlServer_ClassIf,
    NULL
};

ObjClassInfo ControlServer_clsinfo = {
    .instsize = sizeof(ControlServer),
    .classif = (ObjIface*)&ControlServer_ClassIf_tmpl,
    .init = (bool(*)(void*))ControlServer_init,
    .destroy = (void(*)(void*))ControlServer_destroy,
    .ifimpl = _ifimpl_ControlServer,
};

static ObjIface* _ifimpl_ControlHandler[] = {
    NULL
};

ObjClassInfo ControlHandler_clsinfo = {
    .instsize = sizeof(ControlHandler),
    .destroy = (void(*)(void*))ControlHandler_destroy,
    .ifimpl = _ifimpl_ControlHandler,
};

// ==================== Auto-generated section ends ======================
