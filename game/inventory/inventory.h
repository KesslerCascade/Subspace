#pragma once

#include <cx/container/sarray.h>
#include "ftl/ftl.h"
#include "proto.h"

typedef struct InvItem {
    string name;
    InventoryType typ;
    InventoryLocation loc;

    bool ignore;       // for scanning purposes
} InvItem;
saDeclare(InvItem);
extern STypeOps InvItem_ops;

int invCount(InventoryType typ, strref name, InventoryLocation where);
bool invMove(InventoryType typ, strref name, InventoryLocation from, InventoryLocation to);
void invAdd(InventoryType typ, strref name, InventoryLocation where);
bool invRemove(InventoryType typ, strref name, InventoryLocation where);
InventoryLocation invFindAndIgnore(InventoryType typ, strref name, InventoryLocation where);
void invRemoveAll(void);
void invClearIgnore(void);
void invReset(void);
