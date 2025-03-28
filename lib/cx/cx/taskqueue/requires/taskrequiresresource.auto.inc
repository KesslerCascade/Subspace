// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
TaskRequiresResource_ClassIf TaskRequiresResource_ClassIf_tmpl = {
    ._size = sizeof(TaskRequiresResource_ClassIf),
    ._parent = (ObjIface*)&TaskRequires_ClassIf_tmpl,
};

static TaskRequiresResource_ClassIf _impl_TaskRequiresResource_TaskRequiresResource_ClassIf = {
    ._size = sizeof(TaskRequiresResource_ClassIf),
    ._implements = (ObjIface*)&TaskRequiresResource_ClassIf_tmpl,
    .state = (uint32 (*)(void*, ComplexTask*))TaskRequiresResource_state,
    .progress = (int64 (*)(void*))TaskRequiresResource_progress,
    .tryAcquire = (bool (*)(void*, ComplexTask*))TaskRequiresResource_tryAcquire,
    .release = (bool (*)(void*, ComplexTask*))TaskRequiresResource_release,
    .cancel = (void (*)(void*))TaskRequiresResource_cancel,
    .registerTask = (bool (*)(void*, ComplexTask*))TaskRequiresResource_registerTask,
};

static ObjIface* _ifimpl_TaskRequiresResource[] = {
    (ObjIface*)&_impl_TaskRequiresResource_TaskRequiresResource_ClassIf,
    NULL
};

ObjClassInfo TaskRequiresResource_clsinfo = {
    .instsize = sizeof(TaskRequiresResource),
    .classif = (ObjIface*)&TaskRequiresResource_ClassIf_tmpl,
    .parent = &TaskRequires_clsinfo,
    .destroy = (void(*)(void*))TaskRequiresResource_destroy,
    .ifimpl = _ifimpl_TaskRequiresResource,
};

// ==================== Auto-generated section ends ======================
