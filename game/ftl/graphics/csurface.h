#pragma once

#include "ftl/ftl.h"
#include "ftl/struct.h"
#include "hook/function.h"

typedef void (*FUNCTYPE(CSurface_GL_SetColor))(GL_Color color);
DECLFUNC(CSurface_GL_SetColor);
#define CSurface_GL_SetColor(color) FCALL(ftlbase, CSurface_GL_SetColor, color)
