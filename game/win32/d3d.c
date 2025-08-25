#include "osdep.h"
#include "ftl/capp.h"

bool osIsUsingDirect3D()
{
    return CApp_useDirect3D(theApp);
}