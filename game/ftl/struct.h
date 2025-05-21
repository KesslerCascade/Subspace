#pragma once

#include <stdbool.h>

// Helper macro for getting members of structures where the offsets are found at runtime through
// disassembly. This uses the symbol resolution system to automatically calculate and cache the
// offset as a pseudo-symbol.

#define MEMBER(base, ParentType, parent, MemberType, member) \
    (*(MemberType*)((((uintptr_t)(parent)) + (symAddr(base, ParentType##_##member##_offset)))))

typedef struct GL_Color {
    float r;
    float g;
    float b;
    float a;
} GL_Color;

typedef struct Pointf {
    float x, y;
} Pointf;

typedef struct basic_string {
    char* buf;
    unsigned long len;
    char stackbuf[15];
} basic_string;
void basic_string_set(basic_string* str, const char* src);
void basic_string_destroy(basic_string* str);

typedef struct TextString {
    basic_string data;
    bool isLiteral;
} TextString;
