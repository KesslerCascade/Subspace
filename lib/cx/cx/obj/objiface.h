#pragma once

#include <cx/cx.h>
#include <cx/container/sarray.h>

// An interface is similar to an abstract class in that it is a list of functions which
// fulfills a contract.

// This structure can be instantiated in different situations:
//       Template - The interface template is the publicly declared view of the interface.
//                  It contains only basic information about the parent interface and
//                  structure size. The method pointers will all be NULL. This template
//                  is the master copy of the interface, and its address is used to
//                  uniquely identify the interface.
//
//                  When the user queries a class for its interface, the system returns
//                  a copy of the template that is fully populated with method pointers.
//
// Implementation - The implementation form of the interface structure is a statically
//                  scoped instance. It contains a pointer to the interface template
//                  that is being implemented, the size, and pointers to the functions
//                  that implement the interface methods. This instance is used to
//                  register an implementation of the interface with a specific class.

typedef struct ObjIface ObjIface;
typedef struct ObjIface
{
    ObjIface *_implements;      // implementation only - interface template
    ObjIface *_parent;          // template only - parent interface
    size_t _size;               // size of the entire structure

    // interface method pointers beyond this point
} ObjIface;
saDeclarePtr(ObjIface);

// standard naming
#define objIfTmplName(iface) iface##_tmpl
#define objIfImport(iface) extern iface objIfTmplName(iface)

#define objIfCheck(iface) static_assert(((iface)->_implements, (iface)->_size, offsetof(*(iface), _parent) == offsetof(ObjIface, _parent)), "Not an interface")
#define objIfBase(iface) ((ObjIface*)((iface)->_implements, (iface)->_parent, (iface)->_size, (iface)))
