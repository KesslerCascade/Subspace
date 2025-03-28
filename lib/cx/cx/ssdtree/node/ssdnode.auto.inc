// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
SSDIteratorIf SSDIteratorIf_tmpl = {
    ._size = sizeof(SSDIteratorIf),
    ._parent = (ObjIface*)&IteratorIf_tmpl,
};

SSDNodeIf SSDNodeIf_tmpl = {
    ._size = sizeof(SSDNodeIf),
};

SSDIterator_ClassIf SSDIterator_ClassIf_tmpl = {
    ._size = sizeof(SSDIterator_ClassIf),
};

SSDNode_ClassIf SSDNode_ClassIf_tmpl = {
    ._size = sizeof(SSDNode_ClassIf),
};

static SSDIteratorIf _impl_SSDIterator_SSDIteratorIf = {
    ._size = sizeof(SSDIteratorIf),
    ._implements = (ObjIface*)&SSDIteratorIf_tmpl,
};

static SSDIterator_ClassIf _impl_SSDIterator_SSDIterator_ClassIf = {
    ._size = sizeof(SSDIterator_ClassIf),
    ._implements = (ObjIface*)&SSDIterator_ClassIf_tmpl,
    .isHashtable = (bool (*)(void*))SSDIterator_isHashtable,
    .isArray = (bool (*)(void*))SSDIterator_isArray,
};

static ObjIface* _ifimpl_SSDIterator[] = {
    (ObjIface*)&_impl_SSDIterator_SSDIteratorIf,
    (ObjIface*)&_impl_SSDIterator_SSDIterator_ClassIf,
    NULL
};

ObjClassInfo SSDIterator_clsinfo = {
    .instsize = sizeof(SSDIterator),
    .classif = (ObjIface*)&SSDIterator_ClassIf_tmpl,
    .init = (bool(*)(void*))SSDIterator_init,
    .destroy = (void(*)(void*))SSDIterator_destroy,
    ._abstract = true,
    .ifimpl = _ifimpl_SSDIterator,
};

static SSDNodeIf _impl_SSDNode_SSDNodeIf = {
    ._size = sizeof(SSDNodeIf),
    ._implements = (ObjIface*)&SSDNodeIf_tmpl,
};

static Iterable _impl_SSDNode_Iterable = {
    ._size = sizeof(Iterable),
    ._implements = (ObjIface*)&Iterable_tmpl,
};

static SSDNode_ClassIf _impl_SSDNode_SSDNode_ClassIf = {
    ._size = sizeof(SSDNode_ClassIf),
    ._implements = (ObjIface*)&SSDNode_ClassIf_tmpl,
    .isHashtable = (bool (*)(void*))SSDNode_isHashtable,
    .isArray = (bool (*)(void*))SSDNode_isArray,
};

static ObjIface* _ifimpl_SSDNode[] = {
    (ObjIface*)&_impl_SSDNode_SSDNodeIf,
    (ObjIface*)&_impl_SSDNode_Iterable,
    (ObjIface*)&_impl_SSDNode_SSDNode_ClassIf,
    NULL
};

ObjClassInfo SSDNode_clsinfo = {
    .instsize = sizeof(SSDNode),
    .classif = (ObjIface*)&SSDNode_ClassIf_tmpl,
    .destroy = (void(*)(void*))SSDNode_destroy,
    ._abstract = true,
    .ifimpl = _ifimpl_SSDNode,
};

// ==================== Auto-generated section ends ======================
