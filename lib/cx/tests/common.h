#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    const char* name;
    int(*func)();
} testfunc;

extern testfunc TEST_FUNCS[];

int TEST_FILE(int argc, char *argv[])
{
    char buf[64];
    testfunc *f;
    char *t = 0;
    int ntests = 0, testnum = 0;

    if (argc < 2) {
        printf("Available tests:\n");
        for (f = TEST_FUNCS; f->name; ++f) {
            printf("%3d. %s\n", ntests, f->name);
            ++ntests;
        }
        printf("To run a test, enter the test number: ");
        fflush(stdout);
        do {
            if (!fgets(buf, 64, stdin))
                return -1;
        } while (buf[0] == '\n' || buf[0] == '\r');
        testnum = strtol(buf, &t, 10);
        if (t == buf) {
            printf("Couldn't parse that input as a number\n");
            return -1;
        }
        if (testnum < 0 || testnum >= ntests) {
            printf("%3d is an invalid test number.\n", testnum);
            return -1;
        }
        return TEST_FUNCS[testnum].func();
    }

    for (f = TEST_FUNCS; f->name; ++f) {
        if (!strcmp(argv[1], f->name)) {
            return f->func();
        }
    }
    printf("Invalid test name!\n");
    return -1;
}
