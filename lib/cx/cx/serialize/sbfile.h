#pragma once

#include <cx/serialize/streambuf.h>
#include <cx/fs/vfs.h>

// Stock streambuffer producer for files

// Reads a file and pushes the its entire contents into a streambuffer.
// Will auto-chunk based on the streambuffer's target size.
// File will be closed when finished if close is set to true.
bool sbufFileIn(_Pre_valid_ _Post_invalid_ StreamBuffer *sb,
                _Pre_valid_ _When_(close, _Post_invalid_) VFSFile *file,
                bool close);

// -- or --
// 
// Registers as a producer with the streambuffer in pull mode.
// File will be closed when finished if close is set to true.
_Check_return_
bool sbufFilePRegisterPull(_Inout_ StreamBuffer *sb, _Inout_ VFSFile *file, bool close);

// ======================================================================

// Stock streambuffer consumer that outputs to a file

// Consumes all available data from the buffer and writes it to a file.
// File will be closed when finished if close is set to true.
bool sbufFileOut(_Pre_valid_ _Post_invalid_ StreamBuffer *sb,
                 _Pre_valid_ _When_(close, _Post_invalid_) VFSFile *file,
                 bool close);

// -- or -- 

// Registers as a consumer with the streambuffer in push mode.
// File will be closed when finished if close is set to true.
_Check_return_
bool sbufFileCRegisterPush(_Inout_ StreamBuffer *sb, _Inout_ VFSFile *file, bool close);
