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

typedef void (*FUNCTYPE(FileHelper_closeBinaryFile))(int fd);
DECLFUNC(FileHelper_closeBinaryFile);

typedef void (*FUNCTYPE(FileHelper_deleteAllSaveFiles))();
DECLFUNC(FileHelper_deleteAllSaveFiles);
#define FileHelper_deleteAllSaveFiles() FCALL(ftlbase, FileHelper_deleteAllSaveFiles)

typedef bool (*FUNCTYPE(FileHelper_fileExists))(basic_string* filename);
DECLFUNC(FileHelper_fileExists);
#define FileHelper_fileExists(filename) FCALL(ftlbase, FileHelper_fileExists, filename)

typedef void (*FUNCTYPE(FileHelper_deleteFile))(basic_string* filename);
DECLFUNC(FileHelper_deleteFile);
#define FileHelper_deleteFile(filename) FCALL(ftlbase, FileHelper_deleteFile, filename)

typedef void (*FUNCTYPE(FileHelper_renameFile))(basic_string* src, basic_string* dest);
DECLFUNC(FileHelper_renameFile);
#define FileHelper_renameFile(src, dest) FCALL(ftlbase, FileHelper_renameFile, src, dest)