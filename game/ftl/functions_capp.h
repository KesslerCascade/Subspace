#include "ftl/ftl.h"
#include "ftl/struct.h"
#include "hook/function.h"

typedef struct CApp CApp;

extern DisasmTrace CApp_OnExecute_worldgen_trace;   // used for finding serveral different offsets
extern DisasmTrace CApp_OnExecute_audio_trace;
extern DisasmTrace CApp_OnInit_v1_trace;
extern DisasmTrace CApp_OnInit_v2_trace;
extern DisasmTrace CApp_OnKeyDown_trace;
extern DisasmTrace CApp_GenInputEvents_trace;
extern DisasmTrace CApp_OnRender_trace;
extern DisasmTrace CApp_OnLoop_trace;

typedef int (*FUNCTYPE(CApp_OnInit))(CApp* self);
DECLFUNC(CApp_OnInit);
typedef int (*FUNCTYPE(CApp_OnExecute))(CApp* self);
DECLFUNC(CApp_OnExecute);
typedef void (*FUNCTYPE(CApp_OnLoop))(CApp* self);
DECLFUNC(CApp_OnLoop);
typedef void (*FUNCTYPE(CApp_OnRender))(CApp* self);
DECLFUNC(CApp_OnRender);

typedef void (*FUNCTYPE(CApp_OnKeyDown))(CApp* self, int key);
DECLFUNC(CApp_OnKeyDown);

typedef void (*FUNCTYPE(CApp_GenInputEvents))(CApp* self);
DECLFUNC(CApp_GenInputEvents);

DECLSYM(CApp_world_offset);
DECLSYM(CApp_gui_offset);

DECLSYM(CApp_vtable);
DECLSYM(CApp_vtable_OnKeyDown_offset);