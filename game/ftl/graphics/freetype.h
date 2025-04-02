#pragma once

#include "ftl/ftl.h"
#include "ftl/struct.h"
#include "hook/function.h"

typedef Pointf (*FUNCTYPE(freetype_easy_print))(int font, float x, float y, basic_string* text);
DECLFUNC(freetype_easy_print);
#define easy_print(font, x, y, cxxstr) FCALL(ftlbase, freetype_easy_print, font, x, y, cxxstr)

typedef Pointf (*FUNCTYPE(freetype_easy_printRightAlign))(int font, float x, float y,
                                                          basic_string* text);
DECLFUNC(freetype_easy_printRightAlign);
#define easy_printRightAlign(font, x, y, cxxstr) \
    FCALL(ftlbase, freetype_easy_printRightAlign, font, x, y, cxxstr)

typedef Pointf (*FUNCTYPE(freetype_easy_printCenter))(int font, float x, float y,
                                                      basic_string* text);
DECLFUNC(freetype_easy_printCenter);
#define easy_printCenter(font, x, y, cxxstr) \
    FCALL(ftlbase, freetype_easy_printCenter, font, x, y, cxxstr)

typedef Pointf (*FUNCTYPE(freetype_easy_measurePrintLines))(int font, float x, float y,
                                                            int line_length, basic_string* text);
DECLFUNC(freetype_easy_measurePrintLines);
#define easy_measurePrintLines(font, x, y, line_length, cxxstr) \
    FCALL(ftlbase, freetype_easy_measurePrintLines, font, x, y, line_length, cxxstr)
