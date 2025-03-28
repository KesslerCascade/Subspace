#include <cx/container.h>
#include <cx/sys/entry.h>
#include <cx/time.h>
#include <cx/platform/os.h>
#include <cx/string.h>

#include <stdio.h>

DEFINE_ENTRY_POINT;

typedef struct BenchTest {
    int a;
    int b;
    int c;
    int d;
    int e;
    int f;
} BenchTest;

int entryPoint() {
    hashtable htbl;
    htInit(&htbl, int64, opaque(BenchTest), 16);

    BenchTest xyz;

    int64 istart = clockTimer();
    for (int i = 0; i < 5000000; i++) {
        htInsert(&htbl, int64, i, opaque, xyz);
    }
    int64 iend = clockTimer();

    printf("Insertion: %llu ms\n", timeToMsec(iend - istart));

    istart = clockTimer();
    BenchTest tmp;
    for (int i = 0; i < 5000000; i++) {
        if (!htFind(htbl, int64, i, opaque, &tmp, HT_Borrow))
            return 1;
    }
    iend = clockTimer();

    printf("Lookup: %llu ms\n", timeToMsec(iend - istart));

    xaFlush();
    //printf("Sleeping 5 seconds to check memory usage...\n");
    //osSleep(timeS(5));

    istart = clockTimer();
    for (int i = 0; i < 5000000; i++) {
        if (i < 1000 || i > 4999000 || (i & 3) == 1) {
            htRemove(&htbl, int64, i);
        }
    }
    iend = clockTimer();

    printf("Deletion: %llu ms\n", timeToMsec(iend - istart));

    istart = clockTimer();
    htiter hti;
    htiInit(&hti, htbl);
    int count = 0;
    while (htiValid(&hti)) {
        count++;
        htiNext(&hti);
    }
    iend = clockTimer();
    printf("Iteration: %llu ms\n", timeToMsec(iend - istart));
    printf("Count: %d\n", count);

    istart = clockTimer();
    for (int i = 0; i < 5000000; i++) {
        if (i < 1000000 || i > 4000000 || (i & 3)) {
            htRemove(&htbl, int64, i);
        }
    }
    iend = clockTimer();

    printf("Deletion: %llu ms\n", timeToMsec(iend - istart));

    istart = clockTimer();
    htiInit(&hti, htbl);
    count = 0;
    while (htiValid(&hti)) {
        count++;
        htiNext(&hti);
    }
    iend = clockTimer();
    printf("Iteration: %llu ms\n", timeToMsec(iend - istart));
    printf("Count: %d\n", count);

    istart = clockTimer();
    for (int i = 0; i < 5000000; i++) {
        htFind(htbl, int64, i, opaque, &tmp, HT_Borrow);
    }
    iend = clockTimer();

    printf("Lookup: %llu ms\n", timeToMsec(iend - istart));

    xaFlush();
    printf("Sleeping 5 seconds to check memory usage...\n");
    osSleep(timeS(5));
    htDestroy(&htbl);
    xaFlush();
    printf("Hashtable has been destroyed.\n");
    osSleep(timeS(5));
    return 0;
}
