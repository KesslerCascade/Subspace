#pragma once

#include <cx/serialize/streambuf.h>
#include <cx/string/strbase.h>

// Stock streambuffer producer for strings

// Pushes the entire contents of a string into a streambuffer.
// Will auto-chunk based on the streambuffer's target size.
bool sbufStrIn(_Pre_valid_ _Post_invalid_ StreamBuffer *sb, _In_opt_ strref str);

// -- or --
// 
// Registers as a producer with the streambuffer in pull mode.
_Check_return_
bool sbufStrPRegisterPull(_Inout_ StreamBuffer *sb, _In_opt_ strref str);

// ======================================================================

// Stock streambuffer consumer that outputs to a string

// Consumes all available data from the buffer and outputs to a string.
// This overwrites any existing string.
bool sbufStrOut(_Pre_valid_ _Post_invalid_ StreamBuffer *sb, _Inout_ string *strout);

// -- or -- 

// Registers as a consumer with the streambuffer in push mode.
// This consumer appends all output to the given string.
_Check_return_
bool sbufStrCRegisterPush(_Inout_ StreamBuffer *sb, _Inout_ string *strout);

// ======================================================================

// Shortcut for a common use case of creating a streambuffer that outputs
// to a string and the caller intends to be a producer in push mode.
_Check_return_ _Ret_opt_valid_ StreamBuffer*
sbufStrCreatePush(_Inout_ string* strout, size_t targetsz);
