#include "inventory.h"
#include <cx/utils.h>
#include "control/runlog.h"
#include "ftl/stdlib.h"
#include "subspacegame.h"

static void InvItem_dtor(stype st, stgeneric* gen, flags_t flags)
{
    InvItem* item = (InvItem*)gen->st_opaque;
    strDestroy(&item->name);
}
static sa_InvItem items;
STypeOps InvItem_ops = { .dtor = InvItem_dtor };

static LazyInitState items_init_state;
static void items_init(void* unused)
{
    saInit(&items, custom(opaque(InvItem), InvItem_ops), 13);
}

int invCount(InventoryType typ, strref name, InventoryLocation where)
{
    lazyInit(&items_init_state, items_init, NULL);

    int count  = 0;
    int nitems = saSize(items);
    for (int i = 0; i < nitems; i++) {
        if (!items.a[i].ignore && typ == items.a[i].typ &&
            (where == INVL_Any || items.a[i].loc == where) && strEq(items.a[i].name, name))
            count++;
    }
    return count;
}

bool invMove(InventoryType typ, strref name, InventoryLocation from, InventoryLocation to)
{
    lazyInit(&items_init_state, items_init, NULL);

    // iterate backwards to move the most recently added item
    for (int i = saSize(items) - 1; i >= 0; --i) {
        if (!items.a[i].ignore && typ == items.a[i].typ &&
            (from == INVL_Any || items.a[i].loc == from) && strEq(items.a[i].name, name)) {
            from           = items.a[i].loc;   // for run log
            items.a[i].loc = to;

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

void invAdd(InventoryType typ, strref name, InventoryLocation where)
{
    lazyInit(&items_init_state, items_init, NULL);

    InvItem nitem = { .typ = typ, .loc = where, .ignore = false };
    strDup(&nitem.name, name);
    saPushC(&items, opaque, &nitem);

    // don't log when it's populated during saved game load
    if (!gc.loadingGame) {
        runLogSend(&Log_InvAdd,
                   gc.curInvSource ? gc.curInvSource : _S"Other",
                   InventoryTypeNames[typ],
                   name,
                   InventoryLocationNames[where]);
    }
}

bool invRemove(InventoryType typ, strref name, InventoryLocation where)
{
    lazyInit(&items_init_state, items_init, NULL);

    // remove most recently added matching item
    for (int i = saSize(items) - 1; i >= 0; --i) {
        if (!items.a[i].ignore && typ == items.a[i].typ &&
            (where == INVL_Any || items.a[i].loc == where) && strEq(items.a[i].name, name)) {
            saRemove(&items, i);

            runLogSend(&Log_InvRemove,
                       gc.curInvSource ? gc.curInvSource : _S"Other",
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
    lazyInit(&items_init_state, items_init, NULL);

    for (int i = saSize(items) - 1; i >= 0; --i) {
        if (!items.a[i].ignore) {
            runLogSend(&Log_InvRemove,
                       gc.curInvSource ? gc.curInvSource : _S"Other",
                       InventoryTypeNames[items.a[i].typ],
                       items.a[i].name,
                       InventoryLocationNames[items.a[i].loc]);

            saRemove(&items, i);
        }
    }
}

InventoryLocation invFindAndIgnore(InventoryType typ, strref name, InventoryLocation where)
{
    lazyInit(&items_init_state, items_init, NULL);

    int nitems = saSize(items);
    for (int i = 0; i < nitems; i++) {
        if (!items.a[i].ignore && typ == items.a[i].typ &&
            (where == INVL_Any || items.a[i].loc == where) && strEq(items.a[i].name, name)) {
            items.a[i].ignore = true;
            return items.a[i].loc;
        }
    }
    return INVL_Count;
}

void invClearIgnore(void)
{
    lazyInit(&items_init_state, items_init, NULL);

    for (int i = saSize(items) - 1; i >= 0; i--) {
        items.a[i].ignore = false;
    }
}

void invReset(void)
{
    lazyInit(&items_init_state, items_init, NULL);

    saClear(&items);
}
