#include "ftl/ftl.h"
#include "ftl/functions_shipstatus.h"

typedef struct ShipStatus ShipStatus;
typedef struct ShipManager ShipManager;

int subspace_ShipStatus_RenderHealth_pre(ShipStatus* self, bool renderText);
int subspace_ShipStatus_RenderHealth_post(int ret, ShipStatus* self, bool renderText);
