#include "blackbox.h"

#include "cx/container/hashtable.h"
#include "cx/utils/lazyinit.h"
#include "cx/thread/mutex.h"

// do not make this static; the symbol should be visible for debugging purposes!
char dbgBlackBox[BLACKBOX_SIZE];
#define dbgBlackBoxHead (((uint16*)dbgBlackBox)[0])
#define dbgBlackBoxTail (((uint16*)dbgBlackBox)[1])

// This is not a particularly fast or efficient allocation algorithm,
// but it's simple and works reasonably well for a fixed size block.
// bboxSet/Delete should not be called that often.

typedef struct BBoxFreelistNode BBoxFreelistNode;
typedef struct BBoxFreelistNode {
    BBoxFreelistNode *next;
    uint16 start;
    uint16 size;
} BBoxFreelistNode;

static Mutex bbmtx;
static hashtable bbindex;
static BBoxFreelistNode *freelist;

static void freeSpaceAdd(uint16 start, uint16 size)
{
    BBoxFreelistNode *n = freelist;
    BBoxFreelistNode *prev = 0;

    // find where to insert it
    while (n) {
        if (n->start > start)
            break;
        prev = n;
        n = n->next;
    }

    // see if we can coalsece into previous block
    if (prev && prev->start + prev->size == start) {
        prev->size += size;
        return;
    }
    // see if we can coalsece into next block
    if (n && start + size == n->start) {
        n->start = start;
        n->size += size;
        return;
    }

    // insert a new one
    BBoxFreelistNode *fn = xaAlloc(sizeof(BBoxFreelistNode));
    fn->start = start;
    fn->size = size;
    fn->next = n;
    if (prev)
        prev->next = fn;
    else
        freelist = fn;          // this is the new head
}

static void freeSpaceRemove(uint16 start, uint16 size)
{
    BBoxFreelistNode *n = freelist;
    BBoxFreelistNode *prev = 0;

    // find where to remove it from
    while (n) {
        if (n->start >= start + size)
            return;             // past the end of the region

        if (start < n->start + n->size) {
            // are we even to the right place in the list yet?

            if (start <= n->start && start + size >= n->start + n->size) {
                BBoxFreelistNode *next = n->next;
                // covers the entire block, so remove it
                if (prev)
                    prev->next = n->next;
                else
                    freelist = n->next;
                xaFree(n);
                n = next;
                continue;
            } else if (start <= n->start && start + size > n->start) {
                // removing the front of the block
                n->size = n->start + n->size - (start + size);
                n->start = start + size;
            } else if (start < n->start + n->size && start + size >= n->start + n->size) {
                // removing the end of the block
                n->size = start - n->start;
            } else if (start > n->start && start + size < n->start + n->size) {
                // splitting the block :/
                BBoxFreelistNode *fn = xaAlloc(sizeof(BBoxFreelistNode));
                fn->start = start + size;
                fn->size = n->start + n->size - (start + size);
                fn->next = n->next;
                n->size = start - n->start;
                n->next = fn;
            }
        }

        prev = n;
        n = n->next;
    }
}

void bboxInit()
{
    mutexInit(&bbmtx);
    htInit(&bbindex, string, uint16, 0);
    freelist = xaAlloc(sizeof(BBoxFreelistNode));
    freelist->next = 0;
    freelist->start = sizeof(int16) * 2;            // after the head and tail pointers
    freelist->size = BLACKBOX_SIZE - sizeof(int16) * 2;
}

static void _bboxDeleteInternal(uint16 idx)
{
    BlackBoxEnt *ent = (BlackBoxEnt*)&dbgBlackBox[idx];

    htRemove(&bbindex, string, (string)ent->name);

    if (dbgBlackBoxHead == idx)
        dbgBlackBoxHead = ent->next;
    if (dbgBlackBoxTail == idx)
        dbgBlackBoxTail = ent->prev;

    if (ent->prev) {
        BlackBoxEnt *prev = (BlackBoxEnt*)&dbgBlackBox[ent->prev];
        prev->next = ent->next;
    }

    if (ent->next) {
        BlackBoxEnt *next = (BlackBoxEnt*)&dbgBlackBox[ent->next];
        next->prev = ent->prev;
    }

    freeSpaceAdd(idx, bboxEntSize(ent));
}

_Use_decl_annotations_
void bboxSet(strref name, strref val, uint8 flags)
{
    mutexAcquire(&bbmtx);

    BlackBoxEnt *ent = 0, *tail;
    uint16 idx = 0;

    if (htFind(bbindex, strref, name, uint16, &idx)) {
        // already exists in index
        ent = (BlackBoxEnt*)&dbgBlackBox[idx];
        // is it big enough that we can overwrite in place?
        if (ent->vallen >= strLen(val) + 1) {
            // yes, overwrite it
            uint16 origsz = bboxEntSize(ent);
            devAssert(ent->namelen == strLen(name) + 1);
            ent->vallen = strLen(val) + 1;
            ent->flags = flags;
            strCopyOut(val, 0, (uint8*)bboxGetVal(ent), ent->vallen);
            if (bboxEntSize(ent) < origsz)
                freeSpaceAdd(idx + bboxEntSize(ent), origsz - bboxEntSize(ent));
            goto out;
        } else {
            // no, remove it and fall through add new
            _bboxDeleteInternal(idx);
        }
    }

    // bit fit algorithm
    BBoxFreelistNode *best = 0, *n = freelist;
    uint16 needsz = (uint16)offsetof(BlackBoxEnt, name) + strLen(name) + strLen(val) + 2;
    while (n) {
        if (n->size >= needsz) {
            if (!best)
                best = n;
            else if (n->size < best->size)
                best = n;
            if (best->size == needsz)
                break;              // early out for exact match
        }
        n = n->next;
    }

    if (best) {
        // add a new node
        ent = (BlackBoxEnt*)&dbgBlackBox[best->start];

        // update tail
        ent->prev = dbgBlackBoxTail;
        ent->next = 0;
        if (dbgBlackBoxTail) {
            tail = (BlackBoxEnt*)&dbgBlackBox[dbgBlackBoxTail];
            tail->next = best->start;
        }
        dbgBlackBoxTail = best->start;
        if (!dbgBlackBoxHead)
            dbgBlackBoxHead = best->start;      // first item, guess we're head too

        ent->namelen = strLen(name) + 1;
        ent->vallen = strLen(val) + 1;
        ent->flags = flags;
        strCopyOut(name, 0, (uint8*)ent->name, ent->namelen);
        strCopyOut(val, 0, (uint8*)bboxGetVal(ent), ent->vallen);

        // add to index
        htInsert(&bbindex, string, (string)ent->name, uint16, best->start);

        freeSpaceRemove(best->start, needsz);
    } else {
        // blackbox is full
        // TODO: warning message? this probably shouldn't be an assert
    }

out:
    mutexRelease(&bbmtx);
}

_Use_decl_annotations_
void bboxDelete(strref name)
{
    mutexAcquire(&bbmtx);
    uint16 idx = 0;

    if (htFind(bbindex, strref, name, uint16, &idx))
        _bboxDeleteInternal(idx);

    mutexRelease(&bbmtx);
}
