// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
TRGate_ClassIf TRGate_ClassIf_tmpl = {
    ._size = sizeof(TRGate_ClassIf),
};

TaskRequiresGate_ClassIf TaskRequiresGate_ClassIf_tmpl = {
    ._size = sizeof(TaskRequiresGate_ClassIf),
    ._parent = (ObjIface*)&TaskRequires_ClassIf_tmpl,
};

static TRGate_ClassIf _impl_TRGate_TRGate_ClassIf = {
    ._size = sizeof(TRGate_ClassIf),
    ._implements = (ObjIface*)&TRGate_ClassIf_tmpl,
    .open = (bool (*)(void*))TRGate_open,
    .seal = (bool (*)(void*))TRGate_seal,
    .progress = (void (*)(void*))TRGate_progress,
    .registerTask = (bool (*)(void*, ComplexTask*))TRGate_registerTask,
};

static ObjIface* _ifimpl_TRGate[] = {
    (ObjIface*)&_impl_TRGate_TRGate_ClassIf,
    NULL
};

ObjClassInfo TRGate_clsinfo = {
    .instsize = sizeof(TRGate),
    .classif = (ObjIface*)&TRGate_ClassIf_tmpl,
    .init = (bool(*)(void*))TRGate_init,
    .destroy = (void(*)(void*))TRGate_destroy,
    .ifimpl = _ifimpl_TRGate,
};

static TaskRequiresGate_ClassIf _impl_TaskRequiresGate_TaskRequiresGate_ClassIf = {
    ._size = sizeof(TaskRequiresGate_ClassIf),
    ._implements = (ObjIface*)&TaskRequiresGate_ClassIf_tmpl,
    .state = (uint32 (*)(void*, ComplexTask*))TaskRequiresGate_state,
    .progress = (int64 (*)(void*))TaskRequiresGate_progress,
    .tryAcquire = (bool (*)(void*, ComplexTask*))TaskRequiresGate_tryAcquire,
    .release = (bool (*)(void*, ComplexTask*))TaskRequiresGate_release,
    .cancel = (void (*)(void*))TaskRequiresGate_cancel,
    .registerTask = (bool (*)(void*, ComplexTask*))TaskRequiresGate_registerTask,
};

static ObjIface* _ifimpl_TaskRequiresGate[] = {
    (ObjIface*)&_impl_TaskRequiresGate_TaskRequiresGate_ClassIf,
    NULL
};

ObjClassInfo TaskRequiresGate_clsinfo = {
    .instsize = sizeof(TaskRequiresGate),
    .classif = (ObjIface*)&TaskRequiresGate_ClassIf_tmpl,
    .parent = &TaskRequires_clsinfo,
    .destroy = (void(*)(void*))TaskRequiresGate_destroy,
    .ifimpl = _ifimpl_TaskRequiresGate,
};

// ==================== Auto-generated section ends ======================
