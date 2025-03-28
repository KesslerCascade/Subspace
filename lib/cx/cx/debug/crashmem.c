#include "crash_private.h"

static LazyInitState crashMemInitState;
sa_CrashMemRange _dbgCrashDumpMem;

static intptr crashMemCmp(stype st, stgeneric a, stgeneric b, uint32 flags)
{
    CrashMemRange *r1 = (CrashMemRange*)a.st_opaque;
    CrashMemRange *r2 = (CrashMemRange*)b.st_opaque;

    if (r1->start < r2->start)
        return -1;
    if (r1->start > r2->start)
        return 1;
    return 0;
}

static STypeOps crashMemOps = {
    .cmp = crashMemCmp
};

static void crashMemInit(void *data)
{
    saInit(&_dbgCrashDumpMem, custom(opaque(CrashMemRange), crashMemOps), 10, SA_Sorted | SA_Grow(Slow));
}

void dbgCrashIncludeMemory(void *ptr, size_t sz)
{
    // removing any completely contained blocks first makes the
    // add algorithm considerably simpler
    dbgCrashExcludeMemory(ptr, sz);

    CrashMemRange r = { .start = (uintptr)ptr,.end = (uintptr)ptr + sz };
    int32 idx = saFind(_dbgCrashDumpMem, opaque, r, SA_Inexact);

    if (idx > 0 && _dbgCrashDumpMem.a[idx - 1].end == r.start) {
        // extend the previous block
        _dbgCrashDumpMem.a[idx - 1].end = r.end;
    } else if (idx >= 0 && idx < saSize(_dbgCrashDumpMem) && _dbgCrashDumpMem.a[idx].start == r.end) {
        // extend the following block
        _dbgCrashDumpMem.a[idx].start = r.start;
    } else {
        // insert a new block
        saPush(&_dbgCrashDumpMem, opaque, r, SA_Unique);
    }
}

void dbgCrashExcludeMemory(void *ptr, size_t sz)
{
    lazyInit(&crashMemInitState, crashMemInit, 0);
    _Analysis_assume_(_dbgCrashDumpMem.a != NULL);

    int32 idx;
    CrashMemRange r = { .start = (uintptr)ptr,.end = (uintptr)ptr + sz };

    // use bsearch to quickly get to the starting index to scan
    idx = saFind(_dbgCrashDumpMem, opaque, r, SA_Inexact);
    // back up one index entry to catch partial overlap
    idx = max(idx - 1, 0);

    for (; idx < saSize(_dbgCrashDumpMem); idx++) {
        CrashMemRange *ir = &_dbgCrashDumpMem.a[idx];
        if (r.start <= ir->start && r.end >= ir->end) {
            // completely inside removal range, delete it
            saRemove(&_dbgCrashDumpMem, idx--);
        } else if (r.start <= ir->start && r.end > ir->start) {
            // cut off the start of the range
            ir->start = r.end;
        } else if (r.start < ir->end && r.end >= ir->end) {
            // cut off the end of the range
            ir->end = r.start;
        } else if (r.start > ir->start && r.end < ir->end) {
            // have to split the block
            CrashMemRange nr;
            nr.start = r.end;
            nr.end = ir->end;
            ir->end = r.start;

#if DEBUG_LEVEL == 0
            saPush(&_dbgCrashDumpMem, opaque, nr, SA_Unique);
#else
            int32 nidx = saPush(&_dbgCrashDumpMem, opaque, nr, SA_Unique);
            devAssert(nidx == idx + 1);
#endif
        } else if (r.end <= ir->start) {
            // beyond what we're looking for
            break;
        }
    }
}
