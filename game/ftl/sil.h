#pragma once

#include "ftl/ftl.h"
#include "ftl/sil_osdep.h"
#include "hook/function.h"

typedef struct IDArray IDArray;
typedef struct UserDataParams UserDataParams;

// global SIL functions
typedef void* (*FUNCTYPE(texture_lock_readonly))(int texture_id);
DECLFUNC(texture_lock_readonly);
#define texture_lock_readonly(texture_id) FCALL(ftlbase, texture_lock_readonly, texture_id)

typedef int (*FUNCTYPE(texture_width))(int texture_id);
DECLFUNC(texture_width);
#define texture_width(texture_id) FCALL(ftlbase, texture_width, texture_id)

typedef int (*FUNCTYPE(texture_height))(int texture_id);
DECLFUNC(texture_height);
#define texture_height(texture_id) FCALL(ftlbase, texture_height, texture_id)

typedef void (*FUNCTYPE(texture_unlock))(int texture_id);
DECLFUNC(texture_unlock);
#define texture_unlock(texture_id) FCALL(ftlbase, texture_unlock, texture_id)

typedef int (*FUNCTYPE(framebuffer_get_texture))(int framebuffer_id);
DECLFUNC(framebuffer_get_texture);
#define framebuffer_get_texture(framebuffer_id) \
    FCALL(ftlbase, framebuffer_get_texture, framebuffer_id)

typedef int (*FUNCTYPE(thread_init))(void);
DECLFUNC(thread_init);

typedef int (*FUNCTYPE(id_array_register))(IDArray* array, void* object);
DECLFUNC(id_array_register);

typedef UserDataParams* (*FUNCTYPE(new_params))(int operation);
DECLFUNC(new_params);

typedef void* (*FUNCTYPE(mem_alloc))(int64_t size, int align, int flags);
DECLFUNC(mem_alloc);

typedef void* (*FUNCTYPE(sys_texture_lock))(void* texture, int lock_mode, int x, int y, int w,
                                            int h);
DECLFUNC(sys_texture_lock);

typedef int (*FUNCTYPE(sys_graphics_read_pixels))(int x, int y, int w, int h, int stride,
                                                  void* buffer);
DECLFUNC(sys_graphics_read_pixels);
#define sys_graphics_read_pixels(x, y, w, h, stride, buffer) \
    FCALL(ftlbase, sys_graphics_read_pixels, x, y, w, h, stride, buffer);
