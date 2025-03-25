#include "ftl/ftl.h"
#include "hook/function.h"

typedef struct CFPS CFPS;

DECLSYM(CFPS_FPSControl);

typedef int (*FUNCTYPE(CFPS_GetTime))(CFPS* self);
DECLFUNC(CFPS_GetTime);