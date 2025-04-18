#pragma once

// stripped down version of cx streambuffer that uses smalloc and doesn't depend on anything from the C runtime

#include <stdint.h>
#include <stdbool.h>

typedef struct StreamBuffer StreamBuffer;

// Pull callback
// sz is set to the maximum amount of data the may be written to buf.
// The callback should fill buf with up to that amount and return the number
// of bytes that were written.
// The callback may return 0 if no data is currently available but will likely
// be immediately called again, so a performing a blocking wait is advisable.
//
// If the callback needs to write an amount of data that is larger than the
// space available as indicated by sz, it may instead call sbufPWrite with the
// full amount (which will expand the buffer in the process) and return 0.
// 
// If sz is 0, check if the consumer is finished and/or for error state.
typedef size_t(*sbufPullCB)(StreamBuffer *sb, uint8_t *buf, size_t sz, void *ctx);

// Push callback
// When this callback is used (in direct mode), the data is pushed once to the
// callback and MUST all be written in one go or it will be lost.
// If sz is 0, check if the producer is finished and/or for error state.
typedef void(*sbufPushCB)(StreamBuffer *sb, uint8_t *buf, size_t sz, void *ctx);

// Send callback
// This callback is used with sbufCSend. It may be called multiple times with varying
// offsets. The offset passed is always from the start of the available bytes in the
// buffer.
// If this function returns true, the sent bytes will be consumed and removed from the
// buffer. If it returns false, it behaves like the peek functions and does not remove
// the bytes from the buffer.
// If sz is 0, check if the producer is finished and/or for error state.
typedef bool(*sbufSendCB)(StreamBuffer *sb, const uint8_t *buf, size_t off, size_t sz, void *ctx);

// Notify callback
// Notification to a consumer that data is available. The sbufC* functions may be
// used to read all or part of the available data.
// If sz is 0, check if the producer is finished and/or for error state.
typedef void(*sbufNotifyCB)(StreamBuffer *sb, size_t sz, void *ctx);

// Cleanup callback
// Called just before the structure is deallocated, should perform any needed
// cleanup of the user-supplied ctx.
typedef void(*sbufCleanupCB)(void *ctx);

// Push mode:   Producer repeatedly calls sbufPWrite.
//              Consumer is either notified that data is available via callback,
//              or if consumer is in direct mode, available data is pushed to its callback.
// Pull mode:   Consumer repeatedly calls sbufCRead or sbufCSend.
//              Producer's pull callback is called in order to fill the buffer.

enum STREAM_BUFFER_FLAGS_ENUM {
    SBUF_Push =                 0x0001,
    SBUF_Pull =                 0x0002,
    SBUF_Direct =               0x0010,
    SBUF_Error =                0x0800,
    SBUF_Producer_Registered =  0x1000,
    SBUF_Consumer_Registered =  0x2000,
    SBUF_Producer_Done =        0x4000,
    SBUF_Consumer_Done =        0x8000,
};

typedef struct StreamBuffer {
    uint8_t *buf;
    size_t bufsz;               // current buffer size
    size_t targetsz;            // desired max size (can be exceeded in push mode)

    uint8_t *overflow;            // for buffering writes when expanding the main buffer
    size_t overflowtail;        // where in the overflow buffer to start writing
    size_t overflowsz;          // size of overflow buffer, >0 serves as flag that we're swapping buffers

    size_t head;                // start of valid data in buffer
    size_t tail;                // end of valid data in buffer
    // Note that head == tail means an empty buffer, so the maximum amount of data is bufsz - 1.
    // This wastes 1 byte but is worth it to make the logic much simpler.

    sbufPullCB producerPull;
    sbufCleanupCB producerCleanup;
    void *producerCtx;

    sbufNotifyCB consumerNotify;
    sbufPushCB consumerPush;
    sbufCleanupCB consumerCleanup;
    void *consumerCtx;

    int refcount;
    uint32_t flags;
} StreamBuffer;

// Create a stream buffer
// targetsz may be set to 0 if direct mode is going to be used.
// Caller must free the returned pointer with sbufRelease when done.
StreamBuffer *sbufCreate(size_t targetsz);

// Release a reference to a streambuffer
void sbufRelease(StreamBuffer **sb);

// Puts a stream buffer into error state. Processing will be aborted and both consumers
// and producers should call the Finish function as soon as is practical.
void sbufError(StreamBuffer *sb);

static inline bool sbufIsPull(StreamBuffer *sb)
{
    return sb->flags & SBUF_Pull;
}

static inline bool sbufIsPush(StreamBuffer *sb)
{
    return sb->flags & SBUF_Push;
}

// Checks if the stream buffer is in an error state
static inline bool sbufIsError(StreamBuffer *sb)
{
    return sb->flags & SBUF_Error;
}

// Returns true if the producer is finished (EOF)
static inline bool sbufIsPFinished(StreamBuffer *sb)
{
    return (sb->flags & SBUF_Producer_Done) || sbufIsError(sb);
}

// Returns true if the consumer is finished (early out, data no longer need be produced)
static inline bool sbufIsCFinished(StreamBuffer *sb)
{
    return (sb->flags & SBUF_Consumer_Done) || sbufIsError(sb);
}

// ---------------------------------------------------------------------------- Producer Functions

// Registers a producer with the stream buffer in pull mode
bool sbufPRegisterPull(StreamBuffer *sb, sbufPullCB ppull, sbufCleanupCB pcleanup, void *ctx);
// Registers a producer with the stream buffer in push mode
bool sbufPRegisterPush(StreamBuffer *sb, sbufCleanupCB pcleanup, void *ctx);

// Returns the available space for writing to the buffer
size_t sbufPAvail(StreamBuffer *sb);
// Writes data to the buffer. This will always succeed unless the system is out of memory.
// Overflow buffer is used if more written data exceeds current buffer size.
bool sbufPWrite(StreamBuffer *sb, const uint8_t *buf, size_t sz);
// Writes a string to the buffer.
bool sbufPWriteCStr(StreamBuffer* sb, const char *str);
// Writes a string to the buffer along with a system-dependent EOL character.
bool sbufPWriteCLine(StreamBuffer* sb, const char *str);
// Writes a system-dependent EOL character to the buffer.
bool sbufPWriteEOL(StreamBuffer* sb);
// Mark the producer as finished. The producer MUST NOT touch the stream buffer after
// call as it maybe immediately deallocated.
void sbufPFinish(StreamBuffer *sb);

// ---------------------------------------------------------------------------- Consumer Functions

// Registers a consumer with the stream buffer in pull mode
bool sbufCRegisterPull(StreamBuffer *sb, sbufCleanupCB ccleanup, void *ctx);
// Registers a consumer with the stream buffer in push mode
bool sbufCRegisterPush(StreamBuffer *sb, sbufNotifyCB cnotify, sbufCleanupCB ccleanup, void *ctx);
// Registers a consumer with the stream buffer in direct push mode
bool sbufCRegisterPushDirect(StreamBuffer *sb, sbufPushCB cpush, sbufCleanupCB ccleanup, void *ctx);

// Returns how much data is currently buffered waiting to be consumed.
size_t sbufCAvail(StreamBuffer *sb);
// Reads sz bytes from the producer.
// If in pull mode, this will repeatedly call the producer's callback in order to completely
// satisfy the request. It will short read only when the producer is finished (EOF).
// If in push mode, this will fail if more data is requested than is available.
bool sbufCRead(StreamBuffer *sb, uint8_t *buf, size_t sz, size_t *bytesread);

// Peeks at data in the buffer without consuming it.
// In pull mode this will NOT call the callback and only read unconsumed data left in
// the buffer.
// Will never short read, will fail if there is not enough in the buffer (check sbufCAvail).
bool sbufCPeek(StreamBuffer *sb, uint8_t *buf, size_t off, size_t sz);

// For pull mode only, feeds the buffer until it has at least minsz bytes. This is similar
// to what sbufCRead does and will keep retrying until it either has enough or the producer
// finishes. This is useful for feeding the buffer without reading it in order to peek ahead.
// Returns true if the feed request was completely satisfied, or false if the producer finished
// before producing enough bytes.
bool sbufCFeed(StreamBuffer *sb, size_t minsz);

// Sends at most sz bytes from the buffer to a callback function.
// The callback may be called multiple times in order to send the requested data, it is not
// guaranteed that it will arrive in a single chunk.
// This can be used for more efficient operation in push mode. In the notify callback, call
// sbufCSend with the desired number of bytes. The difference is that sbufCRead will copy the
// data to another buffer, while sbufCSend will invoke the callback with a pointer to the
// streambuf's internal buffer, which avoids making another copy.
// If used in pull mode, it functions like sbufCRead and will try to fill the buffer with
// the requested amount of data before calling the callback.
// The callback should return true if it consumed the data, or false if it is only peeking.
bool sbufCSend(StreamBuffer *sb, sbufSendCB func, size_t sz);

// Skip over bytes in the buffer, can be used in conjunction with sbufCPeek.
bool sbufCSkip(StreamBuffer *sb, size_t bytes);

// Mark the consumer as finished. The consumer MUST NOT touch the stream buffer after
// call as it maybe immediately deallocated.
void sbufCFinish(StreamBuffer *sb);
