#pragma once

// Generic callback handles

#include <cx/cx.h>

typedef void(*GenericCallback)();

int _callbackGetHandle(_In_z_ const char *cbtype, _In_ GenericCallback func);
_Ret_opt_valid_
GenericCallback _callbackGetFunc(_In_z_ const char *cbtype, int handle);

#define callbackGetHandle(cbtype, func) _callbackGetHandle(#cbtype, (GenericCallback)((cbtype)func))
#define callbackGetFunc(cbtype, handle) ((cbtype)_callbackGetFunc(#cbtype, handle))
