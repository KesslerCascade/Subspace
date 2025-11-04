#pragma once

#include <cx/container.h>
#include <cx/serialize/streambuf.h>
#include <cx/string.h>

#include <stdbool.h>
#include <stdint.h>
#include "net.h"

#define MAX_CONTROL_MSG   (16 * 1024 * 1024)
#define MAX_CONTROL_FIELD (4 * 1024 * 1024)
#define MAX_CONTROL_ARRAY (1024 * 1024)

// Control protocol:
// Sync bytes (0x0e 0x19 0x01 0x17)
// 4 bytes total message size
// 4 bytes unique Message ID
// 4 bytes In-Reply-To Message ID (or 0 if not in reply to anything)
// 8 bytes timestamp
// 16 bytes command name (null-terminated string)
// 4 bytes number of fields in payload
// payload: repeating
//    32-bit field size
//    16 bytes field name (null terminated string)
//    16-bit type ID
//        0 = int (32 bit)
//        1 = int64 (64 bit)
//        2 = 32-bit float
//        3 = 64-bit float
//        4 = boolean (1 byte)
//        5 = string
//        6 = raw binary data
//    16-bit flags
//        0x0001 = unsigned integer
//        0x0100 = Array (32-bit array count follows)
//    Strings are represented by a 16-bit length field, then a string WITHOUT null terminator.
//    Each field is padded to align to a 4-byte boundary

typedef struct ControlMsgHeader {
    uint8_t sync[4];
    uint32_t size;
    uint32_t msgid;
    uint32_t replyid;
    int64_t timestamp;
    char cmd[16];
    uint32_t nfields;
} ControlMsgHeader;

enum CFTYPE_ENUM {
    CF_INT,
    CF_INT64,
    CF_FLOAT32,
    CF_FLOAT64,
    CF_BOOL,
    CF_STRING,
    CF_RAW   // incompatible with CF_ARRAY
};

enum CFFLAG_ENUM { CF_UNSIGNED = 0x0001, CF_ARRAY = 0x0100 };

typedef struct ControlFieldHeader {
    uint32_t size;
    char name[16];
    uint16_t ftype;
    uint16_t flags;
} ControlFieldHeader;

typedef struct ControlField {
    ControlFieldHeader h;
    uint32_t count;   // for arrays, also raw field size
    stvar d;          // field data
} ControlField;

typedef struct ControlMsg ControlMsg;

// for allocating and receiving an entire message at once
typedef struct ControlMsg {
    ControlMsgHeader hdr;
    hashtable fields;
#ifdef SUBSPACE_GAME
    int priority;   // if multiple messages are sent during the same tick, lower priorities are sent
                    // first
#endif
} ControlMsg;

typedef struct ControlState {
    StreamBuffer* sendbuf;
    StreamBuffer* recvbuf;
    socket_t sock;
    uint32_t nextid;
    uint8_t* tmprecv;
    size_t lastsent;
    bool init;
    bool closed;        // socket was closed

    bool recvhdr;       // have received a header yet
    uint32_t nfield;    // next field to decode
    uint32_t nfields;   // how many fields need to be processed
    uint32_t left;      // how many bytes left in current message being processed
} ControlState;

void controlInit(ControlState* cs, socket_t sock);

bool controlSendBuffer(ControlState* cs);   // send pending bytes in the buffer
bool controlMsgReady(ControlState* cs);     // true if a message can be received from the buffer

// add a message to the output buffer
// for internal use only
bool _controlEncodeMsg(ControlState* cs, ControlMsgHeader* hdr, hashtable fields);

// decode message parts from a FULLY received message in the buffer (see controlMsgReady)
// for internal use only
bool _controlParseHeader(ControlState* cs, ControlMsgHeader* hdr);
bool _controlParseField(ControlState* cs, ControlField* field);
// finish parsing this message and move on to the next one
// for internal use only
void _controlParseDone(ControlState* cs);

// higher level function to parse and return a full message from the receive buffer
ControlMsg* controlRecvMsg(ControlState* cs);

// add a message to the output buffer and try to send it
bool controlSendMsg(ControlState* cs, ControlMsg* msg);

ControlMsg* controlMsgCreate(strref cmd);
void controlMsgDestroy(ControlMsg* msg);

void controlStateDestroy(ControlState* cs);

// field conversions

#define DECL_CFIELDVAL(type)  bool _cfieldVal_##type(hashtable fields, strref name, type* out)
#define DECL_CFIELDVALD(type) type _cfieldValD_##type(hashtable fields, strref name, type def)
DECL_CFIELDVAL(bool);
DECL_CFIELDVALD(bool);
DECL_CFIELDVAL(int32);
DECL_CFIELDVALD(int32);
DECL_CFIELDVAL(uint32);
DECL_CFIELDVALD(uint32);
DECL_CFIELDVAL(int64);
DECL_CFIELDVALD(int64);
DECL_CFIELDVAL(uint64);
DECL_CFIELDVALD(uint64);
DECL_CFIELDVAL(float32);
DECL_CFIELDVALD(float32);
DECL_CFIELDVAL(float64);
DECL_CFIELDVALD(float64);

#define cfieldVal(type, fields, name, out)  _cfieldVal_##type(fields, name, out)
#define cfieldValD(type, fields, name, def) _cfieldValD_##type(fields, name, def)
strref cfieldString(hashtable fields, strref name);

#define cfieldSet(msg, name, type, val) \
    htInsert(&msg->fields, strref, name, stvar, stvar(type, val))
