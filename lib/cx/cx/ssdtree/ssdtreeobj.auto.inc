// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
SSDTree_ClassIf SSDTree_ClassIf_tmpl = {
    ._size = sizeof(SSDTree_ClassIf),
};

static SSDTree_ClassIf _impl_SSDTree_SSDTree_ClassIf = {
    ._size = sizeof(SSDTree_ClassIf),
    ._implements = (ObjIface*)&SSDTree_ClassIf_tmpl,
    .createNode = (SSDNode* (*)(void*, SSDCreateType))SSDTree_createNode,
};

static ObjIface* _ifimpl_SSDTree[] = {
    (ObjIface*)&_impl_SSDTree_SSDTree_ClassIf,
    NULL
};

ObjClassInfo SSDTree_clsinfo = {
    .instsize = sizeof(SSDTree),
    .classif = (ObjIface*)&SSDTree_ClassIf_tmpl,
    .init = (bool(*)(void*))SSDTree_init,
    .destroy = (void(*)(void*))SSDTree_destroy,
    .ifimpl = _ifimpl_SSDTree,
};

// ==================== Auto-generated section ends ======================
