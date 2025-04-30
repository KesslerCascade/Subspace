// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "cmdfeaturestate.h"
// clang-format on
// ==================== Auto-generated section ends ======================

ControlCmd_impl("FeatureState", CmdFeatureState);

uint32 CmdFeatureState_run(_In_ CmdFeatureState* self, _In_ TaskQueue* tq, _In_ TQWorker* worker, _Inout_ TaskControl* tcon)
{
    return TASK_Result_Success;
}

// Autogen begins -----
void CmdFeatureState_register(ControlServer* svr);
#include "cmdfeaturestate.auto.inc"
// Autogen ends -------
