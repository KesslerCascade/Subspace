#pragma once

#include "cx/cx.h"

#define DEFINE_ENTRY_POINT               \
int main(int argc, const char *argv[]) { \
    _entryParseArgs(argc, argv);         \
    return entryPoint();                 \
}
