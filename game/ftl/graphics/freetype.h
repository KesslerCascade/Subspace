#pragma once

#include "ftl/ftl.h"
#include "ftl/struct.h"
#include "hook/function.h"

typedef void (*FUNCTYPE(freetype_easy_print))(int font, float x, float y, basic_string* text);
DECLFUNC(freetype_easy_print);
#define easy_print(font, x, y, cxxstr) FCALL(ftlbase, freetype_easy_print, font, x, y, cxxstr)
