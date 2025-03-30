#include "ftl/ftl.h"
#include "ftl/struct.h"
#include "hook/function.h"

typedef struct ShipStatus ShipStatus;
typedef struct ShipManager ShipManager;

int subspace_ShipStatus_RenderHealth_pre(ShipStatus* self, bool renderText);
int subspace_ShipStatus_RenderHealth_post(int ret, ShipStatus* self, bool renderText);

// FTL functions & wrappers below

typedef void (*FUNCTYPE(ShipStatus_RenderHealth))(ShipStatus* self, bool renderText);
DECLFUNC(ShipStatus_RenderHealth);

DECLSYM(ShipStatus_OnRender);
DECLSYM(ShipStatus_LinkShip);

DECLSYM(ShipStatus_ship_offset);
DECLSYM(ShipStatus_hullBox_offset);
DECLSYM(ShipStatus_hullBox_Red_offset);
DECLSYM(ShipStatus_hullLabel_offset);
DECLSYM(ShipStatus_hullLabel_Red_offset);
DECLSYM(ShipStatus_healthMask_offset);
DECLSYM(ShipStatus_healthMaskTexture_offset);

#define ShipStatus_ship(ship_status) MEMBER(ftlbase, ShipStatus, ship_status, ShipManager*, ship)