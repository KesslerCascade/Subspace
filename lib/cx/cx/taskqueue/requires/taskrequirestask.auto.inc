// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
TaskRequiresTask_ClassIf TaskRequiresTask_ClassIf_tmpl = {
    ._size = sizeof(TaskRequiresTask_ClassIf),
    ._parent = (ObjIface*)&TaskRequires_ClassIf_tmpl,
};

static TaskRequiresTask_ClassIf _impl_TaskRequiresTask_TaskRequiresTask_ClassIf = {
    ._size = sizeof(TaskRequiresTask_ClassIf),
    ._implements = (ObjIface*)&TaskRequiresTask_ClassIf_tmpl,
    .state = (uint32 (*)(void*, ComplexTask*))TaskRequiresTask_state,
    .progress = (int64 (*)(void*))TaskRequiresTask_progress,
    .tryAcquire = (bool (*)(void*, ComplexTask*))TaskRequiresTask_tryAcquire,
    .release = (bool (*)(void*, ComplexTask*))TaskRequiresTask_release,
    .cancel = (void (*)(void*))TaskRequiresTask_cancel,
    .registerTask = (bool (*)(void*, ComplexTask*))TaskRequiresTask_registerTask,
};

static ObjIface* _ifimpl_TaskRequiresTask[] = {
    (ObjIface*)&_impl_TaskRequiresTask_TaskRequiresTask_ClassIf,
    NULL
};

ObjClassInfo TaskRequiresTask_clsinfo = {
    .instsize = sizeof(TaskRequiresTask),
    .classif = (ObjIface*)&TaskRequiresTask_ClassIf_tmpl,
    .parent = &TaskRequires_clsinfo,
    .destroy = (void(*)(void*))TaskRequiresTask_destroy,
    .ifimpl = _ifimpl_TaskRequiresTask,
};

// ==================== Auto-generated section ends ======================
