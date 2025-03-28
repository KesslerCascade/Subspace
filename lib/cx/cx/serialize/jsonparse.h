#pragma once

#include <cx/serialize/jsoncommon.h>

typedef struct SSDNode SSDNode;
typedef struct SSDTree SSDTree;
typedef void (*jsonParseCB)(_In_ JSONParseEvent *ev, _Inout_opt_ void *userdata);

// JSON STREAM PARSER
// Parses JSON data from a streambuffer, which must support PULL mode.
// Repeatedly calls the callback with the user-supplied data.
bool jsonParse(_Inout_ StreamBuffer *sb, _In_ jsonParseCB callback, _Inout_opt_ void *userdata);

// JSON TREE PARSER
// Parses JSON data from a streambuffer, which must support PULL mode.
// Fully loads the data into a semi-structured data tree.
_Ret_opt_valid_ SSDNode *jsonParseTree(_Inout_ StreamBuffer *sb);
_Ret_opt_valid_ SSDNode *jsonParseTreeCustom(_Inout_ StreamBuffer *sb, _In_opt_ SSDTree *tree);

// Parses JSON data from a string.
_Ret_opt_valid_ SSDNode *jsonTreeFromString(_In_opt_ strref str);
