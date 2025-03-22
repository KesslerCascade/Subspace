#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef uintptr_t addr_t;
extern addr_t ftlbase;

typedef struct SegInfo {
    addr_t start;
    addr_t end;
    size_t size;
} SegInfo;

typedef (*entrypoint)();

addr_t loadProgram(const char* filename);
entrypoint getProgramEntry(addr_t baseaddr);

bool getCodeSeg(addr_t baseaddr, SegInfo* si);
bool getDataSeg(addr_t baseaddr, SegInfo* si);
bool getRDataSeg(addr_t baseaddr, SegInfo* si);

uintptr_t makeSegWritable(SegInfo* si);
bool restoreSegAccess(SegInfo* si, uintptr_t origaccess);
