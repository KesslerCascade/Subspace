#include <stdio.h>
#include <cx/string.h>
#include <cx/thread.h>

#define TEST_FILE prqtest
#define TEST_FUNCS prqtest_funcs
#include "common.h"

static int test_prqtest_basic()
{
    int ret = 0;
    PrQueue queue;

    prqInitFixed(&queue, 64);

    int testdata[65];
    int i;
    for(i = 0; i < 65; i++) {
        testdata[i] = i;
    }

    for(i = 0; i < 64; i++) {
        if(!prqPush(&queue, &testdata[i]))
            ret = 1;
    }

    // queue should be full
    if(prqPush(&queue, &testdata[64]))
        ret = 1;

    for(i = 0; i < 64; i++) {
        int *pint = prqPop(&queue);
        if(!pint || *pint != i)
            ret = 1;
    }

    // queue should be empty now
    if(prqPop(&queue) != NULL)
        ret = 1;

    for(i = 0; i < 65; i++) {
        testdata[i] = i + 65;
    }

    for(int j = 0; j < 4; j++) {
        for(i = 0; i < 16; i++) {
            if(!prqPush(&queue, &testdata[j * 16 + i]))
                ret = 1;
        }

        for(i = 0; i < 8; i++) {
            int *pint = prqPop(&queue);
            if(!pint || *pint != 65 + j * 8 + i)
                ret = 1;
        }
    }

    for(i = 0; i < 32; i++) {
        int *pint = prqPop(&queue);
        if(!pint || *pint != 97 + i)
            ret = 1;
    }

    // interleaved push/pop

    for(i = 0; i < 65; i++) {
        testdata[i] = i + 128;
    }

    for(i = 0; i < 64; i++) {
        if(!prqPush(&queue, &testdata[i]))
            ret = 1;

        int *pint = prqPop(&queue);
        if(!pint || *pint != 128 + i)
            ret = 1;
    }

    prqDestroy(&queue);

    return ret;
}

#define PRQ_PRODUCERS 4
#define PRQ_RELAYS 3
#define PRQ_CONSUMERS 6
#define PRQ_NUM 614400

static atomic(bool) mtfail;
static atomic(uint32) pushfail;

static int mtTestProduce(Thread *self)
{
    PrQueue *queue;
    int *testdata;
    int32 tdstart, tdend;
    if(!stvlNext(&self->args, ptr, &queue) ||
       !stvlNext(&self->args, ptr, &testdata) ||
       !stvlNext(&self->args, int32, &tdstart) ||
       !stvlNext(&self->args, int32, &tdend))
        return 0;

    for(int i = tdstart; i < tdend; i++) {
        while(!prqPush(queue, &testdata[i])) {
            atomicFetchAdd(uint32, &pushfail, 1, Relaxed);
            osYield();
        }
    }

    return 0;
}

static int mtTestRelay(Thread *self)
{
    PrQueue *queue1, *queue2;
    int32 num;
    if(!stvlNext(&self->args, ptr, &queue1) ||
       !stvlNext(&self->args, ptr, &queue2) ||
       !stvlNext(&self->args, int32, &num))
        return 0;

    int count = 0;
    while (count < num) {
        int *pint = prqPop(queue1);
        if(pint) {
            while(!prqPush(queue2, pint)) {
                osYield();
            }
            count++;
        } else {
            osYield();
        }

        if (count % 115 == 0)
            prqCollect(queue1);
    }

    prqCollect(queue1);

    return 0;
}

static int mtTestConsume(Thread *self)
{
    PrQueue *queue;
    Event *ev;
    int32 num;
    atomic(uint32) *total;
    atomic(uint32) *done;

    if(!stvlNext(&self->args, ptr, &queue) ||
       !stvlNext(&self->args, ptr, &ev) ||
       !stvlNext(&self->args, int32, &num) ||
       !stvlNext(&self->args, ptr, &total) ||
       !stvlNext(&self->args, ptr, &done))
        return 0;

    unsigned int subtotal = 0;
    int count = 0;
    while(count < num) {
        unsigned int *pint = prqPop(queue);
        if(pint) {
            subtotal += *pint;
            count++;
        } else {
            osYield();
        }
        if (count % 100 == 0)
            prqCollect(queue);
    }

    prqCollect(queue);

    atomicFetchAdd(uint32, total, subtotal, AcqRel);
    atomicFetchAdd(uint32, done, 1, AcqRel);
    eventSignal(ev);
    return 0;
}

static int test_prqtest_mt()
{
    int ret = 0;
    PrQueue queue1, queue2;
    Event ev;
    atomic(uint32) total = atomicInit(0), done = atomicInit(0);

    prqInitFixed(&queue1, 65);
    prqInitFixed(&queue2, 65);

    unsigned int *testdata = xaAlloc(sizeof(int) * PRQ_NUM);
    unsigned int expected = 0;
    for(int i = 0; i < PRQ_NUM; i++) {
        testdata[i] = i;
        expected += i;
    }

    eventInit(&ev);
    for(int i = 0; i < PRQ_CONSUMERS; i++) {
        thrRun(mtTestConsume, _S"mtTestConsume",
               stvar(ptr, &queue2),
               stvar(ptr, &ev),
               stvar(int32, PRQ_NUM / PRQ_CONSUMERS),
               stvar(ptr, &total),
               stvar(ptr, &done));
    }

    for(int i = 0; i < PRQ_RELAYS; i++) {
        thrRun(mtTestRelay, _S"mtTestRelay",
               stvar(ptr, &queue1),
               stvar(ptr, &queue2),
               stvar(int32, PRQ_NUM / PRQ_RELAYS));
    }

    for(int i = 0; i < PRQ_PRODUCERS; i++) {
        thrRun(mtTestProduce, _S"mtTestProduce",
               stvar(ptr, &queue1),
               stvar(ptr, testdata),
               stvar(int32, (PRQ_NUM / PRQ_PRODUCERS) * i),
               stvar(int32, (PRQ_NUM / PRQ_PRODUCERS) * (i +1)));
    }

    int64 starttime = clockWall();
    while(atomicLoad(uint32, &done, Acquire) < PRQ_CONSUMERS) {
        eventWaitTimeout(&ev, timeS(10));
        if(atomicLoad(bool, &mtfail, Acquire) || clockWall() > starttime + timeS(60)) {
            ret = 1;
            break;
        }
    }

    xaFree(testdata);

    unsigned int result = atomicLoad(uint32, &total, Acquire);
    if(result != expected)
        ret = 1;

    prqDestroy(&queue1);
    prqDestroy(&queue2);

    return ret;
}

static int test_prqtest_grow()
{
    int ret = 0;
    PrQueue queue;

    prqInitDynamic(&queue, 8, 16, 64, PRQ_Grow_100, PRQ_Grow_None);

    int testdata[121];
    int i;
    for(i = 0; i < 121; i++) {
        testdata[i] = i;
    }

    // This should expand the queue 3 times, filling
    // the original 8 slots, then 16, then 32, finally 64 more
    for(i = 0; i < 120; i++) {
        if(!prqPush(&queue, &testdata[i]))
            ret = 1;
    }

    // queue should be full
    if(prqPush(&queue, &testdata[120]))
        ret = 1;

    for(i = 0; i < 120; i++) {
        int *pint = prqPop(&queue);
        if(!pint || *pint != i)
            ret = 1;
    }

    // queue should be empty now
    if(prqPop(&queue) != NULL)
        ret = 1;

    // now that it's expanded, we should only have 64 usable slots, so repeat the test and ensure it fulls up at the right time

    for(i = 0; i < 121; i++) {
        testdata[i] = i + 121;
    }

    for(i = 0; i < 64; i++) {
        if(!prqPush(&queue, &testdata[i]))
            ret = 1;
    }

    // queue should be full as one slot is reserved
    if(prqPush(&queue, &testdata[64]))
        ret = 1;

    for(i = 0; i < 64; i++) {
        int *pint = prqPop(&queue);
        if(!pint || *pint != testdata[i])
            ret = 1;
    }

    // queue should be empty now
    if(prqPop(&queue) != NULL)
        ret = 1;

    prqDestroy(&queue);

    return ret;
}

static int test_prqtest_gc()
{
    int ret = 0;
    PrQueue queue;

    prqInitDynamic(&queue, 8, 16, 64, PRQ_Grow_100, PRQ_Grow_None);

    int testdata[121];
    int i;
    for(i = 0; i < 121; i++) {
        testdata[i] = i;
    }

    // fill up first segment, then 1 more to force an expansion
    for(i = 0; i < 9; i++) {
        if(!prqPush(&queue, &testdata[i]))
            ret = 1;
    }

    PrqSegment *seg1 = atomicLoad(ptr, &queue.current, Acquire);
    if(!atomicLoad(ptr, &seg1->nextseg, Acquire) || seg1->size != 8)
        ret = 1;

    for(i = 0; i < 8; i++) {
        int *pint = prqPop(&queue);
        if(!pint || *pint != testdata[i])
            ret = 1;
    }

    prqCollect(&queue);             // once to mark retired
    prqCollect(&queue);             // once to actually retire

    // seg1 should be retired, seg2 should not have a next
    PrqSegment *seg2 = atomicLoad(ptr, &queue.current, Acquire);
    if(seg1 == seg2 || atomicLoad(ptr, &seg2->nextseg, Acquire) || seg2->size != 16)
        return 1;

    // should be able to push 15 more without allocating a new segment
    for (i = 9; i < 24; i++) {
        if(!prqPush(&queue, &testdata[i]))
            ret = 1;
    }
    if(atomicLoad(ptr, &seg2->nextseg, Acquire))
        ret = 1;

    // should do nothing, except maybe deallocate
    prqCollect(&queue);

    if(!prqPush(&queue, &testdata[24]))
        ret = 1;

    PrqSegment *seg3 = atomicLoad(ptr, &seg2->nextseg, Acquire);
    if(!seg3 || seg3->size != 32)
        return 1;

    if(atomicLoad(uint32, &seg2->reserved, Acquire) & 0x80000000)
        ret = 1;
    // should mark seg2 as retired
    prqCollect(&queue);
    if(atomicLoad(ptr, &queue.current, Acquire) != seg2)
        ret = 1;
    if(!(atomicLoad(uint32, &seg2->reserved, Acquire) & 0x80000000))
        ret = 1;

    // pop all of them but one
    for(i = 8; i < 23; i++) {
        int *pint = prqPop(&queue);
        if(!pint || *pint != testdata[i])
            ret = 1;
    }

    // should do nothing
    prqCollect(&queue);
    if(atomicLoad(ptr, &queue.current, Acquire) != seg2)
        ret = 1;

    for(i = 23; i < 24; i++) {
        int *pint = prqPop(&queue);
        if(!pint || *pint != testdata[i])
            ret = 1;
    }

    // finally should move seg2 to retired and promote seg3
    prqCollect(&queue);

    if(atomicLoad(ptr, &queue.current, Acquire) != seg3)
        ret = 1;

    // should be 1 item left in the queue
    if(prqPop(&queue) == NULL)          // 22
        ret = 1;

    // queue should be empty now
    if(prqPop(&queue) != NULL)
        ret = 1;

    prqDestroy(&queue);

    return ret;
}

#define MPMC_PRODUCERS 12
#define MPMC_RELAYS 12
#define MPMC_CONSUMERS 12
#define MPMC_NUM 614400

static int test_prqtest_mpmc()
{
    int ret = 0;
    PrQueue queue1, queue2;
    Event ev;
    atomic(uint32) total = atomicInit(0), done = atomicInit(0);

    atomicStore(bool, &mtfail, false, Release);

    prqInitDynamic(&queue1, 4, 1024, 10240, PRQ_Grow_25, PRQ_Grow_100);
    prqInitDynamic(&queue2, 4, 1024, 10240, PRQ_Grow_150, PRQ_Grow_100);

    atomicStore(bool, &mtfail, false, Release);

    unsigned int *testdata = xaAlloc(sizeof(int) * MPMC_NUM);
    unsigned int expected = 0;
    for(int i = 0; i < MPMC_NUM; i++) {
        testdata[i] = i;
        expected += i;
    }

    eventInit(&ev);
    for(int i = 0; i < MPMC_CONSUMERS; i++) {
        thrRun(mtTestConsume, _S"mtTestConsume",
               stvar(ptr, &queue2),
               stvar(ptr, &ev),
               stvar(int32, MPMC_NUM / MPMC_CONSUMERS),
               stvar(ptr, &total),
               stvar(ptr, &done));
    }

    for(int i = 0; i < MPMC_RELAYS; i++) {
        thrRun(mtTestRelay, _S"mtTestRelay",
               stvar(ptr, &queue1),
               stvar(ptr, &queue2),
               stvar(int32, MPMC_NUM / MPMC_RELAYS));
    }

    for(int i = 0; i < MPMC_PRODUCERS; i++) {
        thrRun(mtTestProduce, _S"mtTestProduce",
               stvar(ptr, &queue1),
               stvar(ptr, testdata),
               stvar(int32, (MPMC_NUM / MPMC_PRODUCERS) * i),
               stvar(int32, (MPMC_NUM / MPMC_PRODUCERS) * (i + 1)));
    }

    int64 starttime = clockWall();
    bool nofree = false;
    while(atomicLoad(uint32, &done, Acquire) < MPMC_CONSUMERS) {
        eventWaitTimeout(&ev, timeS(10));
        if(atomicLoad(bool, &mtfail, Acquire) || clockWall() > starttime + timeS(60)) {
            ret = 1;
            nofree = true;          // threads are still running
            break;
        }
    }

    if (!nofree)
        xaFree(testdata);

    unsigned int result = atomicLoad(uint32, &total, Acquire);
    if(result != expected)
        ret = 1;

    prqDestroy(&queue1);
    prqDestroy(&queue2);

    return ret;
}

testfunc prqtest_funcs[] = {
    { "basic", test_prqtest_basic },
    { "mt", test_prqtest_mt },
    { "grow", test_prqtest_grow },
    { "gc", test_prqtest_gc },
    { "mpmc", test_prqtest_mpmc },
    { 0, 0 }
};
