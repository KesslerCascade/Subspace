#include "ftl/ftl.h"
#include "ftl/sil.h"
#include "hook/hook.h"
#include "patch/patch.h"
#include "win32/winmain.h"

#include <windows.h>

DECLSYM(opengl_sys_texture_lock_wrong_cmp);

static bool validate_sil_texture_lock(addr_t base, Patch* p, PatchState* ps)
{
    addr_t gladdr = symAddr(base, opengl_sys_texture_lock_wrong_cmp);
    // addr_t d3daddr = symAddr(base, d3d_sys_texture_lock_wrong_cmp);

    if (*((uint8_t*)gladdr) != 0x0f || *((uint8_t*)(gladdr + 1)) != 0x85)   // JNE
        return false;

    return true;
}

static bool patch_sil_texture_lock(addr_t base, Patch* p, PatchState* ps)
{
    addr_t gladdr = symAddr(base, opengl_sys_texture_lock_wrong_cmp);
    // addr_t d3daddr = symAddr(base, d3d_sys_texture_lock_wrong_cmp);

    *((uint8_t*)(gladdr + 1)) = 0x84;   // JE

    /* D3D texture reading seems to be broken and crashes in 1.6.14,
    while simply failing in older versions
    if (*((uint8_t*)d3daddr) != 0x75)   // JNZ
        return false;
    *((uint8_t*)d3daddr) = 0x74;        // JZ
    */
    return true;
}

Patch patch_SILTextureLock = {
    .relevant        = AlwaysRequired,
    .validate        = validate_sil_texture_lock,
    .apply           = patch_sil_texture_lock,
    .requiredSymbols = { &SYM(opengl_sys_texture_lock_wrong_cmp), 0 }
};
