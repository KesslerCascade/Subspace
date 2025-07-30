// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
SaveManager_ClassIf SaveManager_ClassIf_tmpl = {
    ._size = sizeof(SaveManager_ClassIf),
    ._parent = (ObjIface*)&SubspaceFeature_ClassIf_tmpl,
};

SaveManagerCopy_ClassIf SaveManagerCopy_ClassIf_tmpl = {
    ._size = sizeof(SaveManagerCopy_ClassIf),
    ._parent = (ObjIface*)&ComplexTask_ClassIf_tmpl,
};

static SaveManager_ClassIf _impl_SaveManager_SaveManager_ClassIf = {
    ._size = sizeof(SaveManager_ClassIf),
    ._implements = (ObjIface*)&SaveManager_ClassIf_tmpl,
    .autoSave = (bool (*)(void*, RunInfo*, strref))SaveManager_autoSave,
    .createSettingsPage = (SettingsPage* (*)(void*, SubspaceUI*))SaveManager_createSettingsPage,
    .applyDefaultSettings = (void (*)(void*))SaveManager_applyDefaultSettings,
};

static ObjIface* _ifimpl_SaveManager[] = {
    (ObjIface*)&_impl_SaveManager_SaveManager_ClassIf,
    NULL
};

ObjClassInfo SaveManager_clsinfo = {
    .instsize = sizeof(SaveManager),
    .classif = (ObjIface*)&SaveManager_ClassIf_tmpl,
    .parent = &SubspaceFeature_clsinfo,
    .ifimpl = _ifimpl_SaveManager,
};

static SaveManagerCopy_ClassIf _impl_SaveManagerCopy_SaveManagerCopy_ClassIf = {
    ._size = sizeof(SaveManagerCopy_ClassIf),
    ._implements = (ObjIface*)&SaveManagerCopy_ClassIf_tmpl,
    .run = (uint32 (*)(void*, TaskQueue*, TQWorker*, TaskControl*))SaveManagerCopy_run,
};

static ObjIface* _ifimpl_SaveManagerCopy[] = {
    (ObjIface*)&_impl_SaveManagerCopy_SaveManagerCopy_ClassIf,
    NULL
};

ObjClassInfo SaveManagerCopy_clsinfo = {
    .instsize = sizeof(SaveManagerCopy),
    .classif = (ObjIface*)&SaveManagerCopy_ClassIf_tmpl,
    .parent = &ComplexTask_clsinfo,
    .destroy = (void(*)(void*))SaveManagerCopy_destroy,
    .ifimpl = _ifimpl_SaveManagerCopy,
};

// ==================== Auto-generated section ends ======================
