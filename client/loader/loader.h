#pragma once

#include <stdint.h>

typedef uintptr_t addr_t;
extern addr_t ftlbase;

typedef (*entrypoint)();

addr_t loadProgram(const char* filename);
entrypoint getProgramEntry(addr_t baseaddr);