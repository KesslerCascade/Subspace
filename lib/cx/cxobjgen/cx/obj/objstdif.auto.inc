// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
Sortable Sortable_tmpl = {
    ._size = sizeof(Sortable),
};

Hashable Hashable_tmpl = {
    ._size = sizeof(Hashable),
};

Convertible Convertible_tmpl = {
    ._size = sizeof(Convertible),
};

IteratorIf IteratorIf_tmpl = {
    ._size = sizeof(IteratorIf),
};

Iterable Iterable_tmpl = {
    ._size = sizeof(Iterable),
};

Iterator_ClassIf Iterator_ClassIf_tmpl = {
    ._size = sizeof(Iterator_ClassIf),
};

static IteratorIf _impl_Iterator_IteratorIf = {
    ._size = sizeof(IteratorIf),
    ._implements = (ObjIface*)&IteratorIf_tmpl,
};

static Iterator_ClassIf _impl_Iterator_Iterator_ClassIf = {
    ._size = sizeof(Iterator_ClassIf),
    ._implements = (ObjIface*)&Iterator_ClassIf_tmpl,
};

static ObjIface* _ifimpl_Iterator[] = {
    (ObjIface*)&_impl_Iterator_IteratorIf,
    (ObjIface*)&_impl_Iterator_Iterator_ClassIf,
    NULL
};

ObjClassInfo Iterator_clsinfo = {
    .instsize = sizeof(Iterator),
    .classif = (ObjIface*)&Iterator_ClassIf_tmpl,
    ._abstract = true,
    .ifimpl = _ifimpl_Iterator,
};

// ==================== Auto-generated section ends ======================
