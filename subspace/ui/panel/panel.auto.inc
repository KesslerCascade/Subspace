// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
Panel_ClassIf Panel_ClassIf_tmpl = {
    ._size = sizeof(Panel_ClassIf),
};

static Sortable _impl_Panel_Sortable = {
    ._size = sizeof(Sortable),
    ._implements = (ObjIface*)&Sortable_tmpl,
    .cmp = (intptr (*)(void*, void*, uint32))Panel_cmp,
};

static Panel_ClassIf _impl_Panel_Panel_ClassIf = {
    ._size = sizeof(Panel_ClassIf),
    ._implements = (ObjIface*)&Panel_ClassIf_tmpl,
    .make = (bool (*)(void*))Panel_make,
    .update = (bool (*)(void*))Panel_update,
    .cmp = (intptr (*)(void*, void*, uint32))Panel_cmp,
};

static ObjIface* _ifimpl_Panel[] = {
    (ObjIface*)&_impl_Panel_Sortable,
    (ObjIface*)&_impl_Panel_Panel_ClassIf,
    NULL
};

ObjClassInfo Panel_clsinfo = {
    .instsize = sizeof(Panel),
    .classif = (ObjIface*)&Panel_ClassIf_tmpl,
    .destroy = (void(*)(void*))Panel_destroy,
    .ifimpl = _ifimpl_Panel,
};

// ==================== Auto-generated section ends ======================
