#pragma once

#include <cx/cx.h>

CX_C_BEGIN

typedef struct str_ref {
    void *_is_string;
} str_ref;

// IMPORTANT NOTE!
// Always initialize sstring to NULL or 0 first!
typedef struct str_ref* _Nullable string;

// String references are borrowed references to strings, for
// controlled situations such as passing as function arguments.
//
// IMPORTANT! It is *NEVER* safe to return a strref from a function
// in a multi-threaded program. Doing so may create a race condition.
// Always use an output argument and duplicate the result into it.
// (this also makes it a lot harder for the caller for forget they
// need to destroy it)
typedef const struct str_ref* _Nullable strref;

// String handle is a pointer to the string variable and must
// always be valid.
typedef string* _Nonnull strhandle;

// Prepare a string variable for use.
#define strInit(o) *(o) = NULL

// Create a new empty string with preallocated storage.
//        o: output string, will be destroyed if it exists
// sizehint: Amount of memory to preallocate for string storage.
void strReset(_Inout_ptr_opt_ strhandle o, uint32 sizehint);

// Duplicate an existing string, making an efficient reference if possible.
//       o: output string, will be destroyed if it exists
//       s: Handle of string to duplicate.
// Returns: String handle.
void strDup(_Inout_ strhandle o, _In_opt_ strref s);

// Like strDup but always makes a copy (does not use copy-on-write optimization).
//       o: output string, will be destroyed if it exists
//       s: Handle of string to copy.
void strCopy(_Inout_ strhandle o, _In_opt_ strref s);

// Clear the contents of a string. May leave existing storage as preallocated
// space if it is efficient to do so.
//      ps: pointer to string to clear
void strClear(_Inout_ strhandle ps);

// Retrieve the length of a string. This does not include preallocated memory.
//       s: String handle.
// Returns: String length.
_When_(s == NULL, _Post_equal_to_(0))
_Pure uint32 strLen(_In_opt_ strref s);

// Sets the length of a string, truncating or filling with null-characters.
//      ps: Pointer to string handle.
//     len: New length
void strSetLen(_Inout_ strhandle ps, uint32 len);

// Checks if a string is empty. Nonexistent strings are considered empty.
//       s: String handle
// Returns: true if string is empty
_When_(s == NULL, _Post_equal_to_(true))
_Pure bool strEmpty(_In_opt_ strref s);

// Disposes of a string handle.
//      ps: Pointer to string handle. Handle will be reset to NULL.
void strDestroy(_Inout_ strhandle ps);

// Obtains a read-only pointer to a classic C-style string.
// Will attempt to return a pointer to the string itself if possible, but
// in the case of ropes or other complex strings, the return value may be
// a scratch buffer instead, along with all the associated caveats and
// warnings (see utils/scratch.h). Care should be taken and the string
// used or copied as soon as possible if it must persist.
//      s: String reference
// Returns: C-style string
_Ret_valid_ const char *_Nonnull strC(_In_opt_ strref s);

// Obtains a read-write pointer to a string's backing memory buffer.
// This causes string memory to no longer be shared with duplicates.
// Pointer is valid until the next string function is called.
//      ps: Pointer to string handle.
//   minsz: If string is shorter than minsz, it will be zero-padded
//          up to this length. Useful for copying data into a string.
// Returns: Memory buffer.
_Ret_valid_ uint8 *_Nonnull strBuffer(_Inout_ strhandle ps, uint32 minsz);

// Copies up to bufsz bytes from the string to an external buffer.
// The resulting C-style string in buf will always be null terminated.
//       s: String handle.
//     off: Offset within string to begin copying.
//     buf: Pointer to memory buffer.
//   bufsz: Size of memory buffer.
// Returns: Number of bytes copied (may be smaller than requested if string length is exceeded).
uint32 strCopyOut(_In_opt_ strref s, uint32 off, _Out_writes_bytes_(bufsz) uint8 *_Nonnull buf, uint32 bufsz);

// Copies raw bytes out of a string without null terminating.
//       s: String handle.
//     off: Offset within string to begin copying.
//     buf: Pointer to memory buffer.
//  maxlen: Maximum number of bytes to copy.
// Returns: Number of bytes copied (may be smaller than requested if string length is exceeded).
uint32 strCopyRaw(_In_opt_ strref s, uint32 off, _Out_writes_bytes_(maxlen) uint8 *_Nonnull buf, uint32 maxlen);

_Pure uint32 _strStackAllocSize(uint32 maxlen);
void _strInitStack(_Inout_ _Deref_pre_valid_ _Deref_post_opt_valid_ strhandle ps, uint32 maxlen);
// Creates a stack-allocated temporary string. This string may be used as a buffer
// to hold results of string operations, but it must NOT be returned or stored in a
// way the survives the execution of the scope in which it is initialized.
//
// When used as a destination parameter, string functions do not promote strings
// to ropes if the destination is a stack-allocated string.
//
// String functions MAY replace the handle with one to a standard heap-allocated
// string if there is not enough buffer space for the result. Because of this,
// the caller MUST call strDestroy on the handle initialized by strTemp before
// leaving the scope in which it is defined.
#define strTemp(ps, maxlen) (*(ps)) = (string)stackAlloc(_strStackAllocSize(maxlen)); \
    _strInitStack(ps, maxlen);

#ifdef _WIN32
// definition of _S interferes with this header, so include it first
#include <wchar.h>
#include <mmintrin.h>
#endif

// Macros for creating string objects out of string literals by tacking on a basic header
// For pure ASCII strings
#define _S (string)"\xE0\xC1"
// For UTF-8 strings
#define _SU (string)"\xA0\xC1"
// For strings with other encodings or raw binary (no embedded NULLs)
#define _SO (string)"\x80\xC1"

CX_C_END
