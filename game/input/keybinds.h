#pragma once

#include "control.h"
#include "subspacegame.h"

#define KEYCODE_MAX 310

enum KEYBIND_CONTEXT {
    KB_CTX_GLOBAL,
    KB_CTX_MENU,
    KB_CTX_GAME
};

enum KEYBIND_FLAGS {
    KB_SHIFT        = 0x0001,   // shift is being held
    KB_PAUSED       = 0x0002,   // the game is paused
    KB_JUMPING      = 0x0004    // the ship is jumping
};

typedef void (*keybind_cb_t)(int key, int flags);

typedef struct KeyBind KeyBind;
typedef struct KeyBind {
    KeyBind* next;      // for chaining by kb manager, do not set

    SubspaceFeature* owner;
    const char* name;

    int key;            // key is sent by the main process and should NOT be set here

    int context;        // context for which this keybind applies
    int flags_req;      // flags REQUIRED for this keybind
    int flags_exc;      // flags EXCLUDED from this keybind

    keybind_cb_t func;
} KeyBind;

bool kbRegisterBind(SubspaceFeature* owner, KeyBind* kb);
bool kbBindKey(const char *name, int key);
KeyBind* kbGetBinds(int key);
