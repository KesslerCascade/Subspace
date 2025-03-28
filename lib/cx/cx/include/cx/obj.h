#pragma once

/* ----- CX obj ----- */

/*
 * Naming conventions:
 *
 * Capitalized names are preferred for higher-order objects like interfaces and classes,
 * but this is a style preference and not enforced.
 *
 * Interfaces:
 * [iface]          - Interface type name
 * [iface]_tmpl     - Interface template object (of type [iface])
 *                    Should be an empty interface with size and parent filled out
 *
 * Classes:
 * [class]          - Class instance type name - contains data members
 * [class]_clsinfo  - Class information for runtime use
 *
 * Interface implementations:
 * [class]_[iface]  - Object of type [iface] that contains function pointers to
 *                    interface methods this class implements
 */

#include <cx/obj/objiface.h>
#include <cx/obj/objclass.h>
#include <cx/obj/objimpl.h>
