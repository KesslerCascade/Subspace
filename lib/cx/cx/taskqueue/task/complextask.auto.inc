// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
ComplexTask_ClassIf ComplexTask_ClassIf_tmpl = {
    ._size = sizeof(ComplexTask_ClassIf),
    ._parent = (ObjIface*)&Task_ClassIf_tmpl,
};

static Sortable _impl_ComplexTask_Sortable = {
    ._size = sizeof(Sortable),
    ._implements = (ObjIface*)&Sortable_tmpl,
    .cmp = (intptr (*)(void*, void*, uint32))ComplexTask_cmp,
};

static Hashable _impl_ComplexTask_Hashable = {
    ._size = sizeof(Hashable),
    ._implements = (ObjIface*)&Hashable_tmpl,
    .hash = (uint32 (*)(void*, uint32))ComplexTask_hash,
};

static ComplexTask_ClassIf _impl_ComplexTask_ComplexTask_ClassIf = {
    ._size = sizeof(ComplexTask_ClassIf),
    ._implements = (ObjIface*)&ComplexTask_ClassIf_tmpl,
    .cancel = (bool (*)(void*))ComplexTask_cancel,
    .reset = (bool (*)(void*))ComplexTask_reset,
    .cmp = (intptr (*)(void*, void*, uint32))ComplexTask_cmp,
    .hash = (uint32 (*)(void*, uint32))ComplexTask_hash,
};

static ObjIface* _ifimpl_ComplexTask[] = {
    (ObjIface*)&_impl_ComplexTask_Sortable,
    (ObjIface*)&_impl_ComplexTask_Hashable,
    (ObjIface*)&_impl_ComplexTask_ComplexTask_ClassIf,
    NULL
};

ObjClassInfo ComplexTask_clsinfo = {
    .instsize = sizeof(ComplexTask),
    .classif = (ObjIface*)&ComplexTask_ClassIf_tmpl,
    .parent = &Task_clsinfo,
    .destroy = (void(*)(void*))ComplexTask_destroy,
    ._abstract = true,
    .ifimpl = _ifimpl_ComplexTask,
};

// ==================== Auto-generated section ends ======================
