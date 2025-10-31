# CX Framework - AI Coding Agent Instructions

## Project Overview
CX is a C utility framework for cross-platform development focusing on programmer convenience and high-level constructs in pure C. It provides a runtime type system, object-oriented programming, copy-on-write strings, generic containers, and platform abstraction layers.

**Core Design Principles:**
- Minimize boilerplate through code generation and macros
- Static linking with no external runtime dependencies
- Memory efficiency through reference counting and lazy allocation
- Type-safe generic programming without template bloat

## Architecture & Key Components

### 1. Code Generation System (`cxautogen`)
**Critical**: CX uses a custom IDL (Interface Definition Language) for object-oriented code.

**`.cxh` files** define interfaces and classes:
```c
interface TestIf1 {
    int testfunc();
}

class TestCls1 implements TestIf1 {
    int data;
    factory create();
}
```

**Build Integration**: `add_cxautogen()` in CMakeLists.txt generates `.h`, `.c`, and `.auto.inc` files:
```cmake
add_cxautogen(objtestobj.cxh)
```

Generated files include:
- `objtestobj.h` - Public API
- `objtestobj.c` - Implementation stubs to fill in
- `objtestobj.auto.inc` - Generated boilerplate

**Never edit generated `.h` files directly** - modify the `.cxh` source instead.

### 2. String Library (`cx/string/`)
Copy-on-write strings with automatic rope conversion for long strings.

**Always initialize strings to NULL:**
```c
string s = 0;  // REQUIRED - never use uninitialized strings
```

**Core pattern - strings are opaque handles:**
```c
string str = 0;
strDup(&str, _S"hello");      // Copy/assign
strAppend(&str, _S" world");  // Modify in place (COW semantics)
strDestroy(&str);             // Cleanup
```

**String literals use `_S` prefix**: `_S"literal"` creates a static string.

### 3. Generic Containers

#### SArray (Dynamic Arrays)
Type-safe dynamic arrays declared per-type:

```c
sa_int32 arr = { 0 };          // Initialize to zero
saInit(&arr, int32, 16);       // Create with initial capacity
saPush(&arr, int32, 42);       // Type-checked push
int32 val = arr.a[0];          // Direct array access
saDestroy(&arr);               // Cleanup
```

**Sorted arrays**: Use `SA_Sorted` flag for O(log n) search with `saFind()`.

#### Hashtable
Generic key-value maps:

```c
hashtable ht = 0;
htInit(&ht, string, int32, 16);
htInsert(&ht, string, _S"key", int32, 42);
int32 val;

if (htFind(ht, string, _S"key", int32, &val) {
    // found, use val
}
// -- OR --
htelem elem = htFind(ht, string, _S"key", none, NULL);
if (elem) {
    val = hteVal(ht, int32, elem);  // can also use hteValPtr to obtain pointer to stored value
}

htDestroy(&ht);
```

**Key pattern**: Functions take type parameters for compile-time checking: `htInsert(&ht, keytype, key, valtype, value)`.

### 4. Object System (`cx/obj/`)
Lightweight OOP with interfaces, inheritance, and mixins.

**Core macros:**
- `objInstIf(obj, Interface)` - Get interface pointer
- `objRelease(&obj)` - Decrement refcount and destroy if zero
- `TestCls(ptr)` - Static cast to class type
- `objDynCast(ptr, TargetClass)` - Runtime type checking, returns NULL if not compatible

**Factory pattern**: Classes typically use `_create()` functions (generated from `factory create();` in `.cxh`).

### 5. Runtime Type System (`cx/stype/`)
`stype` is a 32-bit runtime type descriptor enabling generic programming.

**Common patterns:**
- `stCheckedArg(type, value)` - Type-safe argument passing; implicitly added by macro wrappers
- `stGetSize(stype)` - Get size of type at runtime
- `stVar(type, value)` - Create variant container, stored in the stvar type

### 6. Platform Abstraction (`cx/platform/`, `cx/thread/`)
Cross-platform APIs for:
- Threading: `thrCreate()`, `Mutex`, `RWLock`, `Event`
- Atomics: `atomicStore()`, `atomicFetchAdd()`, etc. (works on MSVC without C11)
- Timers: High-resolution timing APIs
- File system: Path manipulation, VFS

**Windows/Unix/WASM support** through conditional compilation.

## Build System

### CMake Configuration
**Custom build types:**
- `Debug` - Full debug, assertions enabled
- `Dev` - Optimized with debug info (default for development)
- `DevNoOpt` - Debug without optimization
- `Release` - Full optimization

**CMake Presets**: Use `CMakePresets.json` for preconfigured builds:
```bash
cmake --preset msvc-dev       # Windows MSVC
cmake --preset gcc-debug      # Unix GCC
cmake --build build/msvc-dev
```

**Out-of-source builds**: Always build in `build/` or `_win64/` directories.

### Testing
Tests use a custom runner: `test_runner <testname> <subtest>`

Example: `test_runner objtest iface` runs the interface test.

**Test structure** (`tests/common.h`):
- Each test file exports `TEST_FUNCS[]` array
- Sub-tests are individual functions returning 0 on success

## Critical Conventions

### Memory Management

**CRITICAL: Never use `malloc`/`free` directly - always use the xalloc system (`xaAlloc`/`xaFree`).**

The xalloc system wraps mimalloc or the system allocator and provides enhanced functionality including alignment, optional allocations, and out-of-memory handling.

#### xalloc Examples:

```c
// Basic allocation
MyStruct *data = xaAlloc(sizeof(MyStruct));

// Convenience macro for struct allocation
MyStruct *data = xaAllocStruct(MyStruct);

// Zero-filled allocation
uint8 *buffer = xaAlloc(1024, XA_Zero);

// Aligned allocation (align to 2^6 = 64 byte boundary)
void *aligned = xaAlloc(4096, XA_Align(6));

// Optional allocation that may fail (returns NULL on OOM)
void *data = xaAlloc(huge_size, XA_Opt);
if (!data) {
    // handle allocation failure
}

// Resize existing allocation
xaResize(&buffer, new_size);  // Takes pointer-to-pointer

// Free memory
xaFree(data);

// Release with NULL assignment (preferred pattern)
xaRelease(&data);  // Frees and sets data = NULL
```

**Optional allocation tiers** (use with `XA_Optional(tier)`):
- `XA_Optional(High)` - Try hard to allocate (default with `XA_Opt`)
- `XA_Optional(Low)` - Don't try particularly hard
- `XA_Optional(None)` - Fail immediately if no memory available

**Other memory rules:**
1. **Strings**: Always initialize to `0`, use `strDestroy()` to free
2. **Containers**: Always initialize to `{ 0 }` or `0`, use destructor macros
3. **Objects**: Use `objRelease()`, not manual `free()`
4. **Mimalloc**: Default allocator (can override with `CX_USE_SYSTEM_MALLOC`)

### Type Safety
- **Always pass type parameters**: `saPush(&arr, int32, val)` not just `saPush(&arr, val)`
- **Use `stVar()` for variants**: `htInsert(&ht, string, key, stVar(int32, 42))`
- **Check return values**: Many functions return indices (`-1` for not found) or `NULL`

### Naming Patterns
- **Interfaces**: `MyInterface` (PascalCase)
- **Classes**: `MyClass` (PascalCase)
- **Functions**: `myFunction` (camelCase)
- **Macros**: `MACRO_NAME` or `macroName` depending on usage
- **Internal functions**: `_myInternalFunc` (leading underscore), should be considered private

### Header Organization
- `cx/include/cx/*.h` - Public API headers (aggregate includes)
- Module subdirectories contain implementation headers
- **Never include implementation headers directly** - use public includes

## Common Tasks

### Adding a New Class
1. Create `myclass.cxh` with interface/class definitions
2. Add `add_cxautogen(myclass.cxh)` to CMakeLists.txt
3. Run cxautogen to generate files including C stubs
4. Implement methods in `myclass.c`
5. Include generated `myclass.h` in your code

### Adding Tests
1. Create `mytest.c` with test functions
2. Add to `create_test_sourcelist()` in `tests/CMakeLists.txt`
3. Add test cases with `add_test(NAME "category: Test" COMMAND test_runner mytest testname)`

### Cross-Platform Code
- Use `CX_PLATFORM_IS_WINDOWS`, `CX_PLATFORM_IS_UNIX`, `CX_PLATFORM_IS_WASM` defines
- Check compiler with `CX_COMPILER_IS_MSVC`, `CX_COMPILER_IS_CLANG`, `CX_COMPILER_IS_GNU`
- Use platform abstractions in `cx/platform/` rather than direct OS APIs

## Working with Dependencies
Third-party libraries in `3rdparty/`:
- **lua** - Lua scripting (optional, controlled by `CX_LUA` option)
- **mbedtls** - Cryptography
- **mimalloc** - Memory allocator
- **pcre2** - Regular expressions

These are statically linked and managed by the build system.
