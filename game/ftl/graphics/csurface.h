#pragma once

#include "ftl/ftl.h"
#include "ftl/struct.h"
#include "hook/function.h"

extern DisasmTrace CSurface_GL_BlitFrameBuffer_trace;

typedef void (*FUNCTYPE(CSurface_GL_SetColor))(GL_Color color);
DECLFUNC(CSurface_GL_SetColor);
#define CSurface_GL_SetColor(color) FCALL(ftlbase, CSurface_GL_SetColor, color)

typedef void (*FUNCTYPE(CSurface_GL_BlitFrameBuffer))(int* fb, float size_x, float size_y, bool aa);
DECLFUNC(CSurface_GL_BlitFrameBuffer);

typedef void (*FUNCTYPE(CSurface_FinishFrame))(void);
DECLFUNC(CSurface_FinishFrame);