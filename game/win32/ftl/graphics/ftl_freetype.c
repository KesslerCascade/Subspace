#include "ftl/combatcontrol.h"
#include "ftl/graphics/freetype.h"
#include "ftl/mainmenu.h"
#include "ftl/starmap.h"

INITWRAP(freetype_easy_print);
Symbol SYM(freetype_easy_print) = {
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &StarMap_OnRender_sector_title_trace },
             { .type = SYMBOL_FIND_EXPORT,
                .name =
                    "_ZN8freetype10easy_printEiffRKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE" },
             { 0 } }
};
FuncInfo FUNCINFO(freetype_easy_print) = {
    .nargs      = 4,
    .purecdecl  = true,
    .wasinlined = true,
    .args       = { { 4, ARG_INT, 0, true },
                   { 4, ARG_FLOAT, 0, true },
                   { 4, ARG_FLOAT, 0, true },
                   { 4, ARG_PTR, 0, true } }
};

INITWRAP(freetype_easy_printRightAlign);
Symbol SYM(freetype_easy_printRightAlign) = {
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CombatControl_RenderTarget_trace },
             { .type = SYMBOL_FIND_EXPORT,
                .name =
                    "_ZN8freetype20easy_printRightAlignEiffRKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE" },
             { 0 } }
};
FuncInfo FUNCINFO(freetype_easy_printRightAlign) = {
    .nargs      = 4,
    .purecdecl  = true,
    .wasinlined = true,
    .args       = { { 4, ARG_INT, 0, true },
                   { 4, ARG_FLOAT, 0, true },
                   { 4, ARG_FLOAT, 0, true },
                   { 4, ARG_PTR, 0, true } }
};

INITWRAP(freetype_easy_printCenter);
Symbol SYM(freetype_easy_printCenter) = {
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CombatControl_RenderTarget_trace },
             { .type = SYMBOL_FIND_EXPORT,
                .name =
                    "_ZN8freetype16easy_printCenterEiffRKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE" },
             { 0 } }
};
FuncInfo FUNCINFO(freetype_easy_printCenter) = {
    .nargs      = 4,
    .purecdecl  = true,
    .wasinlined = true,
    .args       = { { 4, ARG_INT, 0, true },
                   { 4, ARG_FLOAT, 0, true },
                   { 4, ARG_FLOAT, 0, true },
                   { 4, ARG_PTR, 0, true } }
};

INITWRAP(freetype_easy_measurePrintLines);
Symbol SYM(freetype_easy_measurePrintLines) = {
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &MainMenu_OnRender_trace },
             { .type = SYMBOL_FIND_EXPORT,
                .name =
                    "_ZN8freetype22easy_measurePrintLinesEiffiRKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE" },
             { 0 } }
};
FuncInfo FUNCINFO(freetype_easy_measurePrintLines) = {
    .nargs      = 5,
    .purecdecl  = true,
    .wasinlined = true,
    .args       = { { 4, ARG_INT, 0, true },
                   { 4, ARG_FLOAT, 0, true },
                   { 4, ARG_FLOAT, 0, true },
                   { 4, ARG_INT, 0, true },
                   { 4, ARG_PTR, 0, true } }
};
