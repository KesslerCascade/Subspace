#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include <cx/format.h>


typedef struct Formattable {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    bool (*format)(_In_ void* self, FMTVar* v, string* out);
} Formattable;
extern Formattable Formattable_tmpl;

