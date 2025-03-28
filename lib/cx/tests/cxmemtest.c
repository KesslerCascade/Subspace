#include <stdio.h>
#include <cx/xalloc/xalloc.h>

#define TEST_FILE cxmemtest
#define TEST_FUNCS cxmemtest_funcs
#include "common.h"

static int test_malloc()
{
    char *blk1 = 0, *blk2 = 0, *blk3 = 0;
    int i;

    blk1 = (char*)xaAlloc(50);
    if (!blk1)
        return 1;
    for (i = 0; i < 50; i++)
        blk1[i] = '0' + (i % 10);

    blk2 = (char*)xaAlloc(90000);
    if (!blk2)
        return 1;
    for (i = 0; i < 90000; i++)
        blk2[i] = '0' + (i % 10);

    blk3 = (char*)xaAlloc(67108865);
    if (!blk3)
        return 1;
    for (i = 0; i < 67108865; i++)
        blk3[i] = '0' + (i % 10);

    xaFree(blk1);
    xaFree(blk2);
    xaFree(blk3);

    return 0;
}

static int test_usable_size()
{
    char *blk1 = 0, *blk2 = 0, *blk3 = 0;
    blk1 = (char*)xaAlloc(50);
    blk2 = (char*)xaAlloc(90000);
    blk3 = (char*)xaAlloc(67108865);

    if (!blk1 || xaSize(blk1) < 50)
        return 1;
    if (!blk2 || xaSize(blk2) < 90000)
        return 1;
    if (!blk3 || xaSize(blk3) < 67108865)
        return 1;

    xaFree(blk1);
    xaFree(blk2);
    xaFree(blk3);

    return 0;
}

static int test_free()
{
    char *blk1 = 0, *blk2 = 0, *blk3 = 0;
    int i;

    blk3 = (char*)xaAlloc(67108865);
    for (i = 0; i < 67108865; i++)
        blk3[i] = '0' + (i % 10);

    xaFree(blk3);

    blk2 = (char*)xaAlloc(90000);
    for (i = 0; i < 90000; i++)
        blk2[i] = '0' + (i % 10);

    xaFree(blk2);

    blk1 = (char*)xaAlloc(50);
    for (i = 0; i < 50; i++)
        blk1[i] = '0' + (i % 10);

    xaFree(blk1);

    return 0;
}

static int test_realloc()
{
    char *blk1 = 0;
    int i;

    blk1 = (char*)xaAlloc(50);
    if (!blk1)
        return 1;
    for (i = 0; i < 50; i++)
        blk1[i] = '0' + (i % 10);

    xaResize(&blk1, 90000);
    if (!blk1)
        return 1;
    for (i = 0; i < 90000; i++)
        blk1[i] = '0' + (i % 10);

    xaResize(&blk1, 67108865);
    if (!blk1)
        return 1;
    for (i = 0; i < 67108865; i++)
        blk1[i] = '0' + (i % 10);

    xaFree(blk1);

    return 0;
}

testfunc cxmemtest_funcs[] = {
    { "alloc", test_malloc },
    { "usable_size", test_usable_size },
    { "free", test_free },
    { "realloc", test_realloc },
    { 0, 0 }
};
