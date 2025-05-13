// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
KeyBind_ClassIf KeyBind_ClassIf_tmpl = {
    ._size = sizeof(KeyBind_ClassIf),
};

static KeyBind_ClassIf _impl_KeyBind_KeyBind_ClassIf = {
    ._size = sizeof(KeyBind_ClassIf),
    ._implements = (ObjIface*)&KeyBind_ClassIf_tmpl,
    .send = (void (*)(void*, ControlClient*))KeyBind_send,
};

static ObjIface* _ifimpl_KeyBind[] = {
    (ObjIface*)&_impl_KeyBind_KeyBind_ClassIf,
    NULL
};

ObjClassInfo KeyBind_clsinfo = {
    .instsize = sizeof(KeyBind),
    .classif = (ObjIface*)&KeyBind_ClassIf_tmpl,
    .destroy = (void(*)(void*))KeyBind_destroy,
    .ifimpl = _ifimpl_KeyBind,
};

// ==================== Auto-generated section ends ======================
