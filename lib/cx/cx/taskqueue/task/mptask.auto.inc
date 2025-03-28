// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
MultiphaseTask_ClassIf MultiphaseTask_ClassIf_tmpl = {
    ._size = sizeof(MultiphaseTask_ClassIf),
    ._parent = (ObjIface*)&ComplexTask_ClassIf_tmpl,
};

static Sortable _impl_MultiphaseTask_Sortable = {
    ._size = sizeof(Sortable),
    ._implements = (ObjIface*)&Sortable_tmpl,
};

static Hashable _impl_MultiphaseTask_Hashable = {
    ._size = sizeof(Hashable),
    ._implements = (ObjIface*)&Hashable_tmpl,
};

static MultiphaseTask_ClassIf _impl_MultiphaseTask_MultiphaseTask_ClassIf = {
    ._size = sizeof(MultiphaseTask_ClassIf),
    ._implements = (ObjIface*)&MultiphaseTask_ClassIf_tmpl,
    .finish = (uint32 (*)(void*, uint32, TaskControl*))MultiphaseTask_finish,
    .run = (uint32 (*)(void*, TaskQueue*, TQWorker*, TaskControl*))MultiphaseTask_run,
};

static ObjIface* _ifimpl_MultiphaseTask[] = {
    (ObjIface*)&_impl_MultiphaseTask_Sortable,
    (ObjIface*)&_impl_MultiphaseTask_Hashable,
    (ObjIface*)&_impl_MultiphaseTask_MultiphaseTask_ClassIf,
    NULL
};

ObjClassInfo MultiphaseTask_clsinfo = {
    .instsize = sizeof(MultiphaseTask),
    .classif = (ObjIface*)&MultiphaseTask_ClassIf_tmpl,
    .parent = &ComplexTask_clsinfo,
    .init = (bool(*)(void*))MultiphaseTask_init,
    .destroy = (void(*)(void*))MultiphaseTask_destroy,
    ._abstract = true,
    .ifimpl = _ifimpl_MultiphaseTask,
};

// ==================== Auto-generated section ends ======================
