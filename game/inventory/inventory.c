#include "inventory.h"
#include "control/runlog.h"
#include "ftl/stdlib.h"
#include "subspacegame.h"

static InvItem* items;
static int nitems;
static int nalloc;

int invCount(InventoryType typ, const char* name, InventoryLocation where)
{
    int count = 0;
    for (int i = 0; i < nitems; i++) {
        if (!items[i].ignore && typ == items[i].typ &&
            (where == INVL_Any || items[i].loc == where) && strcmp(items[i].name, name) == 0)
            count++;
    }
    return count;
}

bool invMove(InventoryType typ, const char* name, InventoryLocation from, InventoryLocation to)
{
    // iterate backwards to move the most recently added item
    for (int i = nitems - 1; i >= 0; --i) {
        if (!items[i].ignore && typ == items[i].typ && (from == INVL_Any || items[i].loc == from) &&
            strcmp(items[i].name, name) == 0) {
            from         = items[i].loc;   // for run log
            items[i].loc = to;

            runLogSend(&Log_InvMove,
                       InventoryTypeNames[typ],
                       name,
                       InventoryLocationNames[from],
                       InventoryLocationNames[to]);

            return true;
        }
    }
    return false;
}

void invAdd(InventoryType typ, const char* name, InventoryLocation where)
{
    if (nitems >= nalloc) {
        nalloc = nalloc ? nalloc * 2 : 8;
        items  = realloc(items, nalloc * sizeof(InvItem));
    }
    items[nitems].name   = strdup(name);
    items[nitems].typ    = typ;
    items[nitems].loc    = where;
    items[nitems].ignore = false;
    nitems++;

    // don't log when it's populated during saved game load
    if (!gc.loadingGame) {
        runLogSend(&Log_InvAdd,
                   gc.curInvSource ? gc.curInvSource : "Other",
                   InventoryTypeNames[typ],
                   name,
                   InventoryLocationNames[where]);
    }
}

bool invRemove(InventoryType typ, const char* name, InventoryLocation where)
{
    for (int i = nitems - 1; i >= 0; --i) {
        if (!items[i].ignore && typ == items[i].typ &&
            (where == INVL_Any || items[i].loc == where) && strcmp(items[i].name, name) == 0) {
            free((void*)items[i].name);
            memcpy(&items[i], &items[i + 1], (nitems - i - 1) * sizeof(InvItem));
            nitems--;

            runLogSend(&Log_InvRemove,
                       gc.curInvSource ? gc.curInvSource : "Other",
                       InventoryTypeNames[typ],
                       name,
                       InventoryLocationNames[where]);

            return true;
        }
    }
    return false;
}

void invRemoveAll(void)
{
    for (int i = nitems - 1; i >= 0; --i) {
        if (!items[i].ignore) {
            runLogSend(&Log_InvRemove,
                       gc.curInvSource ? gc.curInvSource : "Other",
                       InventoryTypeNames[items[i].typ],
                       items[i].name,
                       InventoryLocationNames[items[i].loc]);

            free((void*)items[i].name);
            memcpy(&items[i], &items[i + 1], (nitems - i - 1) * sizeof(InvItem));
            nitems--;
        }
    }
}

InventoryLocation invFindAndIgnore(InventoryType typ, const char* name, InventoryLocation where)
{
    for (int i = 0; i < nitems; i++) {
        if (!items[i].ignore && typ == items[i].typ &&
            (where == INVL_Any || items[i].loc == where) && strcmp(items[i].name, name) == 0) {
            items[i].ignore = true;
            return items[i].loc;
        }
    }
    return INVL_Count;
}

void invClearIgnore(void)
{
    for (int i = 0; i < nitems; i++) {
        items[i].ignore = false;
    }
}

void invReset(void)
{
    for (int i = 0; i < nitems; i++) {
        free((void*)items[i].name);
    }
    free(items);
    items  = NULL;
    nitems = 0;
    nalloc = 0;
}
