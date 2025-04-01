#pragma once

#include "ftl/ftl.h"
#include "hook/function.h"

typedef struct TextLibrary TextLibrary;

DECLSYM(opt_framelimit);

DECLSYM(Globals_Library);
#define g_Library ((TextLibrary*)symAddr(ftlbase, Globals_Library))
DECLSYM(Globals_Library_currentLanguage);
#define g_Library_currentLanguage ((basic_string*)symAddr(ftlbase, Globals_Library_currentLanguage))

DECLSYM(TargetFrameTimeMS);
double g_TargetFrameTimeMS;   // relocated out of read-only memory
