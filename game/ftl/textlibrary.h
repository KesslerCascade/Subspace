#pragma once

#include "ftl/ftl.h"
#include "ftl/globals.h"
#include "ftl/struct.h"
#include "hook/function.h"

typedef struct TextLibrary TextLibrary;

basic_string* subspace_TextLibrary_GetText(TextLibrary* self, basic_string* text, basic_string* out,
                                           basic_string* lang);

// FTL functions & hooks below

typedef basic_string* (*FUNCTYPE(TextLibrary_GetText))(TextLibrary* self, basic_string* text,
                                                       basic_string* out, basic_string* lang);

DECLSYM(TextLibrary_GetText_2arg);
DECLINFO(TextLibrary_GetText_2arg);
DECLSYM(TextLibrary_GetText_3arg);
DECLINFO(TextLibrary_GetText_3arg);
DECLFUNCP(TextLibrary_GetText);
#define TextLibrary_GetText(self, text, out, lang) \
    FCALLP(ftlbase, TextLibrary_GetText, self, text, out, lang)
#define GetText(text, out) \
    TextLibrary_GetText(Globals_Library, text, out, Globals_Library_currentLanguage)

#define Globals_Library ((TextLibrary*)symAddr(ftlbase, Globals_Library))
#define Globals_Library_currentLanguage \
    ((TextLibrary*)symAddr(ftlbase, Globals_Library_currentLanguage))
