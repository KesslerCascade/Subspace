#include "ftl/combatcontrol.h"
#include "ftl/graphics/csurface.h"
#include "ftl/shipstatus.h"

INITWRAP(CSurface_GL_SetColor);
Symbol SYM(CSurface_GL_SetColor) = {
    SYMNAME("CSurface::GL_SetColor"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &ShipStatus_RenderEvadeOxygen_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN8CSurface11GL_SetColorE8GL_Color" },
             { 0 } }
};
FuncInfo FUNCINFO(CSurface_GL_SetColor) = {
    .nargs      = 1,
    .purecdecl  = true,
    .wasinlined = true,
    .args       = { { 16, ARG_STRUCT, 0, true } }
};
