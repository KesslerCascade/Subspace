// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
NotableEventPanel_ClassIf NotableEventPanel_ClassIf_tmpl = {
    ._size = sizeof(NotableEventPanel_ClassIf),
    ._parent = (ObjIface*)&Panel_ClassIf_tmpl,
};

static NotableEventPanel_ClassIf _impl_NotableEventPanel_NotableEventPanel_ClassIf = {
    ._size = sizeof(NotableEventPanel_ClassIf),
    ._implements = (ObjIface*)&NotableEventPanel_ClassIf_tmpl,
    .make = (bool (*)(void*))NotableEventPanel_make,
};

static ObjIface* _ifimpl_NotableEventPanel[] = {
    (ObjIface*)&_impl_NotableEventPanel_NotableEventPanel_ClassIf,
    NULL
};

ObjClassInfo NotableEventPanel_clsinfo = {
    .instsize = sizeof(NotableEventPanel),
    .classif = (ObjIface*)&NotableEventPanel_ClassIf_tmpl,
    .parent = &Panel_clsinfo,
    .ifimpl = _ifimpl_NotableEventPanel,
};

// ==================== Auto-generated section ends ======================
