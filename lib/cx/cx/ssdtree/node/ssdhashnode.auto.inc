// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
SSDHashNode_ClassIf SSDHashNode_ClassIf_tmpl = {
    ._size = sizeof(SSDHashNode_ClassIf),
    ._parent = (ObjIface*)&SSDNode_ClassIf_tmpl,
};

SSDHashIter_ClassIf SSDHashIter_ClassIf_tmpl = {
    ._size = sizeof(SSDHashIter_ClassIf),
    ._parent = (ObjIface*)&SSDIterator_ClassIf_tmpl,
};

static SSDNodeIf _impl_SSDHashNode_SSDNodeIf = {
    ._size = sizeof(SSDNodeIf),
    ._implements = (ObjIface*)&SSDNodeIf_tmpl,
    .get = (bool (*)(void*, int32, strref, stvar*, SSDLockState*))SSDHashNode_get,
    .ptr = (stvar* (*)(void*, int32, strref, SSDLockState*))SSDHashNode_ptr,
    .set = (bool (*)(void*, int32, strref, stvar, SSDLockState*))SSDHashNode_set,
    .setC = (bool (*)(void*, int32, strref, stvar*, SSDLockState*))SSDHashNode_setC,
    .remove = (bool (*)(void*, int32, strref, SSDLockState*))SSDHashNode_remove,
    .count = (int32 (*)(void*, SSDLockState*))SSDHashNode_count,
    .iter = (SSDIterator* (*)(void*))SSDHashNode_iter,
    ._iterLocked = (SSDIterator* (*)(void*, SSDLockState*))SSDHashNode__iterLocked,
};

static Iterable _impl_SSDHashNode_Iterable = {
    ._size = sizeof(Iterable),
    ._implements = (ObjIface*)&Iterable_tmpl,
    .iter = (Iterator* (*)(void*))SSDHashNode_iter,
};

static SSDHashNode_ClassIf _impl_SSDHashNode_SSDHashNode_ClassIf = {
    ._size = sizeof(SSDHashNode_ClassIf),
    ._implements = (ObjIface*)&SSDHashNode_ClassIf_tmpl,
    .isHashtable = (bool (*)(void*))SSDHashNode_isHashtable,
    .get = (bool (*)(void*, int32, strref, stvar*, SSDLockState*))SSDHashNode_get,
    .ptr = (stvar* (*)(void*, int32, strref, SSDLockState*))SSDHashNode_ptr,
    .set = (bool (*)(void*, int32, strref, stvar, SSDLockState*))SSDHashNode_set,
    .setC = (bool (*)(void*, int32, strref, stvar*, SSDLockState*))SSDHashNode_setC,
    .remove = (bool (*)(void*, int32, strref, SSDLockState*))SSDHashNode_remove,
    .count = (int32 (*)(void*, SSDLockState*))SSDHashNode_count,
    .iter = (SSDIterator* (*)(void*))SSDHashNode_iter,
    ._iterLocked = (SSDIterator* (*)(void*, SSDLockState*))SSDHashNode__iterLocked,
};

static ObjIface* _ifimpl_SSDHashNode[] = {
    (ObjIface*)&_impl_SSDHashNode_SSDNodeIf,
    (ObjIface*)&_impl_SSDHashNode_Iterable,
    (ObjIface*)&_impl_SSDHashNode_SSDHashNode_ClassIf,
    NULL
};

ObjClassInfo SSDHashNode_clsinfo = {
    .instsize = sizeof(SSDHashNode),
    .classif = (ObjIface*)&SSDHashNode_ClassIf_tmpl,
    .parent = &SSDNode_clsinfo,
    .init = (bool(*)(void*))SSDHashNode_init,
    .destroy = (void(*)(void*))SSDHashNode_destroy,
    .ifimpl = _ifimpl_SSDHashNode,
};

static SSDIteratorIf _impl_SSDHashIter_SSDIteratorIf = {
    ._size = sizeof(SSDIteratorIf),
    ._implements = (ObjIface*)&SSDIteratorIf_tmpl,
    .valid = (bool (*)(void*))SSDHashIter_valid,
    .next = (bool (*)(void*))SSDHashIter_next,
    .get = (bool (*)(void*, stvar*))SSDHashIter_get,
    .ptr = (stvar* (*)(void*))SSDHashIter_ptr,
    .name = (strref (*)(void*))SSDHashIter_name,
    .idx = (int32 (*)(void*))SSDHashIter_idx,
    .iterOut = (bool (*)(void*, int32*, strref*, stvar**))SSDHashIter_iterOut,
};

static SSDHashIter_ClassIf _impl_SSDHashIter_SSDHashIter_ClassIf = {
    ._size = sizeof(SSDHashIter_ClassIf),
    ._implements = (ObjIface*)&SSDHashIter_ClassIf_tmpl,
    .isHashtable = (bool (*)(void*))SSDHashIter_isHashtable,
    .valid = (bool (*)(void*))SSDHashIter_valid,
    .next = (bool (*)(void*))SSDHashIter_next,
    .get = (bool (*)(void*, stvar*))SSDHashIter_get,
    .ptr = (stvar* (*)(void*))SSDHashIter_ptr,
    .name = (strref (*)(void*))SSDHashIter_name,
    .idx = (int32 (*)(void*))SSDHashIter_idx,
    .iterOut = (bool (*)(void*, int32*, strref*, stvar**))SSDHashIter_iterOut,
};

static ObjIface* _ifimpl_SSDHashIter[] = {
    (ObjIface*)&_impl_SSDHashIter_SSDIteratorIf,
    (ObjIface*)&_impl_SSDHashIter_SSDHashIter_ClassIf,
    NULL
};

ObjClassInfo SSDHashIter_clsinfo = {
    .instsize = sizeof(SSDHashIter),
    .classif = (ObjIface*)&SSDHashIter_ClassIf_tmpl,
    .parent = &SSDIterator_clsinfo,
    .destroy = (void(*)(void*))SSDHashIter_destroy,
    .ifimpl = _ifimpl_SSDHashIter,
};

// ==================== Auto-generated section ends ======================
