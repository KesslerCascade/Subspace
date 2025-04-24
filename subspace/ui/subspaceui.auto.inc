// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
SubspaceUI_ClassIf SubspaceUI_ClassIf_tmpl = {
    ._size = sizeof(SubspaceUI_ClassIf),
};

static SubspaceUI_ClassIf _impl_SubspaceUI_SubspaceUI_ClassIf = {
    ._size = sizeof(SubspaceUI_ClassIf),
    ._implements = (ObjIface*)&SubspaceUI_ClassIf_tmpl,
    .init = (bool (*)(void*))SubspaceUI_init,
    .shutdown = (bool (*)(void*))SubspaceUI_shutdown,
    .start = (void (*)(void*))SubspaceUI_start,
    .stop = (void (*)(void*))SubspaceUI_stop,
};

static ObjIface* _ifimpl_SubspaceUI[] = {
    (ObjIface*)&_impl_SubspaceUI_SubspaceUI_ClassIf,
    NULL
};

ObjClassInfo SubspaceUI_clsinfo = {
    .instsize = sizeof(SubspaceUI),
    .classif = (ObjIface*)&SubspaceUI_ClassIf_tmpl,
    .destroy = (void(*)(void*))SubspaceUI_destroy,
    .ifimpl = _ifimpl_SubspaceUI,
};

// ==================== Auto-generated section ends ======================
