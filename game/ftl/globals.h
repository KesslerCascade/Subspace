#pragma once

#include "ftl/ftl.h"
#include "hook/function.h"

typedef struct TextLibrary TextLibrary;
typedef struct ResourceControl ResourceControl;

DECLSYM(opt_framelimit);
#define g_opt_framelimit (*(char*)symAddr(ftlbase, opt_framelimit))

DECLSYM(Globals_Library);
#define g_Library ((TextLibrary*)symAddr(ftlbase, Globals_Library))
DECLSYM(Globals_Library_currentLanguage);
#define g_Library_currentLanguage ((basic_string*)symAddr(ftlbase, Globals_Library_currentLanguage))

DECLSYM(TargetFrameTimeMS);
double g_TargetFrameTimeMS;   // relocated out of read-only memory

DECLSYM(ResourceControl_GlobalResources);
#define g_ResourceControl_GlobalResources \
    ((basic_sResourceControltring*)symAddr(ftlbase, ResourceControl_GlobalResources))
