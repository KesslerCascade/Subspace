#include <stdio.h>
#include <cx/log.h>
#include <cx/string.h>
#include <cx/thread.h>
#include <cx/platform/os.h>

#include <cx/time.h>
#define TEST_FILE logtest
#define TEST_FUNCS logtest_funcs
#include "common.h"

static Event logtestevent;

typedef struct LogTestData {
    int test;
    int count;
    bool fail;
    LogCategory *lastcat;
} LogTestData;

static void testdest(int level, LogCategory *cat, int64 timestamp, strref msg, uint32 batchid, void *userdata)
{
    LogTestData *td = (LogTestData*)userdata;
    bool signal = true;

    td->count++;
    td->lastcat = cat;

    if (td->test == 1) {
        td->fail = !strEq(msg, _S"Info test");
    } else if (td->test == 2) {
        td->fail = !strEq(msg, _S"Notice test");
    } else if (td->test == 3) {
        signal = (td->count == 2);
        td->fail = !strEq(msg, _S"Info test") && !strEq(msg, _S"Notice test");
    } else if (td->test == 4) {
        // should NOT receive this test
        td->fail = true;
    } else if (td->test == 5) {
        signal = (td->count == 2);
        td->fail = !strEq(msg, _S"Error test");
    } else if (td->test == 1000 && level == -1) {
        td->fail = false;
    } else if (td->test == 20) {
        signal = (td->count == 16);
        td->fail = false;
    } else if (td->test == 21) {
        signal = (td->count == 1600);
        td->fail = false;
    } else if (td->test == 31) {
        signal = (td->count == 5);
        td->fail = !(td->count == 5);
    } else if (td->test == 32) {
        signal = (td->count == 3);
        td->fail = !(td->count == 3);
    } else {
        td->fail = true;
    }

    if (signal)
        eventSignal(&logtestevent);
}

static int test_log_levels()
{
    int ret = 0;
    LogTestData td = { 0 };
    eventInit(&logtestevent);

    logRestart();   // only needed for alltests; shutdown may have previously been called

    LogMembufData *lmd = logmembufCreate(4096);
    logRegisterDest(LOG_Verbose, NULL, logmembufDest, lmd);
    logRegisterDest(LOG_Info, NULL, testdest, &td);
    logRegisterDest(LOG_Error, NULL, testdest, &td);

    td.test = 1;
    td.count = 0;
    td.fail = true;
    logStr(Info, _S"Info test");
    if (!eventWaitTimeout(&logtestevent, timeS(1)))
        ret = 1;
    if (td.fail || td.count != 1)
        ret = 1;

    td.test = 2;
    td.count = 0;
    td.fail = true;
    logStr(Notice, _S"Notice test");
    if (!eventWaitTimeout(&logtestevent, timeS(1)))
        ret = 1;
    if (td.fail || td.count != 1)
        ret = 1;

    td.test = 3;
    td.count = 0;
    td.fail = true;
    logStr(Info, _S"Info test");
    logStr(Notice, _S"Notice test");
    if (!eventWaitTimeout(&logtestevent, timeS(1)))
        ret = 1;
    if (td.fail || td.count != 2)
        ret = 1;

    // should NOT be received by the destination
    td.test = 4;
    td.count = 0;
    td.fail = false;
    logStr(Verbose, _S"Verbose test");
    osSleep(timeMS(100));
    if (td.fail || td.count != 0)
        ret = 1;

    // should be received by both destinations
    td.test = 5;
    td.count = 0;
    logStr(Verbose, _S"Verbose test 2");
    logStr(Verbose, _S"Verbose test 3");
    logStr(Verbose, _S"Verbose test 4");
    logStr(Error, _S"Error test");
    if (!eventWaitTimeout(&logtestevent, timeS(1)))
        ret = 1;
    if (td.fail || td.count != 2)
        ret = 1;

    if (lmd->cur != 284)
        ret = 1;

    logShutdown();

    eventDestroy(&logtestevent);
    return ret;
}


static int test_log_shutdown()
{
    int ret = 0;
    LogTestData td = { 0 };
    eventInit(&logtestevent);

    logRestart();   // only needed for alltests; shutdown may have previously been called

    logRegisterDest(LOG_Info, NULL, testdest, &td);

    td.test = 1000;
    td.fail = true;
    logShutdown();
    if (td.fail)
        ret = 1;

    eventDestroy(&logtestevent);
    return ret;
}

static int test_log_batch()
{
    int ret = 0;
    LogTestData td = { 0 };
    eventInit(&logtestevent);

    logRestart();   // only needed for alltests; shutdown may have previously been called

    LogMembufData *lmd = logmembufCreate(128 * 1024);
    logRegisterDest(LOG_Verbose, NULL, logmembufDest, lmd);
    logRegisterDest(LOG_Info, NULL, testdest, &td);
    logRegisterDest(LOG_Error, NULL, testdest, &td);

    td.test = 20;
    td.count = 0;
    td.fail = true;
    logBatchBegin();
    for (int i = 0; i < 16; i++) {
        logFmt(Info, _S"${string} test", stvar(string, _S"Info"));
    }
    if (td.count != 0)
        ret = 1;
    logBatchEnd();
    if (!eventWaitTimeout(&logtestevent, timeS(1)))
        ret = 1;
    if (td.fail || td.count != 16)
        ret = 1;

    td.test = 21;
    td.count = 0;
    td.fail = true;
    logBatchBegin();
    for (int i = 0; i < 1600; i++) {
        logFmt(Info, _S"${string} ${string}", stvar(strref, _S"Info"), stvar(strref, _S"test"));
    }
    osSleep(timeMS(100));
    if (td.count != 0)
        ret = 1;
    logBatchEnd();
    if (!eventWaitTimeout(&logtestevent, timeS(1)))
        ret = 1;
    if (td.fail || td.count != 1600)
        ret = 1;

    if (lmd->cur != 46864)
        ret = 1;

    logShutdown();

    eventDestroy(&logtestevent);
    return ret;
}

static int test_log_categories()
{
    int ret        = 0;
    LogTestData td = { 0 };
    eventInit(&logtestevent);

    logRestart();   // only needed for alltests; shutdown may have previously been called

    LogMembufData *lmd = logmembufCreate(4096);
    LogCategory *cat1 = logCreateCat(_S"cat1", false);
    LogCategory *cat2 = logCreateCat(_S"cat2", true);
    LogCategory *cat3 = logCreateCat(_S"cat3", true);
    logRegisterDest(LOG_Verbose, NULL, logmembufDest, lmd);
    logRegisterDest(LOG_Info, NULL, testdest, &td);
    logRegisterDest(LOG_Info, cat1, testdest, &td);
    logRegisterDest(LOG_Info, cat2, testdest, &td);

    // should only be received by the NULL filter
    td.test = 1;
    td.count = 0;
    td.fail = true;
    logStr(Info, _S"Info test");
    if (!eventWaitTimeout(&logtestevent, timeS(1)))
        ret = 1;
    if (td.fail || td.count != 1)
        ret = 1;

    // should be received by cat1 and NULL filter
    td.lastcat = NULL;
    td.test = 3;
    td.count = 0;
    td.fail = true;
    logStrC(Info, cat1, _S"Info test");
    if (!eventWaitTimeout(&logtestevent, timeS(1)))
        ret = 1;
    if (td.fail || td.count != 2 || td.lastcat != cat1)
        ret = 1;

    // should ONLY be received by cat2 filter
    td.lastcat = NULL;
    td.test = 1;
    td.count = 0;
    td.fail = false;
    logStrC(Info, cat2, _S"Info test");
    if (!eventWaitTimeout(&logtestevent, timeS(1)))
        ret = 1;
    if (td.fail || td.count != 1 || td.lastcat != cat2)
        ret = 1;

    // should not be received by ANY destination
    td.lastcat = NULL;
    td.test = 4;
    td.count = 0;
    td.fail = false;
    logStrC(Info, cat3, _S"Info test");
    osSleep(timeMS(100));
    if (td.fail || td.count != 0 || td.lastcat != NULL)
        ret = 1;

    if (lmd->cur != 65)
        ret = 1;

    logShutdown();

    eventDestroy(&logtestevent);
    return ret;
}

static int test_log_defer()
{
    int ret = 0;
    LogTestData td = { 0 };
    eventInit(&logtestevent);

    logRestart();   // only needed for alltests; shutdown may have previously been called

    LogDeferData *ldata1 = logDeferCreate();
    LogDeferData *ldata2 = logDeferCreate();

    LogDest *ldd1 = logRegisterDest(LOG_Verbose, NULL, logDeferDest, ldata1);
    LogDest *ldd2 = logRegisterDest(LOG_Verbose, NULL, logDeferDest, ldata2);

    td.test = 31;
    td.count = 0;
    td.fail = true;

    logStr(Info, _S"Info test");
    logStr(Notice, _S"Notice test");
    logStr(Warn, _S"Warn test");
    logStr(Verbose, _S"Verbose test");
    logStr(Error, _S"Error test");
    logStr(Info, _S"Info test 2");

    // nothing should be received yet, should all be in defer buffers
    osSleep(timeMS(100));
    if (td.count != 0)
        ret = 1;

    // Everything should have gone to the defer buffers during the osSleep above,
    // but for testing purposes make absolutely sure that nothing is still in-flight.
    // In a real-world scenario that would be fine as they'd just end up going to the
    // real destination instead, but we are specifically are testing the defer handoff.
    logFlush();

    LogMembufData *lmd = logmembufCreate(4096);
    logRegisterDestWithDefer(LOG_Verbose, NULL, logmembufDest, lmd, ldd1);
    logRegisterDestWithDefer(LOG_Info, NULL, testdest, &td, ldd2);

    // Specifically check for 5 events. The Verbose level entry went into the defer buffer
    // because it was registered at that level, but should have been filtered before going to
    // the actual destination.
    if (!eventWaitTimeout(&logtestevent, timeS(1)))
        ret = 1;
    if (td.fail || td.count != 5)
        ret = 1;

    td.test = 32;
    td.count = 0;
    td.fail = true;
    logStr(Info, _S"Info test");
    logStr(Notice, _S"Notice test");
    logStr(Warn, _S"Warn test");
    if (!eventWaitTimeout(&logtestevent, timeS(1)))
        ret = 1;
    if (td.fail || td.count != 3)
        ret = 1;

    // LMD buffer *should* include the Verbose entry
    if (lmd->cur != 271)
        ret = 1;

    logShutdown();

    eventDestroy(&logtestevent);
    return ret;
}

testfunc logtest_funcs[] = {
    { "levels", test_log_levels },
    { "shutdown", test_log_shutdown },
    { "batch", test_log_batch },
    { "categories", test_log_categories },
    { "defer", test_log_defer },
    { 0, 0 }
};
