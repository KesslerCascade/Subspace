#include "ftl/ftl.h"
#include "ftl/struct.h"
#include "hook/function.h"

typedef struct ShipStatus ShipStatus;
typedef struct ShipManager ShipManager;

extern DisasmTrace ShipStatus_RenderEvadeOxygen_trace;
extern DisasmTrace ShipStatus_LinkShip_trace_v1;
extern DisasmTrace ShipStatus_LinkShip_trace_v2;

int subspace_ShipStatus_RenderHealth_pre(ShipStatus* self, bool renderText);
void subspace_ShipStatus_RenderHealth_post(ShipStatus* self, bool renderText);
void subspace_ShipStatus_OnRender_post(ShipStatus* self);

// FTL functions & wrappers below

typedef void (*FUNCTYPE(ShipStatus_RenderHealth))(ShipStatus* self, bool renderText);
DECLFUNC(ShipStatus_RenderHealth);

typedef void (*FUNCTYPE(ShipStatus_OnRender))(ShipStatus* self);
DECLFUNC(ShipStatus_OnRender);

DECLSYM(ShipStatus_LinkShip);
DECLSYM(ShipStatus_RenderEvadeOxygen);

DECLSYM(ShipStatus_ship_offset);
DECLSYM(ShipStatus_hullBox_offset);
DECLSYM(ShipStatus_hullBox_Red_offset);
DECLSYM(ShipStatus_hullLabel_offset);
DECLSYM(ShipStatus_hullLabel_Red_offset);
DECLSYM(ShipStatus_healthMask_offset);
DECLSYM(ShipStatus_healthMaskTexture_offset);

#define ShipStatus_ship(ship_status) MEMBER(ftlbase, ShipStatus, ship_status, ShipManager*, ship)