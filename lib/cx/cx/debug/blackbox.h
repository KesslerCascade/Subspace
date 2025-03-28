#pragma once

#include "cx/cx.h"
#include "cx/string.h"

CX_C_BEGIN

#define BLACKBOX_SIZE 65535
extern char dbgBlackBox[];

// blackbox internal structures for offline parsing
typedef struct BlackBoxEnt {
    uint16 prev;
    uint16 next;
    uint8 flags;
    uint8 namelen;
    uint16 vallen;
    char name[1];
    // char val[];
} BlackBoxEnt;
#define bboxGetVal(ent) ((char*)ent + offsetof(BlackBoxEnt, name) + ent->namelen)
#define bboxEntSize(ent) (offsetof(BlackBoxEnt, name) + ent->namelen + ent->vallen)

enum BLACKBOX_FLAGS {
    BBox_Private        = 0x01, // potentially private; allow user to opt-out of including in crash reports
};

void bboxInit();
void bboxSet(_In_ strref name, _In_ strref val, uint8 flags);
void bboxDelete(_In_ strref name);

CX_C_END
