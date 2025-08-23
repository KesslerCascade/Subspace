#pragma once

#include "ftl/ftl.h"
#include "hook/function.h"

typedef struct MainMenu MainMenu;
typedef struct ShipManager ShipManager;

extern DisasmTrace MainMenu_OnRender_trace;

DECLSYM(MainMenu_OnRender);

typedef void (*FUNCTYPE(MainMenu_Open))(MainMenu* self);
DECLFUNC(MainMenu_Open);
#define MainMenu_Open(self) FCALL(ftlbase, MainMenu_Open, self)

typedef void (*FUNCTYPE(MainMenu_Close))(MainMenu* self);
DECLFUNC(MainMenu_Close);
#define MainMenu_Close(self) FCALL(ftlbase, MainMenu_Close, self)

typedef void (*FUNCTYPE(MainMenu_OnLoop))(MainMenu* self);
DECLFUNC(MainMenu_OnLoop);

typedef int (*FUNCTYPE(MainMenu_Choice))(MainMenu* self);
DECLFUNC(MainMenu_Choice);

typedef ShipManager* (*FUNCTYPE(MainMenu_GetTutorialShip))(MainMenu* self);
DECLFUNC(MainMenu_GetTutorialShip);
#define MainMenu_GetTutorialShip(self) FCALL(ftlbase, MainMenu_GetTutorialShip, self)