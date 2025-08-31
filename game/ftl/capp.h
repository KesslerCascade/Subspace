#pragma once

#include "ftl/capp_osdep.h"
#include "ftl/ftl.h"
#include "ftl/struct.h"
#include "hook/function.h"

typedef struct CApp CApp;
extern CApp* theApp;

int subspace_CApp_OnExecute_pre(CApp* self);
int subspace_CApp_OnExecute_post(int ret, CApp* self);
int subspace_CApp_OnLoop_pre(CApp* self);
void subspace_CApp_OnLoop_post(CApp* self);
int subspace_CApp_OnKeyDown_pre(CApp* self, int key);

// FTL functions & wrapers below

extern DisasmTrace CApp_OnExecute_trace;
extern DisasmTrace CApp_OnExecute_worldgen_trace;   // used for finding serveral different offsets
extern DisasmTrace CApp_OnExecute_rungame_trace_1;
extern DisasmTrace CApp_OnExecute_rungame_trace_2;
extern DisasmTrace CApp_OnExecute_audio_trace;
extern DisasmTrace CApp_OnExecute_audio_filehelper_trace;
extern DisasmTrace CApp_OnInit_v1_trace;
extern DisasmTrace CApp_OnInit_v2_trace;
extern DisasmTrace CApp_OnKeyDown_trace;
extern DisasmTrace CApp_GenInputEvents_trace;
extern DisasmTrace CApp_OnRender_trace;
extern DisasmTrace CApp_OnLoop_trace;
extern DisasmTrace CApp_OnLoop_trace_s0;
extern DisasmTrace CApp_OnLoop_trace_s1;
extern DisasmTrace CApp_OnLoop_trace_s5;
extern DisasmTrace CApp_OnLoop_trace_s8;
extern DisasmTrace CApp_OnLoop_menu_trace;

typedef int (*FUNCTYPE(CApp_OnInit))(CApp* self);
DECLFUNC(CApp_OnInit);
typedef int (*FUNCTYPE(CApp_OnExecute))(CApp* self);
DECLFUNC(CApp_OnExecute);
typedef void (*FUNCTYPE(CApp_OnLoop))(CApp* self);
DECLFUNC(CApp_OnLoop);
typedef void (*FUNCTYPE(CApp_OnRender))(CApp* self);
DECLFUNC(CApp_OnRender);
#define CApp_OnRender(self) FCALL(ftlbase, CApp_OnRender, self)
typedef void (*FUNCTYPE(CApp_UpdateWindowSettings))(CApp* self);
DECLFUNC(CApp_UpdateWindowSettings);

typedef void (*FUNCTYPE(CApp_OnKeyDown))(CApp* self, int key);
DECLFUNC(CApp_OnKeyDown);

typedef void (*FUNCTYPE(CApp_GenInputEvents))(CApp* self);
DECLFUNC(CApp_GenInputEvents);

DECLSYM(CApp_world_offset);
DECLSYM(CApp_gui_offset);
DECLSYM(CApp_menu_offset);
DECLSYM(CApp_framebuffer_offset);
DECLSYM(CApp_useFrameBuffer_offset);

DECLSYM(CApp_screen_x_offset);
DECLSYM(CApp_screen_y_offset);
DECLSYM(CApp_x_bar_offset);
DECLSYM(CApp_y_bar_offset);
DECLSYM(CApp_modifier_x_offset);
DECLSYM(CApp_modifier_y_offset);

DECLSYM(CApp_vtable);
DECLSYM(CApp_vtable_OnKeyDown_offset);

#define CApp_gui(capp) MEMBER(ftlbase, CApp, capp, CommandGui*, gui)
#define CApp_world(capp) MEMBER(ftlbase, CApp, capp, WorldManager*, world)
#define CApp_menu(capp)  (&MEMBER(ftlbase, CApp, capp, MainMenu, menu))
#define CApp_framebuffer(capp)    MEMBER(ftlbase, CApp, capp, int*, framebuffer)
#define CApp_useFrameBuffer(capp) MEMBER(ftlbase, CApp, capp, bool, useFrameBuffer)
#define CApp_screen_x(capp)       MEMBER(ftlbase, CApp, capp, int, screen_x)
#define CApp_screen_y(capp)       MEMBER(ftlbase, CApp, capp, int, screen_y)
#define CApp_x_bar(capp)          MEMBER(ftlbase, CApp, capp, int, x_bar)
#define CApp_y_bar(capp)          MEMBER(ftlbase, CApp, capp, int, y_bar)
#define CApp_modifier_x(capp)     MEMBER(ftlbase, CApp, capp, int, modifier_x)
#define CApp_modifier_y(capp)     MEMBER(ftlbase, CApp, capp, int, modifier_y)
