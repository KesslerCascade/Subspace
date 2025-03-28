#pragma once

#include "jsoncommon.h"
#include <cx/ssdtree/ssdshared.h>

typedef struct JSONOut JSONOut;

#define JSON_Indent(x) (x & JSON_Indent_Mask)
enum JSON_OUT_FLAGS {
  //JSON_Indent       = 0x00000000,             // Indent a certain number of spaces for each level in the hierarchy
    JSON_Indent_Mask  = 0x0000001f,
    JSON_Compact      = 0x00000100,             // Omit spaces around { }, [ ], and :
    JSON_Single_Line  = 0x00000200,             // Do not insert linefeed characters, this causes indent to be ignored
    JSON_ASCII_Only   = 0x00000400,             // escape all non-ASCII characters in the output

    JSON_Unix_EOL     = 0x00010000,             // Force UNIX-style EOL regardless of operating system
    JSON_Windows_EOL  = 0x00020000,             // Force Windows-style EOL regardless of operating system

    // presets
    JSON_Pretty       = 0x00000004,             // 4-space indent
    JSON_Minimal      = 0x00000300,             // Compact | Single Line
};

// JSON OUTPUT
// These functions write to a streambuffer in PUSH mode.
// They should be repeatedly called with parse events in order to build the output.
_Ret_opt_valid_ JSONOut *jsonOutBegin(_Inout_ StreamBuffer *sb, flags_t flags);
bool jsonOut(_Inout_ JSONOut *jo, _In_ JSONParseEvent *ev);
_At_(*jo, _Pre_valid_ _Post_invalid_)
void jsonOutEnd(_Inout_ JSONOut **jo);

// JSON TREE OUTPUT

bool _jsonOutTree(_Inout_ StreamBuffer *sb, _In_ SSDNode *tree, flags_t flags, _Inout_opt_ SSDLockState *_ssdCurrentLockState);
// bool jsonOutTree(StreamBuffer *sb, SSDNode *tree, flags_t flags)
// Serialize an SSD tree to a streambuffer as JSON
#define jsonOutTree(sb, tree, flags) _jsonOutTree(sb, tree, flags, (SSDLockState*)_ssdCurrentLockState)

bool _jsonTreeToString(_Inout_ string *out, _In_ SSDNode *tree, flags_t flags, _Inout_opt_ SSDLockState *_ssdCurrentLockState);
// Serialize an SSD tree to a string
// bool jsonTreeToString(string *out, SSDNode *tree, flags_t flags)
#define jsonTreeToString(out, tree, flags) _jsonTreeToString(out, tree, flags, (SSDLockState*)_ssdCurrentLockState)

