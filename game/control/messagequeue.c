#include "messagequeue.h"
#include "ftl/stdlib.h"

MessageQueue* msgqCreate(int initsz, bool withcbs)
{
    MessageQueue* queue = malloc(sizeof(MessageQueue));
    if (!queue)
        return NULL;

    queue->maxmsgs = initsz;
    queue->nmsgs   = 0;
    queue->msgs    = malloc(sizeof(void*) * initsz);
    if (withcbs)
        queue->cbs = malloc(sizeof(void*) * initsz);

    return queue;
}

void msgqAdd(MessageQueue* queue, ControlMsg* msg, controlclientcb_t cb)
{
    if (queue->nmsgs == queue->maxmsgs) {
        queue->maxmsgs *= 2;
        queue->msgs = realloc(queue->msgs, sizeof(void*) * queue->maxmsgs);
        if (queue->cbs)
            queue->cbs = realloc(queue->cbs, sizeof(void*) * queue->maxmsgs);
    }

    if (queue->cbs)
        queue->cbs[queue->nmsgs] = cb;
    queue->msgs[queue->nmsgs++] = msg;
}

void msgqClear(MessageQueue* queue)
{
    for (int i = 0; i < queue->nmsgs; i++) {
        controlMsgFree(queue->msgs[i], CF_ALLOC_AUTO);
        queue->msgs[i] = NULL;
        if (queue->cbs)
            queue->cbs[i] = NULL;
    }
    queue->nmsgs = 0;
}

void msgqDestroy(MessageQueue* queue)
{
    msgqClear(queue);
    free(queue->msgs);
    if (queue->cbs)
        free(queue->cbs);
    free(queue);
}
