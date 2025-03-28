// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
TQMonitor_ClassIf TQMonitor_ClassIf_tmpl = {
    ._size = sizeof(TQMonitor_ClassIf),
};

static TQMonitor_ClassIf _impl_TQMonitor_TQMonitor_ClassIf = {
    ._size = sizeof(TQMonitor_ClassIf),
    ._implements = (ObjIface*)&TQMonitor_ClassIf_tmpl,
};

static ObjIface* _ifimpl_TQMonitor[] = {
    (ObjIface*)&_impl_TQMonitor_TQMonitor_ClassIf,
    NULL
};

ObjClassInfo TQMonitor_clsinfo = {
    .instsize = sizeof(TQMonitor),
    .classif = (ObjIface*)&TQMonitor_ClassIf_tmpl,
    .destroy = (void(*)(void*))TQMonitor_destroy,
    ._abstract = true,
    .ifimpl = _ifimpl_TQMonitor,
};

// ==================== Auto-generated section ends ======================
