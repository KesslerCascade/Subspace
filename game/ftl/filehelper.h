#pragma once
#include "ftl/ftl.h"
#include "ftl/struct.h"
#include "hook/function.h"

basic_string* subspace_FileHelper_getUserFolder(basic_string* out);

// FTL functions & wrappers below

typedef basic_string* (*FUNCTYPE(FileHelper_getUserFolder))(basic_string* out);
DECLFUNC(FileHelper_getUserFolder);
#define FileHelper_getUserFolder(out) FCALL(ftlbase, FileHelper_getUserFolder, out)


typedef basic_string* (*FUNCTYPE(FileHelper_getSaveFile))(basic_string* out);
DECLFUNC(FileHelper_getSaveFile);
#define FileHelper_getSaveFile(out) FCALL(ftlbase, FileHelper_getSaveFile, out)
