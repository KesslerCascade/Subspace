// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
GameInfoPanel_ClassIf GameInfoPanel_ClassIf_tmpl = {
    ._size = sizeof(GameInfoPanel_ClassIf),
    ._parent = (ObjIface*)&Panel_ClassIf_tmpl,
};

static GameInfoPanel_ClassIf _impl_GameInfoPanel_GameInfoPanel_ClassIf = {
    ._size = sizeof(GameInfoPanel_ClassIf),
    ._implements = (ObjIface*)&GameInfoPanel_ClassIf_tmpl,
    .make = (bool (*)(void*))GameInfoPanel_make,
    .update = (bool (*)(void*))GameInfoPanel_update,
};

static ObjIface* _ifimpl_GameInfoPanel[] = {
    (ObjIface*)&_impl_GameInfoPanel_GameInfoPanel_ClassIf,
    NULL
};

ObjClassInfo GameInfoPanel_clsinfo = {
    .instsize = sizeof(GameInfoPanel),
    .classif = (ObjIface*)&GameInfoPanel_ClassIf_tmpl,
    .parent = &Panel_clsinfo,
    .destroy = (void(*)(void*))GameInfoPanel_destroy,
    .ifimpl = _ifimpl_GameInfoPanel,
};

// ==================== Auto-generated section ends ======================
