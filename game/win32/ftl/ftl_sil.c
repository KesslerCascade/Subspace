#include "ftl/osdep.h"

#include "ftl/capp.h"
#include "ftl/graphics/csurface.h"
#include "ftl/sil.h"
#include "hook/disasmtrace.h"

#include "disasm.h"

Symbol SYM(thread_init) = {
    SYMNAME("thread_init"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &WinMain_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "thread_init" },
             { .type   = SYMBOL_FIND_DISASM,
                .disasm = &WinMain_fallback_trace },   // more expensive but robust method
              { 0 } }
};
FuncInfo FUNCINFO(thread_init) = { .nargs = 0, .purecdecl = true, .rettype = RET_INT };

DisasmTrace thread_init_trace = {
    .c    = DTRACE_ADDR,
    .csym = &SYM(thread_init),
    .ops  = { { DT_OP(SKIP), .imin = 3, .imax = 12 },
             { I_CALL },   // CALL sys_mutex_create
              { DT_OP(SKIP), .imin = 0, .imax = 9 },
             { I_JZ },
             { DT_OP(SKIP), .imin = 0, .imax = 7 },
             { I_CALL,
                .argout = { DT_OUT_SYM1 },
                .argcap = { DT_CAPTURE1 } },           // CALL id_array_register
              { DT_OP(SKIP), .imin = 0, .imax = 7 },
             { I_CALL, .argcap = { DT_CAPTURE2 } },   // CALL id_array_release
              { DT_OP(SKIP), .imin = 0, .imax = 7 },
             { I_CALL, .argcap = { DT_MATCH1 } },     // CALL id_array_register
              { DT_OP(SKIP), .imin = 0, .imax = 7 },
             { I_CALL, .argcap = { DT_MATCH2 } },     // CALL id_array_release
              { DT_OP(SKIP), .imin = 0, .imax = 7 },
             { I_CALL, .argcap = { DT_MATCH1 } },     // CALL id_array_register
              { DT_OP(SKIP), .imin = 0, .imax = 7 },
             { I_CALL, .argcap = { DT_MATCH2 } },     // CALL id_array_release
              { DT_OP(FINISH) } },
    .out  = { &SYM(id_array_register) }
};

// alternate version for 1.6.5 that has a different SIL which skips an array release?
DisasmTrace thread_init_trace_2 = {
    .c    = DTRACE_ADDR,
    .csym = &SYM(thread_init),
    .ops  = { { DT_OP(SKIP), .imin = 3, .imax = 12 },
             { I_CALL },   // CALL sys_mutex_create
              { DT_OP(SKIP), .imin = 0, .imax = 12 },
             { I_JZ },
             { DT_OP(SKIP), .imin = 0, .imax = 7 },
             { I_CALL,
                .argout = { DT_OUT_SYM1 },
                .argcap = { DT_CAPTURE1 } },           // CALL id_array_register
              { DT_OP(SKIP), .imin = 0, .imax = 12 },
             { I_CALL, .argcap = { DT_MATCH1 } },     // CALL id_array_register
              { DT_OP(SKIP), .imin = 0, .imax = 7 },
             { I_CALL, .argcap = { DT_CAPTURE2 } },   // CALL id_array_release
              { DT_OP(SKIP), .imin = 0, .imax = 7 },
             { I_CALL, .argcap = { DT_MATCH1 } },     // CALL id_array_register
              { DT_OP(SKIP), .imin = 0, .imax = 7 },
             { I_CALL, .argcap = { DT_MATCH2 } },     // CALL id_array_release
              { DT_OP(FINISH) } },
    .out  = { &SYM(id_array_register) }
};

Symbol SYM(id_array_register) = {
    SYMNAME("id_array_register"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &thread_init_trace },
             { .type = SYMBOL_FIND_DISASM, .disasm = &thread_init_trace_2 },
             { .type = SYMBOL_FIND_EXPORT, .name = "id_array_register" },
             { 0 } }
};
FuncInfo FUNCINFO(id_array_register) = {
    .nargs     = 2,
    .purecdecl = true,
    .args      = { { 4, ARG_PTR, 0, true }, { 4, ARG_PTR, 0, true } },
    .rettype   = RET_INT
};

DisasmTrace new_params_trace = {
    .c    = DTRACE_CALLS,
    .csym = &SYM(id_array_register),
    .mod  = DTRACE_MOD_FUNCSTART,
    .ops  = { { I_PUSH, .argf = { ARG_REG }, .args = { { REG_EBP } }, .outip = DT_OUT_SYM1 },
             { DT_OP(SKIP), .imin = 2, .imax = 13 },
             { I_MOV, .argf = { ARG_REG, ARG_ADDR }, .args = { { REG_ESP }, { .addr = 0x7c } } },
             { DT_OP(SKIP), .imin = 0, .imax = 8 },
             { I_CALL, .argout = { DT_OUT_SYM2 } },
             { DT_OP(FINISH) } },
    .out  = { &SYM(new_params),   // DT_OUT_SYM1
              &SYM(mem_alloc) }
};

Symbol SYM(new_params) = {
    SYMNAME("new_params"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &new_params_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "new_params" },
             { 0 } }
};
FuncInfo FUNCINFO(new_params) = {
    .nargs     = 2,
    .purecdecl = true,
    .args      = { { 4, ARG_PTR, 0, true }, { 4, ARG_PTR, 0, true } },
    .rettype   = RET_INT
};

Symbol SYM(mem_alloc) = {
    SYMNAME("mem_alloc"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &new_params_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "mem_alloc" },
             { 0 } }
};
FuncInfo FUNCINFO(mem_alloc) = {
    .nargs     = 3,
    .purecdecl = true,
    .args      = { { 8, ARG_INT, 0, true }, { 4, ARG_INT, 0, true }, { 4, ARG_INT, 0, true } },
    .rettype   = RET_PTR
};

// These are REALLY hard to find. They're only referenced from two functions,
// userdata_save_screenshot, and userdata_save_savefile.
// Both of those are dead code that is never called from anywhere in the FTL executable.
// That makes them very difficult to track down as they aren't detected as functions and aren't in
// our hashtable, so the usual methods for location function start addresses don't work.

// Thought a lot of hoops we can find out where new_params lives, and look for that, as it's called
// fairly early in the function. Unfortunately that makes it harder to differentiante because we
// can't rewind to the function start to check the parameter (7) for the specific call we're after.

DisasmTrace userdata_save_screenshot_trace = {
    .c    = DTRACE_CALLS,
    .csym = &SYM(new_params),
    .ops  = { { I_CALL },
             { I_TEST, .argf = { ARG_REG, ARG_REG }, .args = { { REG_EAX }, { REG_EAX } } },
             { DT_OP(SKIP), .imin = 0, .imax = 5 },
             { I_JZ },
             { DT_OP(SKIP), .imin = 0, .imax = 2 },
             { I_MOV,
                .argf   = { 0, ARG_REG },
                .args   = { { 0 }, { REG_EBP } },
                .argcap = { 0, DT_CAPTURE1 } },        // texture_id
              { DT_OP(SKIP), .imin = 0, .imax = 4 },
             { I_CALL, .argout = { DT_OUT_SYM1 } },   // CALL texture_lock_readonly
              { I_TEST, .argf = { ARG_REG, ARG_REG }, .args = { { REG_EAX }, { REG_EAX } } },
             { DT_OP(SKIP), .imin = 1, .imax = 5 },
             { I_MOV, .argf = { 0, ARG_ADDR }, .argcap = { 0, DT_MATCH1 } },   // texture_id
              { DT_OP(SKIP), .imin = 0, .imax = 3 },
             { I_CALL, .argout = { DT_OUT_SYM2 } },   // CALL texture_width
              { DT_OP(SKIP), .imin = 0, .imax = 5 },
             { I_MOV, .argf = { 0, ARG_ADDR }, .argcap = { 0, DT_MATCH1 } },   // texture_id
              { DT_OP(SKIP), .imin = 0, .imax = 3 },
             { I_CALL, .argout = { DT_OUT_SYM3 } },   // CALL texture_height
              { DT_OP(SKIP), .imin = 0, .imax = 6 },
             { I_MOV, .argf = { 0, ARG_ADDR }, .args = { { 0 }, { .addr = 4 } } },
             { DT_OP(SKIP), .imin = 0, .imax = 10 },
             { I_CALL, .argf = { ARG_ADDR }, .argsym = { &SYM(mem_alloc) } },   // CALL mem_alloc
              { DT_OP(SKIP), .imin = 30, .imax = 40 },
             { I_MOV, .argf = { 0, ARG_ADDR }, .argcap = { 0, DT_MATCH1 } },    // texture_id
              { DT_OP(SKIP), .imin = 0, .imax = 3 },
             { I_CALL, .argout = { DT_OUT_SYM4 } },   // CALL texture_unlock
              { DT_OP(FINISH) } },
    .out  = { &SYM(texture_lock_readonly),             // DT_OUT_SYM1
              &SYM(texture_width),                     // DT_OUT_SYM2
              &SYM(texture_height),                    // DT_OUT_SYM3
              &SYM(texture_unlock) }
};

INITWRAP(texture_lock_readonly);
Symbol SYM(texture_lock_readonly) = {
    SYMNAME("texture_lock_readonly"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &userdata_save_screenshot_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "texture_lock_readonly" },
             { 0 } }
};
FuncInfo FUNCINFO(texture_lock_readonly) = {
    .nargs     = 1,
    .purecdecl = true,
    .args      = { { 4, ARG_INT, 0, true } },
    .rettype   = RET_PTR
};

INITWRAP(texture_width);
Symbol SYM(texture_width) = {
    SYMNAME("texture_width"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &userdata_save_screenshot_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "texture_width" },
             { 0 } }
};
FuncInfo FUNCINFO(texture_width) = { .nargs     = 1,
                                     .purecdecl = true,
                                     .args      = { { 4, ARG_INT, 0, true } },
                                     .rettype   = RET_INT };

INITWRAP(texture_height);
Symbol SYM(texture_height) = {
    SYMNAME("texture_height"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &userdata_save_screenshot_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "texture_height" },
             { 0 } }
};
FuncInfo FUNCINFO(texture_height) = { .nargs     = 1,
                                      .purecdecl = true,
                                      .args      = { { 4, ARG_INT, 0, true } },
                                      .rettype   = RET_INT };

INITWRAP(texture_unlock);
Symbol SYM(texture_unlock) = {
    SYMNAME("texture_unlock"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &userdata_save_screenshot_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "texture_unlock" },
             { 0 } }
};
FuncInfo FUNCINFO(texture_unlock) = { .nargs     = 1,
                                      .purecdecl = true,
                                      .args      = { { 4, ARG_INT, 0, true } },
                                      .rettype   = RET_VOID };

INITWRAP(framebuffer_get_texture);
Symbol SYM(framebuffer_get_texture) = {
    SYMNAME("framebuffer_get_texture"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CSurface_GL_BlitFrameBuffer_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "framebuffer_get_texture" },
             { 0 } }
};
FuncInfo FUNCINFO(framebuffer_get_texture) = {
    .nargs     = 1,
    .purecdecl = true,
    .args      = { { 4, ARG_INT, 0, true } },
    .rettype   = RET_INT
};

DisasmTrace texture_lock_readonly_trace = {
    .c    = DTRACE_ADDR,
    .csym = &SYM(texture_lock_readonly),
    .ops  = { { DT_OP(SKIP), .imin = 10, .imax = 35, .flow = DT_FLOW_JMP_BOTH },
             { I_MOV,
                .argf = { ARG_MATCH, ARG_ADDR },
                .args = { { .base = REG_ESP, .idx = REG_UNDEF, .disp = 4 }, { .addr = 1 } } },
             { DT_OP(SKIP), .imin = 0, .imax = 6 },
             { I_CALL, .argout = { DT_OUT_SYM1 } },   // CALL sys_texture_lock
              { DT_OP(FINISH) } },
    .out  = { &SYM(sys_texture_lock) }
};

INITWRAP(sys_texture_lock);
Symbol SYM(sys_texture_lock) = {
    SYMNAME("sys_texture_lock"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &texture_lock_readonly_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "sys_texture_lock" },
             { 0 } }
};
FuncInfo FUNCINFO(sys_texture_lock) = {
    .nargs     = 6,
    .purecdecl = true,
    .args      = { { 4, ARG_PTR, 0, true },
                  { 4, ARG_INT, 0, true },
                  { 4, ARG_INT, 0, true },
                  { 4, ARG_INT, 0, true },
                  { 4, ARG_INT, 0, true },
                  { 4, ARG_INT, 0, true } },
    .rettype   = RET_PTR
};

// Windows-specific
DECLSYM(opengl_sys_texture_lock);
DECLSYM(d3d_sys_texture_lock);
DisasmTrace sys_texture_lock_trace = {
    .c    = DTRACE_ADDR,
    .csym = &SYM(sys_texture_lock),
    .ops  = { { DT_OP(SKIP), .imin = 4, .imax = 25 },
             { DT_OP(LABEL), .val = 1 },
             { I_JNZ },
             { DT_OP(SKIP), .imin = 0, .imax = 16 },
             { I_CALL, .argout = { DT_OUT_SYM1 } },   // CALL opengl_sys_texture_lock
              { DT_OP(GOTO), .val = 1 },
             { DT_OP(JMP) },
             { DT_OP(SKIP), .imin = 0, .imax = 13 },
             { I_CALL, .argout = { DT_OUT_SYM2 } },   // CALL d3d_sys_texture_lock
              { DT_OP(FINISH) } },
    .out  = { &SYM(opengl_sys_texture_lock),           // DT_OUT_SYM1
              &SYM(d3d_sys_texture_lock) }
};

Symbol SYM(opengl_sys_texture_lock) = {
    SYMNAME("opengl_sys_texture_lock"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &sys_texture_lock_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "opengl_sys_texture_lock" },
             { 0 } }
};
Symbol SYM(d3d_sys_texture_lock) = {
    SYMNAME("d3d_sys_texture_lock"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &sys_texture_lock_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "d3d_sys_texture_lock" },
             { 0 } }
};

// the SIL code in FTL has this check backwards. It's fixed in the current source but apparently not
// the verison they compiled against. Mark the instruction IP so it can be easily patched.
DECLSYM(opengl_sys_texture_lock_wrong_cmp);
DisasmTrace opengl_sys_texture_lock_trace = {
    .c    = DTRACE_ADDR,
    .csym = &SYM(opengl_sys_texture_lock),
    .ops  = { { DT_OP(SKIP), .imin = 28, .imax = 35 },
             { I_CALL, .argf = { ARG_ADDR }, .argsym = { &SYM(mem_alloc) } },
             { DT_OP(SKIP), .imin = 4, .imax = 12 },
             { I_CMP, .argf = { ARG_ADDR, ARG_ADDR }, .args = { { .addr = 0x16 }, { .addr = 0 } } },
             { I_JNZ, .outip = DT_OUT_SYM1 },   // this should be a JZ!!!!!!
              { DT_OP(FINISH) } },
    .out  = { &SYM(opengl_sys_texture_lock_wrong_cmp) }
};

// D3D verison makes the same mistake, but is also correct in current source...
DECLSYM(d3d_sys_texture_lock_wrong_cmp);
DisasmTrace d3d_sys_texture_lock_trace = {
    .c    = DTRACE_ADDR,
    .csym = &SYM(d3d_sys_texture_lock),
    .ops  = { { DT_OP(SKIP), .imin = 16, .imax = 28 },
             { I_CALL, .argf = { ARG_ADDR }, .argsym = { &SYM(mem_alloc) } },
             { DT_OP(SKIP), .imin = 3, .imax = 10 },
             { I_CMP, .argf = { ARG_ADDR, ARG_ADDR }, .args = { { .addr = 0x1f }, { .addr = 0 } } },
             { I_JNZ, .outip = DT_OUT_SYM1 },   // this should be a JZ!!!!!!
              { DT_OP(FINISH) } },
    .out  = { &SYM(d3d_sys_texture_lock_wrong_cmp) }
};

Symbol SYM(opengl_sys_texture_lock_wrong_cmp) = {
    SYMNAME("opengl_sys_texture_lock (bugfix)"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &opengl_sys_texture_lock_trace }, { 0 } }
};
Symbol SYM(d3d_sys_texture_lock_wrong_cmp) = {
    SYMNAME("d3d_sys_texture_lock (bugfix)"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &d3d_sys_texture_lock_trace }, { 0 } }
};

bool patch_sil_texture_lock(addr_t base)
{
    addr_t gladdr = symAddr(base, opengl_sys_texture_lock_wrong_cmp);
    // addr_t d3daddr = symAddr(base, d3d_sys_texture_lock_wrong_cmp);

    if (gladdr == 0)   // || d3daddr == 0)
        return false;

    if (*((uint8_t*)gladdr) != 0x0f || *((uint8_t*)(gladdr + 1)) != 0x85)   // JNE
        return false;
    *((uint8_t*)(gladdr + 1)) = 0x84;                                       // JE

    /* D3D texture reading seems to be broken and crashes in 1.6.14,
    while simply failing in older versions
    if (*((uint8_t*)d3daddr) != 0x75)   // JNZ
        return false;
    *((uint8_t*)d3daddr) = 0x74;        // JZ
    */
    return true;
}
