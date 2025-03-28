#pragma once

#include "stype.h"

// conversion handlers for built-in types

// can be used by other handlers to chain to if they can convert to a generic type like int64 first

bool stConvert_none(stype destst, stgeneric *dest, stype srcst, stgeneric src, uint32 flags);
bool stConvert_bool(stype destst, stgeneric *dest, stype srcst, stgeneric src, uint32 flags);
bool stConvert_int(stype destst, stgeneric *dest, stype srcst, stgeneric src, uint32 flags);
bool stConvert_float32(stype destst, stgeneric *dest, stype srcst, stgeneric src, uint32 flags);
bool stConvert_float64(stype destst, stgeneric *dest, stype srcst, stgeneric src, uint32 flags);
bool stConvert_ptr(stype destst, stgeneric *dest, stype srcst, stgeneric src, uint32 flags);
