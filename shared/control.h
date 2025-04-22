#pragma once

#ifndef SUBSPACE_GAME
#include <cx/string.h>
#include <cx/serialize/streambuf.h>
#else
#include <sbuflite.h>
#endif

#include "net.h"
#include <stdint.h>
#include <stdbool.h>

#define MAX_CONTROL_MSG (16 * 1024 * 1024)
#define MAX_CONTROL_FIELD (4 * 1024 * 1024)
#define MAX_CONTROL_ARRAY (1024 * 1024)

// Control protocol:
// Sync bytes (0x0e 0x19 0x01 0x17)
// 4 bytes total message size
// 4 bytes unique Message ID
// 4 bytes In-Reply-To Message ID (or 0 if not in reply to anything)
// 16 bytes command name (null-terminated string)
// 4 bytes number of fields in payload
// payload: repeating
//    32-bit field size
//    16 bytes field name (null terminated string)
//    16-bit type ID
//        1 = int (32 bit)
//        2 = 32-bit float
//        3 = 64-bit float
//        4 = string
//        65536 = raw data
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
    char cmd[16];
    uint32_t nfields;
} ControlMsgHeader;

enum CFTYPE_ENUM {
    CF_INT,
    CF_FLOAT32,
    CF_FLOAT64,
    CF_STRING,
    CF_RAW      // incompatible with CF_ARRAY
};

enum CFFLAG_ENUM {
    CF_UNSIGNED = 0x0001,
    CF_ARRAY    = 0x0100
};

typedef struct ControlFieldHeader {
    uint32_t size;
    char name[16];
    uint16_t ftype;
    uint16_t flags;
} ControlFieldHeader;

typedef struct ControlField {
    ControlFieldHeader h;
    uint32_t count;   // for arrays, also raw field size
    union {
        int cfd_int;
        int* cfd_int_arr;
        unsigned int cfd_uint;
        unsigned int* cfd_uint_arr;
        float cfd_float32;
        float* cfd_float32_arr;
        double cfd_float64;
        double* cfd_float64_arr;
#ifdef SUBSPACE_GAME
        char* cfd_str;
        char** cfd_str_arr;
#else
        string cfd_str;
        sa_string cfd_str_arr;
#endif
        void* cfd_raw;
    } d;
} ControlField;

typedef struct ControlMsg ControlMsg;

enum CFALLOCMODE_ENUM {
    CF_ALLOC_AUTO = 0, // controlRecv allocates using standard system malloc (or game malloc)
    CF_ALLOC_NEVER,    // never allocate; just fail to receive strings and raw data
#ifdef SUBSPACE_GAME
    CF_ALLOC_SALLOC,   // controlRecv allocates using smalloc (slow), must be free using sfree
#endif
    CF_ALLOC_PRE,      // caller preallocates space or provides a static buffer and sets size accordingly
};

// for allocating and receiving an entire message at once
typedef struct ControlMsg {
    ControlMsgHeader hdr;
    ControlField** fields;
} ControlMsg;

typedef struct ControlState {
    StreamBuffer *sendbuf;
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

bool controlSend(ControlState *cs);    // send pending bytes in the buffer
bool controlMsgReady(ControlState* cs); // true if a message can be received from the buffer

// add a message to the output buffer and try to send it
bool controlPutMsg(ControlState* cs, ControlMsgHeader *hdr, ControlField** fields);

// decode message parts from a FULLY received message in the buffer (see controlMsgReady)
bool controlGetHeader(ControlState* cs, ControlMsgHeader* hdr);
bool controlGetField(ControlState* cs, ControlField* field, int allocmode);
ControlMsg *controlGetMsg(ControlState* cs, int allocmode);  // must be CF_ALLOC_AUTO or CF_ALLOC_SALLOC
void controlRecvDone(ControlState* cs);  // finish receiving this message and move on to the next one

void controlFieldFree(ControlField* field, int allocmode);
void controlFieldFreeMulti(uint32_t nfields, ControlField** fields, int allocmode, bool freearr);
void controlMsgFree(ControlMsg* msg, int allocmode);
ControlMsg* controlAllocMsg(int nfields, int allocmode);

void controlStateDestroy(ControlState* cs);
