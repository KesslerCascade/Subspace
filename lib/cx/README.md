# C, eXtended
## What is cx?
CX is a collection of utilities and a framework for cross-platform development in the C
language. Its focus is on programmer convenience, compact code, and making traditionally
high-level constructs available with a minimum of effort. The design philosophy is to
make these features available in a compact, unobtrusive way.

## Key features of cx:
* Growing library of cross-platform interfaces to system dependent features such as timers,
  threading, locks, and atomics (even for compilers like MSVC that don't support C11 atomics).
* Lightweight runtime type system for efficient generic programming that doesn't create
  template explosions or code bloat.
* Memory-efficient string library with copy-on-write semantics and automatic transition to
  ropes for very long strings.
* Flexible and convenient container system (currently dynamic arrays and hashtables) that
  supports automatic cleanup without having to manually write code for every container.
* Optional object-oriented programming system that supports classes, runtime polymorphism,
  interfaces, and mixins. The OO framework is designed to minimize the amount of boilerplate
  code involved in creating new classes.
* Optional try/catch/finally exception handling with stack unwinding.
* Designed to be self contained and statically linked, so it does not add extra dependencies
  to your project or require a complex build environment.
  
## Currently supported platforms and compilers
* FreeBSD + Clang *or* GCC
* Windows + MSVC 2015 or later
* Linux + GCC *or* Clang
* WebAssembly + Emscripten

## Currently supported hardware architectures

* x86-64
* x86 (32 bit)
* ARM64
* WebAssembly

## Why not just use C++, C# or one of 1,000 interpreted languages?
Those are valid choices for many projects. cx is intended for situations where you either
must use C (i.e. to interface with a large existing codebase), have a need for extreme
performance without incurring the bloat, tricky semantics, and maintenance headaches of
C++, do not want to depend on a bulky runtime, or simply prefer writing in straight C.
