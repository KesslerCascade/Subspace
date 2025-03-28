// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
SSDSingleNode_ClassIf SSDSingleNode_ClassIf_tmpl = {
    ._size = sizeof(SSDSingleNode_ClassIf),
    ._parent = (ObjIface*)&SSDNode_ClassIf_tmpl,
};

SSDSingleIter_ClassIf SSDSingleIter_ClassIf_tmpl = {
    ._size = sizeof(SSDSingleIter_ClassIf),
    ._parent = (ObjIface*)&SSDIterator_ClassIf_tmpl,
};

static SSDNodeIf _impl_SSDSingleNode_SSDNodeIf = {
    ._size = sizeof(SSDNodeIf),
    ._implements = (ObjIface*)&SSDNodeIf_tmpl,
    .get = (bool (*)(void*, int32, strref, stvar*, SSDLockState*))SSDSingleNode_get,
    .ptr = (stvar* (*)(void*, int32, strref, SSDLockState*))SSDSingleNode_ptr,
    .set = (bool (*)(void*, int32, strref, stvar, SSDLockState*))SSDSingleNode_set,
    .setC = (bool (*)(void*, int32, strref, stvar*, SSDLockState*))SSDSingleNode_setC,
    .remove = (bool (*)(void*, int32, strref, SSDLockState*))SSDSingleNode_remove,
    .count = (int32 (*)(void*, SSDLockState*))SSDSingleNode_count,
    .iter = (SSDIterator* (*)(void*))SSDSingleNode_iter,
    ._iterLocked = (SSDIterator* (*)(void*, SSDLockState*))SSDSingleNode__iterLocked,
};

static Iterable _impl_SSDSingleNode_Iterable = {
    ._size = sizeof(Iterable),
    ._implements = (ObjIface*)&Iterable_tmpl,
    .iter = (Iterator* (*)(void*))SSDSingleNode_iter,
};

static SSDSingleNode_ClassIf _impl_SSDSingleNode_SSDSingleNode_ClassIf = {
    ._size = sizeof(SSDSingleNode_ClassIf),
    ._implements = (ObjIface*)&SSDSingleNode_ClassIf_tmpl,
    .get = (bool (*)(void*, int32, strref, stvar*, SSDLockState*))SSDSingleNode_get,
    .ptr = (stvar* (*)(void*, int32, strref, SSDLockState*))SSDSingleNode_ptr,
    .set = (bool (*)(void*, int32, strref, stvar, SSDLockState*))SSDSingleNode_set,
    .setC = (bool (*)(void*, int32, strref, stvar*, SSDLockState*))SSDSingleNode_setC,
    .remove = (bool (*)(void*, int32, strref, SSDLockState*))SSDSingleNode_remove,
    .count = (int32 (*)(void*, SSDLockState*))SSDSingleNode_count,
    .iter = (SSDIterator* (*)(void*))SSDSingleNode_iter,
    ._iterLocked = (SSDIterator* (*)(void*, SSDLockState*))SSDSingleNode__iterLocked,
};

static ObjIface* _ifimpl_SSDSingleNode[] = {
    (ObjIface*)&_impl_SSDSingleNode_SSDNodeIf,
    (ObjIface*)&_impl_SSDSingleNode_Iterable,
    (ObjIface*)&_impl_SSDSingleNode_SSDSingleNode_ClassIf,
    NULL
};

ObjClassInfo SSDSingleNode_clsinfo = {
    .instsize = sizeof(SSDSingleNode),
    .classif = (ObjIface*)&SSDSingleNode_ClassIf_tmpl,
    .parent = &SSDNode_clsinfo,
    .destroy = (void(*)(void*))SSDSingleNode_destroy,
    .ifimpl = _ifimpl_SSDSingleNode,
};

static SSDIteratorIf _impl_SSDSingleIter_SSDIteratorIf = {
    ._size = sizeof(SSDIteratorIf),
    ._implements = (ObjIface*)&SSDIteratorIf_tmpl,
    .valid = (bool (*)(void*))SSDSingleIter_valid,
    .next = (bool (*)(void*))SSDSingleIter_next,
    .get = (bool (*)(void*, stvar*))SSDSingleIter_get,
    .ptr = (stvar* (*)(void*))SSDSingleIter_ptr,
    .name = (strref (*)(void*))SSDSingleIter_name,
    .idx = (int32 (*)(void*))SSDSingleIter_idx,
    .iterOut = (bool (*)(void*, int32*, strref*, stvar**))SSDSingleIter_iterOut,
};

static SSDSingleIter_ClassIf _impl_SSDSingleIter_SSDSingleIter_ClassIf = {
    ._size = sizeof(SSDSingleIter_ClassIf),
    ._implements = (ObjIface*)&SSDSingleIter_ClassIf_tmpl,
    .valid = (bool (*)(void*))SSDSingleIter_valid,
    .next = (bool (*)(void*))SSDSingleIter_next,
    .get = (bool (*)(void*, stvar*))SSDSingleIter_get,
    .ptr = (stvar* (*)(void*))SSDSingleIter_ptr,
    .name = (strref (*)(void*))SSDSingleIter_name,
    .idx = (int32 (*)(void*))SSDSingleIter_idx,
    .iterOut = (bool (*)(void*, int32*, strref*, stvar**))SSDSingleIter_iterOut,
};

static ObjIface* _ifimpl_SSDSingleIter[] = {
    (ObjIface*)&_impl_SSDSingleIter_SSDIteratorIf,
    (ObjIface*)&_impl_SSDSingleIter_SSDSingleIter_ClassIf,
    NULL
};

ObjClassInfo SSDSingleIter_clsinfo = {
    .instsize = sizeof(SSDSingleIter),
    .classif = (ObjIface*)&SSDSingleIter_ClassIf_tmpl,
    .parent = &SSDIterator_clsinfo,
    .ifimpl = _ifimpl_SSDSingleIter,
};

// ==================== Auto-generated section ends ======================
