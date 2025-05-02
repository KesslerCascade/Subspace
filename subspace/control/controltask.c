// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "controltask.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include "gamemgr/gameinst.h"

void ControlTask_destroy(_In_ ControlTask* self)
{
    controlMsgFree(self->msg, CF_ALLOC_AUTO);
    // Autogen begins -----
    objRelease(&self->client);
    objRelease(&self->handler);
    // Autogen ends -------
}

_objfactory_guaranteed ControlTask*
ControlTask_create(ControlClient* client, ControlHandler* handler, ControlMsg* msg)
{
    ControlTask* self;
    self = objInstCreate(ControlTask);

    self->client  = objAcquire(client);
    self->handler = objAcquire(handler);
    self->msg     = msg;

    objInstInit(self);

    if (handler->require)
        ctaskRequireResource(self, handler->require);

    return self;
}

uint32 ControlTask_run(_In_ ControlTask* self, _In_ TaskQueue* tq, _In_ TQWorker* worker,
                       _Inout_ TaskControl* tcon)
{
    GameInst* inst = objAcquireFromWeak(GameInst, self->client->inst);
    if (self->handler->needinst && !inst)
        return TASK_Result_Success;

    hashtable fields;
    htInit(&fields, string, stvar, self->msg->hdr.nfields * 2);

    for (int i = 0; i < self->msg->hdr.nfields; i++) {
        ControlField* f = self->msg->fields[i];
        stvar val       = { 0 };
        if (!(f->h.flags & CF_ARRAY)) {
            switch (f->h.ftype) {
            case CF_INT:
                if (f->h.flags & CF_UNSIGNED) {
                    val.type           = stType(uint32);
                    val.data.st_uint32 = f->d.cfd_uint;
                } else {
                    val.type          = stType(int32);
                    val.data.st_int32 = f->d.cfd_int;
                }
                break;
            case CF_FLOAT32:
                val.type            = stType(float32);
                val.data.st_float32 = f->d.cfd_float32;
                break;
            case CF_FLOAT64:
                val.type            = stType(float64);
                val.data.st_float64 = f->d.cfd_float64;
                break;
            case CF_STRING:
                val.type = stType(string);
                strDup(&val.data.st_string, f->d.cfd_str);
                break;
            }
        } else {
            val.type = stType(sarray);
            switch (f->h.ftype) {
            case CF_INT:
                if (f->h.flags * CF_UNSIGNED)
                    saInit(&val.data.st_sarray, uint32, f->count);
                else
                    saInit(&val.data.st_sarray, int32, f->count);

                saSetSize(&val.data.st_sarray, f->count);
                memcpy(val.data.st_sarray.a, f->d.cfd_int_arr, f->count * sizeof(int32));
                break;
            case CF_FLOAT32:
                saInit(&val.data.st_sarray, float32, f->count);
                saSetSize(&val.data.st_sarray, f->count);
                memcpy(val.data.st_sarray.a, f->d.cfd_float32_arr, f->count * sizeof(float32));
                break;
            case CF_FLOAT64:
                saInit(&val.data.st_sarray, float64, f->count);
                saSetSize(&val.data.st_sarray, f->count);
                memcpy(val.data.st_sarray.a, f->d.cfd_float64_arr, f->count * sizeof(float64));
                break;
            case CF_STRING:
                saClone(&val.data.st_sarray, f->d.cfd_str_arr);
                break;
            }
        }

        if (val.type != 0)
            htInsertC(&fields, string, (string)f->h.name, stvar, &val);
    }

    self->handler->cb(inst, self->client, self->msg, fields);

    objRelease(&inst);
    htDestroy(&fields);

    return TASK_Result_Success;
}

// Autogen begins -----
#include "controltask.auto.inc"
// Autogen ends -------
