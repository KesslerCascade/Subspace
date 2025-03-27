#pragma once

// Helper macro for getting members of structures where the offsets are found at runtime through
// disassembly. This uses the symbol resolution system to automatically calculate and cache the
// offset as a pseudo-symbol.

#define MEMBER(base, ParentType, parent, MemberType, member) \
    (*(MemberType*)((((uintptr_t)(parent)) + (symAddr(base, ParentType##_##member##_offset)))))

typedef struct Pointf {
    float x, y;
} Pointf;

typedef struct basic_string {
    char* buf;
} basic_string;
