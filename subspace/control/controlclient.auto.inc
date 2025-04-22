// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
ControlClient_ClassIf ControlClient_ClassIf_tmpl = {
    ._size = sizeof(ControlClient_ClassIf),
};

static ControlClient_ClassIf _impl_ControlClient_ControlClient_ClassIf = {
    ._size = sizeof(ControlClient_ClassIf),
    ._implements = (ObjIface*)&ControlClient_ClassIf_tmpl,
    .queue = (void (*)(void*, ControlMsg*))ControlClient_queue,
    .send = (void (*)(void*))ControlClient_send,
    .recv = (void (*)(void*))ControlClient_recv,
};

static ObjIface* _ifimpl_ControlClient[] = {
    (ObjIface*)&_impl_ControlClient_ControlClient_ClassIf,
    NULL
};

ObjClassInfo ControlClient_clsinfo = {
    .instsize = sizeof(ControlClient),
    .classif = (ObjIface*)&ControlClient_ClassIf_tmpl,
    .init = (bool(*)(void*))ControlClient_init,
    .destroy = (void(*)(void*))ControlClient_destroy,
    .ifimpl = _ifimpl_ControlClient,
};

// ==================== Auto-generated section ends ======================
