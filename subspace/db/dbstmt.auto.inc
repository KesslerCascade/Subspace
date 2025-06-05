// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
DbStmt_ClassIf DbStmt_ClassIf_tmpl = {
    ._size = sizeof(DbStmt_ClassIf),
};

static DbStmt_ClassIf _impl_DbStmt_DbStmt_ClassIf = {
    ._size = sizeof(DbStmt_ClassIf),
    ._implements = (ObjIface*)&DbStmt_ClassIf_tmpl,
    .bind = (bool (*)(void*, int, stvar))DbStmt_bind,
    .exec = (bool (*)(void*))DbStmt_exec,
    .reset = (void (*)(void*))DbStmt_reset,
    .clearBind = (void (*)(void*))DbStmt_clearBind,
};

static ObjIface* _ifimpl_DbStmt[] = {
    (ObjIface*)&_impl_DbStmt_DbStmt_ClassIf,
    NULL
};

ObjClassInfo DbStmt_clsinfo = {
    .instsize = sizeof(DbStmt),
    .classif = (ObjIface*)&DbStmt_ClassIf_tmpl,
    .init = (bool(*)(void*))DbStmt_init,
    .destroy = (void(*)(void*))DbStmt_destroy,
    .ifimpl = _ifimpl_DbStmt,
};

// ==================== Auto-generated section ends ======================
