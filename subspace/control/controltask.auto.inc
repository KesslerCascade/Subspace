// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
ControlTask_ClassIf ControlTask_ClassIf_tmpl = {
    ._size = sizeof(ControlTask_ClassIf),
    ._parent = (ObjIface*)&ComplexTask_ClassIf_tmpl,
};

static Sortable _impl_ControlTask_Sortable = {
    ._size = sizeof(Sortable),
    ._implements = (ObjIface*)&Sortable_tmpl,
};

static Hashable _impl_ControlTask_Hashable = {
    ._size = sizeof(Hashable),
    ._implements = (ObjIface*)&Hashable_tmpl,
};

static ControlTask_ClassIf _impl_ControlTask_ControlTask_ClassIf = {
    ._size = sizeof(ControlTask_ClassIf),
    ._implements = (ObjIface*)&ControlTask_ClassIf_tmpl,
};

static ObjIface* _ifimpl_ControlTask[] = {
    (ObjIface*)&_impl_ControlTask_Sortable,
    (ObjIface*)&_impl_ControlTask_Hashable,
    (ObjIface*)&_impl_ControlTask_ControlTask_ClassIf,
    NULL
};

ObjClassInfo ControlTask_clsinfo = {
    .instsize = sizeof(ControlTask),
    .classif = (ObjIface*)&ControlTask_ClassIf_tmpl,
    .parent = &ComplexTask_clsinfo,
    .destroy = (void(*)(void*))ControlTask_destroy,
    ._abstract = true,
    .ifimpl = _ifimpl_ControlTask,
};

// ==================== Auto-generated section ends ======================
