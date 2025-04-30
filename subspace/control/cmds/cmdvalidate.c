// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "cmdvalidate.h"
// clang-format on
// ==================== Auto-generated section ends ======================

ControlCmd_impl("Validate", CmdValidate);

uint32 CmdValidate_run(_In_ CmdValidate* self, _In_ TaskQueue* tq, _In_ TQWorker* worker, _Inout_ TaskControl* tcon)
{
    return TASK_Result_Success;
}

// Autogen begins -----
void CmdValidate_register(ControlServer* svr);
#include "cmdvalidate.auto.inc"
// Autogen ends -------
