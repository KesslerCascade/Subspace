#pragma once

#include "ftl/ftl.h"
#include "hook/function.h"

typedef struct MainMenu MainMenu;

extern DisasmTrace MainMenu_OnRender_trace;

DECLSYM(MainMenu_OnRender);

typedef void (*FUNCTYPE(MainMenu_Open))(MainMenu* self);
DECLFUNC(MainMenu_Open);
