#pragma once

#include <cx/serialize/streambuf.h>

typedef enum JSON_PARSE_CONTEXT {
    JSON_Top,
    JSON_Object,
    JSON_Array,
} JsonContextType;

typedef enum JSON_PARSE_EVENT {
    JSON_Parse_Unknown,             // never actually sent, this would result in a parse error
    JSON_Object_Begin,              // a new object starts at the current context
    JSON_Object_Key,                // a valid json string was parsed in an object key context
    JSON_Object_End,                // end of the current object
    JSON_Array_Begin,               // a new array starts at the current context
    JSON_Array_End,                 // end of the current array
    JSON_String,                    // a string value was parsed
    JSON_Int,                       // a number was parsed that can be represented by an integer
    JSON_Float,                     // a number was parsed that cannot be represented by an integer
    JSON_True,                      // a true value was parsed
    JSON_False,                     // a false value was parsed
    JSON_Null,                      // a null value was parsed
    JSON_Error,                     // a parse error occurred; strData contains an error message.
                                    // JSON_End will be the next event and the parser will return false.
    JSON_End                        // parsing finished, any user-passed context should be cleaned
                                    // up if desired
} JsonEventType;

typedef struct JSONParseContext JSONParseContext;
typedef struct JSONParseContext {
    JSONParseContext *parent;
    JsonContextType ctype;          // type of context

    union {
        string curKey;              // current key being parsed if this is an object
        int32 curIdx;               // current index being parsed if this is an array
    } cdata;
} JSONParseContext;

typedef struct JSONParseEvent {
    JSONParseContext *ctx;

    JsonEventType etype;            // type of event
    union {
        int64 intData;
        double floatData;
        string strData;
    } edata;
} JSONParseEvent;
