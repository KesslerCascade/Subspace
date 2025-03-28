#pragma once

#include <cx/fs/file.h>
#include <cx/serialize/streambuf.h>

// Stock streambuffer producer for low-level filesystem files

// Reads a file and pushes the its entire contents into a streambuffer.
// Will auto-chunk based on the streambuffer's target size.
// File will be closed when finished if close is set to true.
bool sbufFSFileIn(_Pre_valid_ _Post_invalid_ StreamBuffer* sb, _Pre_valid_ _When_(close, _Post_invalid_) FSFile* file,
                  bool close);

// -- or --
//
// Registers as a producer with the streambuffer in pull mode.
// File will be closed when finished if close is set to true.
_Check_return_ bool
sbufFSFilePRegisterPull(_Inout_ StreamBuffer* sb, _Inout_ FSFile* file, bool close);

// ======================================================================

// Stock streambuffer consumer that outputs to a file

// Consumes all available data from the buffer and writes it to a file.
// File will be closed when finished if close is set to true.
bool sbufFSFileOut(_Pre_valid_ _Post_invalid_ StreamBuffer* sb, _Pre_valid_ _When_(close, _Post_invalid_) FSFile* file,
                   bool close);

// -- or --

// Registers as a consumer with the streambuffer in push mode.
// File will be closed when finished if close is set to true.
_Check_return_ bool
sbufFSFileCRegisterPush(_Inout_ StreamBuffer* sb, _Inout_ FSFile* file, bool close);
