#include "ftl/ftl.h"
#include "hook/function.h"

typedef struct CApp CApp;

typedef int (*FUNCTYPE(CApp_OnExecute))(CApp* self);
DECLFUNC(CApp_OnExecute);
typedef void (*FUNCTYPE(CApp_OnLoop))(CApp* self);
DECLFUNC(CApp_OnLoop);

typedef void (*FUNCTYPE(CApp_OnKeyDown))(CApp* self, int key);
DECLFUNC(CApp_OnKeyDown);

typedef void (*FUNCTYPE(CApp_GenInputEvents))(CApp* self);
DECLFUNC(CApp_GenInputEvents);

DECLSYM(CApp_world_offset);
DECLSYM(CApp_gui_offset);

DECLSYM(CApp_vtable);
DECLSYM(CApp_vtable_OnKeyDown_offset);