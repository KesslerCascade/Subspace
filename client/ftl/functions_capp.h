#include "ftl/ftl.h"
#include "hook/function.h"

typedef struct CApp CApp;

typedef int (*FUNCTYPE(CApp_OnExecute))(CApp* self);
DECLFUNC(CApp_OnExecute);
typedef int (*FUNCTYPE(CApp_OnLoop))(CApp* self);
DECLFUNC(CApp_OnLoop);

DECLSYM(CApp_world_offset);
DECLSYM(CApp_gui_offset);