#pragma once

#include "ftl/ftl.h"
#include "proto.h"

typedef struct InvItem {
    const char* name;   // owned name, must dealloc
    InventoryType typ;
    InventoryLocation loc;

    bool ignore;       // for scanning purposes
} InvItem;

int invCount(InventoryType typ, const char* name, InventoryLocation where);
bool invMove(InventoryType typ, const char* name, InventoryLocation from, InventoryLocation to);
void invAdd(InventoryType typ, const char* name, InventoryLocation where);
bool invRemove(InventoryType typ, const char* name, InventoryLocation where);
InventoryLocation invFindAndIgnore(InventoryType typ, const char* name, InventoryLocation where);
void invRemoveAll(void);
void invClearIgnore(void);
void invReset(void);
