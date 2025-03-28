#pragma once

#include "cx/fs/file.h"
#include "cx/fs/fs.h"
#include "cx/fs/path.h"
#include "cx/string.h"
#include "cx/thread/rwlock.h"
#include "cx/utils.h"

extern RWLock _fsCurDirLock;
extern string _fsCurDir;
extern string fsPathSepStr;
extern string fsNSSepStr;
extern string fsPlatformPathSepStr;
