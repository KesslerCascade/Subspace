#pragma once

#include "cx/obj.h"
#include "cx/container.h"

void _objHydrateIface(_In_ ObjIface *ifimpl, _Inout_ sa_ObjIface *impls, _Inout_ hashtable *impltbl);
bool _objCheckIface(_In_ ObjIface *iface);
