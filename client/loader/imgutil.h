#pragma once

// Miscellaneous utilities for dealing with executable images in memory

typedef unsigned long addr_t;
extern addr_t ftlbase;

#define rva(b, v) ((addr_t)(b) + (addr_t)(v))

#ifndef CURBASE
#define CURBASE ftlbase
#endif
#define crva(v) rva(CURBASE, v)

#define addr(p)   ((addr_t)(p))
#define ptr(a)    ((void*)(a))
#define chptr(a)  ((char*)(a))
#define uchptr(a) ((unsigned char*)(a))
#define usptr(a)  ((unsigned short*)(a))
#define dwptr(a)  ((unsigned long*)(a))

#define prva(b, v)   ptr(rva(b, v))
#define chprva(b, v) chptr(rva(b, v))
#define dwprva(b, v) dwptr(rva(b, v))

#define var(t, n)      (*(t*)(_##n##_addr))
#define svar(t, n)     (*(t*)(_static_##n##_addr))
#define avar(t, n, d)  (*(t(*)[d])(_##n##_addr))
#define savar(t, n, d) (*(t(*)[d])(_static_##n##_addr))
#define iavar(t, n)    (*(t(*)[])(_##n##_addr))
#define siavar(t, n)   (*(t(*)[])(_static_##n##_addr))
