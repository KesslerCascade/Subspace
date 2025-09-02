#include "ftl/capp.h"
#include "ftl/combatcontrol.h"
#include "ftl/graphics/csurface.h"
#include "ftl/shipstatus.h"
#include "ftl/sil.h"
#include "hook/disasmtrace.h"

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

INITWRAP(CSurface_GL_BlitFrameBuffer);
Symbol SYM(CSurface_GL_BlitFrameBuffer) = {
    SYMNAME("CSurface::GL_BlitFrameBuffer"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CApp_OnRender_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN8CSurface18GL_BlitFrameBufferEPiffb" },
             { 0 } }
};
FuncInfo FUNCINFO(CSurface_GL_BlitFrameBuffer) = {
    .nargs     = 4,
    .purecdecl = true,
    .args      = { { 4, ARG_PTR, 0, true },
                  { 4, ARG_FLOAT, 0, true },
                  { 4, ARG_FLOAT, 0, true },
                  { 4, ARG_INT, 0, true } },
    .rettype   = RET_VOID
};

DisasmTrace CSurface_GL_BlitFrameBuffer_trace = {
    .c    = DTRACE_ADDR,
    .csym = &SYM(CSurface_GL_BlitFrameBuffer),
    .ops  = { { DT_OP(SKIP), .imin = 10, .imax = 18 },
             { I_CALL },                              // CALL graphics_set_fixed_color
              { DT_OP(SKIP), .imin = 0, .imax = 4 },
             { I_CALL, .argout = { DT_OUT_SYM1 } },   // CAL _framebuffer_get_texture
              { DT_OP(FINISH) } },
    .out  = { &SYM(framebuffer_get_texture) }
};

INITWRAP(CSurface_FinishFrame);
Symbol SYM(CSurface_FinishFrame) = {
    SYMNAME("CSurface::FinishFrame"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CApp_OnRender_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN8CSurface11FinishFrameEv" },
             { 0 } }
};
FuncInfo FUNCINFO(CSurface_FinishFrame) = { .nargs = 0, .stdcall = true, .rettype = RET_VOID };
