#pragma once

#include <cx/fs/fs_private.h>

// NT native path for windows API, returns scratch buffer!
_Ret_z_ wchar_t* fsPathToNT(_In_opt_ strref path);
