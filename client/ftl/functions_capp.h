#include "capp/capp.h"
#include "ftl/ftl.h"
#include "hook/function.h"

typedef int (*FUNCTYPE(CApp_OnExecute))(CApp* self);
DECLFUNC(CApp_OnExecute);
