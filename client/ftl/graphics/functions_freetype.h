#include "ftl/ftl.h"
#include "ftl/struct.h"
#include "hook/function.h"

typedef void (*FUNCTYPE(freetype_easy_print))(int fontSize, float x, float y, basic_string* text);
DECLFUNC(freetype_easy_print);
#define easy_print(fontSize, x, y, cxxstr) \
    FCALL(ftlbase, freetype_easy_print, fontSize, x, y, cxxstr)
