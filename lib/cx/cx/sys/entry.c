#include "entry.h"
#include <cx/container/sarray.h>
#include <cx/string.h>

string cmdProgram = 0;
sa_string cmdArgs;

void _entryParseArgs(int argc, const char **argv)
{
    strDestroy(&cmdProgram);
    saInit(&cmdArgs, string, 1);

    if (argc < 1)
        return;

    strDup(&cmdProgram, (string)argv[0]);

    for (int i = 1; i < argc; i++) {
        saPush(&cmdArgs, string, (string)argv[i]);
    }
}

void _entryParseArgsU16(int argc, const uint16 **argv)
{
    strDestroy(&cmdProgram);
    saInit(&cmdArgs, string, 1);

    if (argc < 1)
        return;

    string temp = 0;

    strFromUTF16(&temp, argv[0], cstrLenw(argv[0]));
    strDup(&cmdProgram, temp);

    for (int i = 1; i < argc; i++) {
        strFromUTF16(&temp, argv[i], cstrLenw(argv[i]));
        saPush(&cmdArgs, string, temp);
    }
    strDestroy(&temp);
}
