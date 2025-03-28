// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "trfifo.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include <cx/taskqueue/taskqueue.h>

static void destroyFifoNode(TRFifoNode *node) {
    objRelease(&node->task);
    xaFree(node);
}

static void destroyFifoList(TRFifoNode *node) {
    TRFifoNode* next;

    while(node) {
        next = node->next;
        objRelease(&node->task);
        xaFree(node);
        node = next;
    }
}

bool TRFifo_registerTask(_In_ TRFifo* self, ComplexTask* task)
{
    withMutex(&self->_fifomtx) {
        TRFifoNode* nnode = xaAllocStruct(TRFifoNode, XA_Zero);
        nnode->task       = objAcquire(task);
        if (self->tail) {
            self->tail->next = nnode;
        }
        self->tail = nnode;
        if (!self->head)
            self->head = nnode;
    }

    return true;
}

bool TRFifo_canAcquire(_In_ TRFifo* self, ComplexTask* task)
{
    bool ret = false;

    withMutex (&self->_fifomtx) {
        // FIFO is strictly ordered -- task may ONLY acquire if it's at the head of the queue
        if (!self->cur && self->head && self->head->task == task)
                ret = true;
    }

    return ret;
}

bool TRFifo_tryAcquire(_In_ TRFifo* self, ComplexTask* task)
{
    bool ret = false;

    withMutex (&self->_fifomtx) {
        // FIFO is strictly ordered -- task may ONLY acquire if it's at the head of the queue
        if (!self->cur) {
            if (self->head && self->head->task == task) {
                TRFifoNode* next = self->head->next;
                destroyFifoNode(self->head);
                self->head = next;

                // if we destroyed the only node; need to clear tail as well
                if (!next)
                    self->tail = NULL;

                self->cur = task;
                ret       = true;
            }
        } else {
            ret = (self->cur == task);   // task already has it!
        }
    }

    return ret;
}

void TRFifo_release(_In_ TRFifo* self, ComplexTask* task)
{
    withMutex(&self->_fifomtx) {
        if (self->cur == task)
            self->cur = NULL;

        
        // advance the next task in line
        if (self->head)
            ctaskAdvance(self->head->task);
    }
}

_objinit_guaranteed bool TRFifo_init(_In_ TRFifo* self)
{
    // Autogen begins -----
    mutexInit(&self->_fifomtx);
    return true;
    // Autogen ends -------
}

void TRFifo_destroy(_In_ TRFifo* self)
{
    destroyFifoList(self->head);
    // Autogen begins -----
    mutexDestroy(&self->_fifomtx);
    // Autogen ends -------
}

_objfactory_guaranteed TRFifo* TRFifo_create()
{
    TRFifo* self;
    self = objInstCreate(TRFifo);
    objInstInit(self);
    return self;
}

// Autogen begins -----
#include "trfifo.auto.inc"
// Autogen ends -------
