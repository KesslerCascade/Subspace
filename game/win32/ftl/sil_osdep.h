#pragma once

#include "ftl/ftl.h"
#include "hook/function.h"

DECLSYM(opengl_sys_texture_lock);
DECLSYM(d3d_sys_texture_lock);
DECLSYM(opengl_sys_texture_lock_wrong_cmp);
DECLSYM(d3d_sys_texture_lock_wrong_cmp);

DECLSYM(pglReadPixels);
DECLSYM(glReadPixels);
DECLSYM(opengl_sys_graphics_read_pixels);

typedef void* (*FUNCTYPE(windows_window))(void);
DECLFUNC(windows_window);
#define windows_window() FCALL(ftlbase, windows_window)
