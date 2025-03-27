#include "ftl/ftl.h"
#include "hook/function.h"

typedef struct MouseControl MouseControl;

typedef void (*FUNCTYPE(MouseControl_OnRender))(MouseControl* self);
DECLFUNC(MouseControl_OnRender);
