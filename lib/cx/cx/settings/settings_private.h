#pragma once

#include "settings.h"
#include "settingstree.h"
#include "settingshashnode.h"

#include <cx/container.h>
#include <cx/time.h>
#include <cx/serialize/streambuf.h>
#include <cx/ssdtree/ssdtree.h>

#define SETTINGS_DEFAULT_FLUSH_INTERVAL (timeS(30))

void _setsThreadCheck(void);
void _setsThreadWatch(SSDNode *sets);
void _setsThreadForget(SSDNode *sets);
bool _setsWriteTree(SSDNode *root, SettingsTree *tree, SSDLockState *lstate);

_meta_inline bool _setsWriteBoundVar(SettingsBind *bind, stype styp, stgeneric val)
{
    // for strings, destroy destination first to prevent leaks
    if (stEq(styp, stType(string)))
        strDestroy(&bind->var->st_string);
    return _stConvert(bind->type, bind->var, styp, NULL, val, 0);
}

#define _setsBCacheSize(btype) (stHasFlag(btype, PassPtr) ? sizeof(void*) : stGetSize(btype))
_meta_inline void _setsUpdateBindCache(SettingsBind *bind)
{
    // binding cache is a snapshot of the bound variable, but NOT the data or anything that needs
    // a heavyweight copy. I.e. for strings it's just the pointer. This means we won't detect
    // if a string is changed in-memory without reallocating it, but avoids an extra string
    // copy/compare every time setsGet is called.

    memcpy(&bind->cache, bind->var, _setsBCacheSize(bind->type));
}
