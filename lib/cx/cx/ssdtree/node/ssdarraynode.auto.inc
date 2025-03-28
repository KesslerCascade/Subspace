// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
SSDArrayNode_ClassIf SSDArrayNode_ClassIf_tmpl = {
    ._size = sizeof(SSDArrayNode_ClassIf),
    ._parent = (ObjIface*)&SSDNode_ClassIf_tmpl,
};

SSDArrayIter_ClassIf SSDArrayIter_ClassIf_tmpl = {
    ._size = sizeof(SSDArrayIter_ClassIf),
    ._parent = (ObjIface*)&SSDIterator_ClassIf_tmpl,
};

static SSDNodeIf _impl_SSDArrayNode_SSDNodeIf = {
    ._size = sizeof(SSDNodeIf),
    ._implements = (ObjIface*)&SSDNodeIf_tmpl,
    .get = (bool (*)(void*, int32, strref, stvar*, SSDLockState*))SSDArrayNode_get,
    .ptr = (stvar* (*)(void*, int32, strref, SSDLockState*))SSDArrayNode_ptr,
    .set = (bool (*)(void*, int32, strref, stvar, SSDLockState*))SSDArrayNode_set,
    .setC = (bool (*)(void*, int32, strref, stvar*, SSDLockState*))SSDArrayNode_setC,
    .remove = (bool (*)(void*, int32, strref, SSDLockState*))SSDArrayNode_remove,
    .count = (int32 (*)(void*, SSDLockState*))SSDArrayNode_count,
    .iter = (SSDIterator* (*)(void*))SSDArrayNode_iter,
    ._iterLocked = (SSDIterator* (*)(void*, SSDLockState*))SSDArrayNode__iterLocked,
};

static Iterable _impl_SSDArrayNode_Iterable = {
    ._size = sizeof(Iterable),
    ._implements = (ObjIface*)&Iterable_tmpl,
    .iter = (Iterator* (*)(void*))SSDArrayNode_iter,
};

static SSDArrayNode_ClassIf _impl_SSDArrayNode_SSDArrayNode_ClassIf = {
    ._size = sizeof(SSDArrayNode_ClassIf),
    ._implements = (ObjIface*)&SSDArrayNode_ClassIf_tmpl,
    .append = (bool (*)(void*, stvar, SSDLockState*))SSDArrayNode_append,
    .isArray = (bool (*)(void*))SSDArrayNode_isArray,
    .get = (bool (*)(void*, int32, strref, stvar*, SSDLockState*))SSDArrayNode_get,
    .ptr = (stvar* (*)(void*, int32, strref, SSDLockState*))SSDArrayNode_ptr,
    .set = (bool (*)(void*, int32, strref, stvar, SSDLockState*))SSDArrayNode_set,
    .setC = (bool (*)(void*, int32, strref, stvar*, SSDLockState*))SSDArrayNode_setC,
    .remove = (bool (*)(void*, int32, strref, SSDLockState*))SSDArrayNode_remove,
    .count = (int32 (*)(void*, SSDLockState*))SSDArrayNode_count,
    .iter = (SSDIterator* (*)(void*))SSDArrayNode_iter,
    ._iterLocked = (SSDIterator* (*)(void*, SSDLockState*))SSDArrayNode__iterLocked,
};

static ObjIface* _ifimpl_SSDArrayNode[] = {
    (ObjIface*)&_impl_SSDArrayNode_SSDNodeIf,
    (ObjIface*)&_impl_SSDArrayNode_Iterable,
    (ObjIface*)&_impl_SSDArrayNode_SSDArrayNode_ClassIf,
    NULL
};

ObjClassInfo SSDArrayNode_clsinfo = {
    .instsize = sizeof(SSDArrayNode),
    .classif = (ObjIface*)&SSDArrayNode_ClassIf_tmpl,
    .parent = &SSDNode_clsinfo,
    .init = (bool(*)(void*))SSDArrayNode_init,
    .destroy = (void(*)(void*))SSDArrayNode_destroy,
    .ifimpl = _ifimpl_SSDArrayNode,
};

static SSDIteratorIf _impl_SSDArrayIter_SSDIteratorIf = {
    ._size = sizeof(SSDIteratorIf),
    ._implements = (ObjIface*)&SSDIteratorIf_tmpl,
    .valid = (bool (*)(void*))SSDArrayIter_valid,
    .next = (bool (*)(void*))SSDArrayIter_next,
    .get = (bool (*)(void*, stvar*))SSDArrayIter_get,
    .ptr = (stvar* (*)(void*))SSDArrayIter_ptr,
    .name = (strref (*)(void*))SSDArrayIter_name,
    .idx = (int32 (*)(void*))SSDArrayIter_idx,
    .iterOut = (bool (*)(void*, int32*, strref*, stvar**))SSDArrayIter_iterOut,
};

static SSDArrayIter_ClassIf _impl_SSDArrayIter_SSDArrayIter_ClassIf = {
    ._size = sizeof(SSDArrayIter_ClassIf),
    ._implements = (ObjIface*)&SSDArrayIter_ClassIf_tmpl,
    .isArray = (bool (*)(void*))SSDArrayIter_isArray,
    .valid = (bool (*)(void*))SSDArrayIter_valid,
    .next = (bool (*)(void*))SSDArrayIter_next,
    .get = (bool (*)(void*, stvar*))SSDArrayIter_get,
    .ptr = (stvar* (*)(void*))SSDArrayIter_ptr,
    .name = (strref (*)(void*))SSDArrayIter_name,
    .idx = (int32 (*)(void*))SSDArrayIter_idx,
    .iterOut = (bool (*)(void*, int32*, strref*, stvar**))SSDArrayIter_iterOut,
};

static ObjIface* _ifimpl_SSDArrayIter[] = {
    (ObjIface*)&_impl_SSDArrayIter_SSDIteratorIf,
    (ObjIface*)&_impl_SSDArrayIter_SSDArrayIter_ClassIf,
    NULL
};

ObjClassInfo SSDArrayIter_clsinfo = {
    .instsize = sizeof(SSDArrayIter),
    .classif = (ObjIface*)&SSDArrayIter_ClassIf_tmpl,
    .parent = &SSDIterator_clsinfo,
    .destroy = (void(*)(void*))SSDArrayIter_destroy,
    .ifimpl = _ifimpl_SSDArrayIter,
};

// ==================== Auto-generated section ends ======================
