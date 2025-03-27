#include "ftl/graphics/functions_freetype.h"
#include "ftl/functions_starmap.h"

INITWRAP(freetype_easy_print);
Symbol SYM(freetype_easy_print) = {
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &StarMap_OnRender_sector_title_trace },
             //    { .type = SYMBOL_FIND_EXPORT, .name =
             //    "_ZN8freetype10easy_printEiffRKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE"
             //    },
              { 0 } }
};
FuncInfo FUNCINFO(freetype_easy_print) = {
    .nargs     = 4,
    .purecdecl = true,
    .args      = { { 4, ARG_INT, 0, false },
                  { 4, ARG_FLOAT, 0, false },
                  { 4, ARG_FLOAT, 0, false },
                  { 4, ARG_PTR, 0, false } }
};
