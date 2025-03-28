// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
TQTest1_ClassIf TQTest1_ClassIf_tmpl = {
    ._size = sizeof(TQTest1_ClassIf),
    ._parent = (ObjIface*)&Task_ClassIf_tmpl,
};

TQTestFail_ClassIf TQTestFail_ClassIf_tmpl = {
    ._size = sizeof(TQTestFail_ClassIf),
    ._parent = (ObjIface*)&Task_ClassIf_tmpl,
};

TQTestCC1_ClassIf TQTestCC1_ClassIf_tmpl = {
    ._size = sizeof(TQTestCC1_ClassIf),
    ._parent = (ObjIface*)&Task_ClassIf_tmpl,
};

TQTestCC2_ClassIf TQTestCC2_ClassIf_tmpl = {
    ._size = sizeof(TQTestCC2_ClassIf),
    ._parent = (ObjIface*)&Task_ClassIf_tmpl,
};

TQTestSched_ClassIf TQTestSched_ClassIf_tmpl = {
    ._size = sizeof(TQTestSched_ClassIf),
    ._parent = (ObjIface*)&ComplexTask_ClassIf_tmpl,
};

TQTestS1_ClassIf TQTestS1_ClassIf_tmpl = {
    ._size = sizeof(TQTestS1_ClassIf),
    ._parent = (ObjIface*)&TQTestSched_ClassIf_tmpl,
};

TQTestS2_ClassIf TQTestS2_ClassIf_tmpl = {
    ._size = sizeof(TQTestS2_ClassIf),
    ._parent = (ObjIface*)&TQTestSched_ClassIf_tmpl,
};

TQDelayTest_ClassIf TQDelayTest_ClassIf_tmpl = {
    ._size = sizeof(TQDelayTest_ClassIf),
    ._parent = (ObjIface*)&Task_ClassIf_tmpl,
};

TQMTest_ClassIf TQMTest_ClassIf_tmpl = {
    ._size = sizeof(TQMTest_ClassIf),
    ._parent = (ObjIface*)&ComplexTask_ClassIf_tmpl,
};

TQRTestMtx_ClassIf TQRTestMtx_ClassIf_tmpl = {
    ._size = sizeof(TQRTestMtx_ClassIf),
    ._parent = (ObjIface*)&ComplexTask_ClassIf_tmpl,
};

TQRTestFifo_ClassIf TQRTestFifo_ClassIf_tmpl = {
    ._size = sizeof(TQRTestFifo_ClassIf),
    ._parent = (ObjIface*)&ComplexTask_ClassIf_tmpl,
};

TQRTestLifo_ClassIf TQRTestLifo_ClassIf_tmpl = {
    ._size = sizeof(TQRTestLifo_ClassIf),
    ._parent = (ObjIface*)&ComplexTask_ClassIf_tmpl,
};

TQRTestGate_ClassIf TQRTestGate_ClassIf_tmpl = {
    ._size = sizeof(TQRTestGate_ClassIf),
    ._parent = (ObjIface*)&ComplexTask_ClassIf_tmpl,
};

TQMPTest_ClassIf TQMPTest_ClassIf_tmpl = {
    ._size = sizeof(TQMPTest_ClassIf),
    ._parent = (ObjIface*)&MultiphaseTask_ClassIf_tmpl,
};

TQTimeoutTest1_ClassIf TQTimeoutTest1_ClassIf_tmpl = {
    ._size = sizeof(TQTimeoutTest1_ClassIf),
    ._parent = (ObjIface*)&ComplexTask_ClassIf_tmpl,
};

TQTimeoutTest2_ClassIf TQTimeoutTest2_ClassIf_tmpl = {
    ._size = sizeof(TQTimeoutTest2_ClassIf),
    ._parent = (ObjIface*)&ComplexTask_ClassIf_tmpl,
};

static TQTest1_ClassIf _impl_TQTest1_TQTest1_ClassIf = {
    ._size = sizeof(TQTest1_ClassIf),
    ._implements = (ObjIface*)&TQTest1_ClassIf_tmpl,
    .run = (uint32 (*)(void*, TaskQueue*, TQWorker*, TaskControl*))TQTest1_run,
};

static ObjIface* _ifimpl_TQTest1[] = {
    (ObjIface*)&_impl_TQTest1_TQTest1_ClassIf,
    NULL
};

ObjClassInfo TQTest1_clsinfo = {
    .instsize = sizeof(TQTest1),
    .classif = (ObjIface*)&TQTest1_ClassIf_tmpl,
    .parent = &Task_clsinfo,
    .ifimpl = _ifimpl_TQTest1,
};

static TQTestFail_ClassIf _impl_TQTestFail_TQTestFail_ClassIf = {
    ._size = sizeof(TQTestFail_ClassIf),
    ._implements = (ObjIface*)&TQTestFail_ClassIf_tmpl,
    .run = (uint32 (*)(void*, TaskQueue*, TQWorker*, TaskControl*))TQTestFail_run,
};

static ObjIface* _ifimpl_TQTestFail[] = {
    (ObjIface*)&_impl_TQTestFail_TQTestFail_ClassIf,
    NULL
};

ObjClassInfo TQTestFail_clsinfo = {
    .instsize = sizeof(TQTestFail),
    .classif = (ObjIface*)&TQTestFail_ClassIf_tmpl,
    .parent = &Task_clsinfo,
    .ifimpl = _ifimpl_TQTestFail,
};

static TQTestCC1_ClassIf _impl_TQTestCC1_TQTestCC1_ClassIf = {
    ._size = sizeof(TQTestCC1_ClassIf),
    ._implements = (ObjIface*)&TQTestCC1_ClassIf_tmpl,
    .run = (uint32 (*)(void*, TaskQueue*, TQWorker*, TaskControl*))TQTestCC1_run,
};

static ObjIface* _ifimpl_TQTestCC1[] = {
    (ObjIface*)&_impl_TQTestCC1_TQTestCC1_ClassIf,
    NULL
};

ObjClassInfo TQTestCC1_clsinfo = {
    .instsize = sizeof(TQTestCC1),
    .classif = (ObjIface*)&TQTestCC1_ClassIf_tmpl,
    .parent = &Task_clsinfo,
    .ifimpl = _ifimpl_TQTestCC1,
};

static TQTestCC2_ClassIf _impl_TQTestCC2_TQTestCC2_ClassIf = {
    ._size = sizeof(TQTestCC2_ClassIf),
    ._implements = (ObjIface*)&TQTestCC2_ClassIf_tmpl,
    .run = (uint32 (*)(void*, TaskQueue*, TQWorker*, TaskControl*))TQTestCC2_run,
};

static ObjIface* _ifimpl_TQTestCC2[] = {
    (ObjIface*)&_impl_TQTestCC2_TQTestCC2_ClassIf,
    NULL
};

ObjClassInfo TQTestCC2_clsinfo = {
    .instsize = sizeof(TQTestCC2),
    .classif = (ObjIface*)&TQTestCC2_ClassIf_tmpl,
    .parent = &Task_clsinfo,
    .ifimpl = _ifimpl_TQTestCC2,
};

static Sortable _impl_TQTestSched_Sortable = {
    ._size = sizeof(Sortable),
    ._implements = (ObjIface*)&Sortable_tmpl,
};

static Hashable _impl_TQTestSched_Hashable = {
    ._size = sizeof(Hashable),
    ._implements = (ObjIface*)&Hashable_tmpl,
};

static TQTestSched_ClassIf _impl_TQTestSched_TQTestSched_ClassIf = {
    ._size = sizeof(TQTestSched_ClassIf),
    ._implements = (ObjIface*)&TQTestSched_ClassIf_tmpl,
};

static ObjIface* _ifimpl_TQTestSched[] = {
    (ObjIface*)&_impl_TQTestSched_Sortable,
    (ObjIface*)&_impl_TQTestSched_Hashable,
    (ObjIface*)&_impl_TQTestSched_TQTestSched_ClassIf,
    NULL
};

ObjClassInfo TQTestSched_clsinfo = {
    .instsize = sizeof(TQTestSched),
    .classif = (ObjIface*)&TQTestSched_ClassIf_tmpl,
    .parent = &ComplexTask_clsinfo,
    ._abstract = true,
    .ifimpl = _ifimpl_TQTestSched,
};

static TQTestS1_ClassIf _impl_TQTestS1_TQTestS1_ClassIf = {
    ._size = sizeof(TQTestS1_ClassIf),
    ._implements = (ObjIface*)&TQTestS1_ClassIf_tmpl,
    .run = (uint32 (*)(void*, TaskQueue*, TQWorker*, TaskControl*))TQTestS1_run,
};

static ObjIface* _ifimpl_TQTestS1[] = {
    (ObjIface*)&_impl_TQTestS1_TQTestS1_ClassIf,
    NULL
};

ObjClassInfo TQTestS1_clsinfo = {
    .instsize = sizeof(TQTestS1),
    .classif = (ObjIface*)&TQTestS1_ClassIf_tmpl,
    .parent = &TQTestSched_clsinfo,
    .ifimpl = _ifimpl_TQTestS1,
};

static TQTestS2_ClassIf _impl_TQTestS2_TQTestS2_ClassIf = {
    ._size = sizeof(TQTestS2_ClassIf),
    ._implements = (ObjIface*)&TQTestS2_ClassIf_tmpl,
    .run = (uint32 (*)(void*, TaskQueue*, TQWorker*, TaskControl*))TQTestS2_run,
};

static ObjIface* _ifimpl_TQTestS2[] = {
    (ObjIface*)&_impl_TQTestS2_TQTestS2_ClassIf,
    NULL
};

ObjClassInfo TQTestS2_clsinfo = {
    .instsize = sizeof(TQTestS2),
    .classif = (ObjIface*)&TQTestS2_ClassIf_tmpl,
    .parent = &TQTestSched_clsinfo,
    .destroy = (void(*)(void*))TQTestS2_destroy,
    .ifimpl = _ifimpl_TQTestS2,
};

static TQDelayTest_ClassIf _impl_TQDelayTest_TQDelayTest_ClassIf = {
    ._size = sizeof(TQDelayTest_ClassIf),
    ._implements = (ObjIface*)&TQDelayTest_ClassIf_tmpl,
    .run = (uint32 (*)(void*, TaskQueue*, TQWorker*, TaskControl*))TQDelayTest_run,
};

static ObjIface* _ifimpl_TQDelayTest[] = {
    (ObjIface*)&_impl_TQDelayTest_TQDelayTest_ClassIf,
    NULL
};

ObjClassInfo TQDelayTest_clsinfo = {
    .instsize = sizeof(TQDelayTest),
    .classif = (ObjIface*)&TQDelayTest_ClassIf_tmpl,
    .parent = &Task_clsinfo,
    .ifimpl = _ifimpl_TQDelayTest,
};

static TQMTest_ClassIf _impl_TQMTest_TQMTest_ClassIf = {
    ._size = sizeof(TQMTest_ClassIf),
    ._implements = (ObjIface*)&TQMTest_ClassIf_tmpl,
    .run = (uint32 (*)(void*, TaskQueue*, TQWorker*, TaskControl*))TQMTest_run,
};

static ObjIface* _ifimpl_TQMTest[] = {
    (ObjIface*)&_impl_TQMTest_TQMTest_ClassIf,
    NULL
};

ObjClassInfo TQMTest_clsinfo = {
    .instsize = sizeof(TQMTest),
    .classif = (ObjIface*)&TQMTest_ClassIf_tmpl,
    .parent = &ComplexTask_clsinfo,
    .ifimpl = _ifimpl_TQMTest,
};

static TQRTestMtx_ClassIf _impl_TQRTestMtx_TQRTestMtx_ClassIf = {
    ._size = sizeof(TQRTestMtx_ClassIf),
    ._implements = (ObjIface*)&TQRTestMtx_ClassIf_tmpl,
    .run = (uint32 (*)(void*, TaskQueue*, TQWorker*, TaskControl*))TQRTestMtx_run,
};

static ObjIface* _ifimpl_TQRTestMtx[] = {
    (ObjIface*)&_impl_TQRTestMtx_TQRTestMtx_ClassIf,
    NULL
};

ObjClassInfo TQRTestMtx_clsinfo = {
    .instsize = sizeof(TQRTestMtx),
    .classif = (ObjIface*)&TQRTestMtx_ClassIf_tmpl,
    .parent = &ComplexTask_clsinfo,
    .ifimpl = _ifimpl_TQRTestMtx,
};

static TQRTestFifo_ClassIf _impl_TQRTestFifo_TQRTestFifo_ClassIf = {
    ._size = sizeof(TQRTestFifo_ClassIf),
    ._implements = (ObjIface*)&TQRTestFifo_ClassIf_tmpl,
    .run = (uint32 (*)(void*, TaskQueue*, TQWorker*, TaskControl*))TQRTestFifo_run,
};

static ObjIface* _ifimpl_TQRTestFifo[] = {
    (ObjIface*)&_impl_TQRTestFifo_TQRTestFifo_ClassIf,
    NULL
};

ObjClassInfo TQRTestFifo_clsinfo = {
    .instsize = sizeof(TQRTestFifo),
    .classif = (ObjIface*)&TQRTestFifo_ClassIf_tmpl,
    .parent = &ComplexTask_clsinfo,
    .ifimpl = _ifimpl_TQRTestFifo,
};

static TQRTestLifo_ClassIf _impl_TQRTestLifo_TQRTestLifo_ClassIf = {
    ._size = sizeof(TQRTestLifo_ClassIf),
    ._implements = (ObjIface*)&TQRTestLifo_ClassIf_tmpl,
    .run = (uint32 (*)(void*, TaskQueue*, TQWorker*, TaskControl*))TQRTestLifo_run,
};

static ObjIface* _ifimpl_TQRTestLifo[] = {
    (ObjIface*)&_impl_TQRTestLifo_TQRTestLifo_ClassIf,
    NULL
};

ObjClassInfo TQRTestLifo_clsinfo = {
    .instsize = sizeof(TQRTestLifo),
    .classif = (ObjIface*)&TQRTestLifo_ClassIf_tmpl,
    .parent = &ComplexTask_clsinfo,
    .ifimpl = _ifimpl_TQRTestLifo,
};

static TQRTestGate_ClassIf _impl_TQRTestGate_TQRTestGate_ClassIf = {
    ._size = sizeof(TQRTestGate_ClassIf),
    ._implements = (ObjIface*)&TQRTestGate_ClassIf_tmpl,
    .run = (uint32 (*)(void*, TaskQueue*, TQWorker*, TaskControl*))TQRTestGate_run,
};

static ObjIface* _ifimpl_TQRTestGate[] = {
    (ObjIface*)&_impl_TQRTestGate_TQRTestGate_ClassIf,
    NULL
};

ObjClassInfo TQRTestGate_clsinfo = {
    .instsize = sizeof(TQRTestGate),
    .classif = (ObjIface*)&TQRTestGate_ClassIf_tmpl,
    .parent = &ComplexTask_clsinfo,
    .ifimpl = _ifimpl_TQRTestGate,
};

static TQMPTest_ClassIf _impl_TQMPTest_TQMPTest_ClassIf = {
    ._size = sizeof(TQMPTest_ClassIf),
    ._implements = (ObjIface*)&TQMPTest_ClassIf_tmpl,
    .finish = (uint32 (*)(void*, uint32, TaskControl*))TQMPTest_finish,
};

static ObjIface* _ifimpl_TQMPTest[] = {
    (ObjIface*)&_impl_TQMPTest_TQMPTest_ClassIf,
    NULL
};

ObjClassInfo TQMPTest_clsinfo = {
    .instsize = sizeof(TQMPTest),
    .classif = (ObjIface*)&TQMPTest_ClassIf_tmpl,
    .parent = &MultiphaseTask_clsinfo,
    .ifimpl = _ifimpl_TQMPTest,
};

static TQTimeoutTest1_ClassIf _impl_TQTimeoutTest1_TQTimeoutTest1_ClassIf = {
    ._size = sizeof(TQTimeoutTest1_ClassIf),
    ._implements = (ObjIface*)&TQTimeoutTest1_ClassIf_tmpl,
    .run = (uint32 (*)(void*, TaskQueue*, TQWorker*, TaskControl*))TQTimeoutTest1_run,
};

static ObjIface* _ifimpl_TQTimeoutTest1[] = {
    (ObjIface*)&_impl_TQTimeoutTest1_TQTimeoutTest1_ClassIf,
    NULL
};

ObjClassInfo TQTimeoutTest1_clsinfo = {
    .instsize = sizeof(TQTimeoutTest1),
    .classif = (ObjIface*)&TQTimeoutTest1_ClassIf_tmpl,
    .parent = &ComplexTask_clsinfo,
    .ifimpl = _ifimpl_TQTimeoutTest1,
};

static TQTimeoutTest2_ClassIf _impl_TQTimeoutTest2_TQTimeoutTest2_ClassIf = {
    ._size = sizeof(TQTimeoutTest2_ClassIf),
    ._implements = (ObjIface*)&TQTimeoutTest2_ClassIf_tmpl,
    .run = (uint32 (*)(void*, TaskQueue*, TQWorker*, TaskControl*))TQTimeoutTest2_run,
};

static ObjIface* _ifimpl_TQTimeoutTest2[] = {
    (ObjIface*)&_impl_TQTimeoutTest2_TQTimeoutTest2_ClassIf,
    NULL
};

ObjClassInfo TQTimeoutTest2_clsinfo = {
    .instsize = sizeof(TQTimeoutTest2),
    .classif = (ObjIface*)&TQTimeoutTest2_ClassIf_tmpl,
    .parent = &ComplexTask_clsinfo,
    .ifimpl = _ifimpl_TQTimeoutTest2,
};

// ==================== Auto-generated section ends ======================
