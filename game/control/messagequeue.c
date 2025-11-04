#include "messagequeue.h"
#include "ftl/stdlib.h"

MessageQueue* msgqCreate(int initsz, bool withcbs)
{
    MessageQueue* queue = xa_malloc(sizeof(MessageQueue));
    if (!queue)
        return NULL;

    queue->maxmsgs = initsz;
    queue->nmsgs   = 0;
    queue->msgs    = xa_malloc(sizeof(void*) * initsz);
    if (withcbs)
        queue->cbs = xa_malloc(sizeof(void*) * initsz);
    else
        queue->cbs = NULL;

    return queue;
}

void msgqAdd(MessageQueue* queue, ControlMsg* msg, controlclientcb_t cb)
{
    if (queue->nmsgs == queue->maxmsgs) {
        queue->maxmsgs *= 2;
        queue->msgs = xa_realloc(queue->msgs, sizeof(void*) * queue->maxmsgs);
        if (queue->cbs)
            queue->cbs = xa_realloc(queue->cbs, sizeof(void*) * queue->maxmsgs);
    }

    if (queue->cbs)
        queue->cbs[queue->nmsgs] = cb;
    queue->msgs[queue->nmsgs++] = msg;
}

void msgqClear(MessageQueue* queue)
{
    for (int i = 0; i < queue->nmsgs; i++) {
        controlMsgDestroy(queue->msgs[i]);
        queue->msgs[i] = NULL;
        if (queue->cbs)
            queue->cbs[i] = NULL;
    }
    queue->nmsgs = 0;
}

void msgqDestroy(MessageQueue* queue)
{
    msgqClear(queue);
    xa_free(queue->msgs);
    if (queue->cbs)
        xa_free(queue->cbs);
    xa_free(queue);
}
