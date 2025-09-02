#include "ftl/ftl.h"
#include "ftl/struct.h"
#include "hook/function.h"

DECLSYM(CApp_useDirect3D_offset);

#define CApp_useDirect3D(capp) MEMBER(ftlbase, CApp, capp, bool, useDirect3D)
