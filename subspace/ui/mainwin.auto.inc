// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
MainWin_ClassIf MainWin_ClassIf_tmpl = {
    ._size = sizeof(MainWin_ClassIf),
};

static MainWin_ClassIf _impl_MainWin_MainWin_ClassIf = {
    ._size = sizeof(MainWin_ClassIf),
    ._implements = (ObjIface*)&MainWin_ClassIf_tmpl,
    .make = (bool (*)(void*))MainWin_make,
    .makeMenu = (void (*)(void*))MainWin_makeMenu,
    .show = (void (*)(void*))MainWin_show,
    .showMenu = (void (*)(void*, int, int))MainWin_showMenu,
    .showLayoutMenu = (void (*)(void*, Ihandle*, Ihandle*, int, int))MainWin_showLayoutMenu,
    .update = (void (*)(void*))MainWin_update,
    .updatePanel = (bool (*)(void*, strref))MainWin_updatePanel,
    .updateAll = (void (*)(void*))MainWin_updateAll,
    .finish = (void (*)(void*))MainWin_finish,
    .setLayoutDirty = (void (*)(void*))MainWin_setLayoutDirty,
};

static ObjIface* _ifimpl_MainWin[] = {
    (ObjIface*)&_impl_MainWin_MainWin_ClassIf,
    NULL
};

ObjClassInfo MainWin_clsinfo = {
    .instsize = sizeof(MainWin),
    .classif = (ObjIface*)&MainWin_ClassIf_tmpl,
    .init = (bool(*)(void*))MainWin_init,
    .destroy = (void(*)(void*))MainWin_destroy,
    .ifimpl = _ifimpl_MainWin,
};

// ==================== Auto-generated section ends ======================
