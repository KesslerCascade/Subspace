#pragma once

#include <cx/utils/macros.h>

// collection of macros to iterate through various containers and containerlike objects

// foreach takes a type specifier (mostly stype containers, though there are a few extra),
// a name for an iterator variable, and a set of initializaiton parameters that vary depending
// on the type.
#define foreach(type, itervar, ...) foreach_##type(type, itervar, __VA_ARGS__)

#define foreach_generic(type, itervar, ...) for (register char _foreach_outer = 1; _foreach_outer; _foreach_outer = 0) \
        for(ForEachIterType_##type itervar = ForEachIterInit_##type; _foreach_outer; _foreach_outer = 0) \
        for(; _foreach_outer; ForEachFinish_##type(itervar), _foreach_outer = 0) \
        for(ForEachInit_##type(itervar, __VA_ARGS__); ForEachValid_##type(itervar); ForEachNext_##type(itervar))

#define foreach_sarray(...) _foreach_array_msvc_workaround((__VA_ARGS__))
#define _foreach_array_msvc_workaround(args) _foreach_sarray args
#define _foreach_sarray(type, itervar, elemtype, elemvar, arrref) if ((arrref)._is_sarray) \
        for (register char _foreach_outer = 1; _foreach_outer; _foreach_outer = 0) \
        for (int32 itervar = 0, _##itervar##_max = saSize(arrref); _foreach_outer; _foreach_outer = 0) \
        for (elemtype elemvar = (elemtype){0}; _foreach_outer; _foreach_outer = 0) \
        for (itervar = 0; itervar < _##itervar##_max && (elemvar = (arrref).a[itervar], 1); ++itervar)

#define foreach_hashtable foreach_generic
#define ForEachIterType_hashtable htiter
#define ForEachIterInit_hashtable {0}
#define ForEachInit_hashtable(itervar, ht) htiInit(&itervar, ht)
#define ForEachValid_hashtable(itervar) htiValid(&itervar)
#define ForEachNext_hashtable(itervar) htiNext(&itervar)
#define ForEachFinish_hashtable(itervar) htiFinish(&itervar)

#define foreach_string foreach_generic
#define ForEachIterType_string striter
#define ForEachIterInit_string {0}
#define ForEachInit_string(itervar, str) striInit(&itervar, str)
#define ForEachValid_string(itervar) striValid(&itervar)
#define ForEachNext_string(itervar) striNext(&itervar)
#define ForEachFinish_string(itervar) striFinish(&itervar)

#define foreach_vfssearch foreach_generic
#define ForEachIterType_vfssearch FSSearchIter
#define ForEachIterInit_vfssearch {0}
#define ForEachInit_vfssearch(itervar, ...) vfsSearchInit(&itervar, __VA_ARGS__)
#define ForEachValid_vfssearch(itervar) vfsSearchValid(&itervar)
#define ForEachNext_vfssearch(itervar) vfsSearchNext(&itervar)
#define ForEachFinish_vfssearch(itervar) vfsSearchFinish(&itervar)

#define foreach_object(...) _foreach_object_msvc_workaround((__VA_ARGS__))
#define _foreach_object_msvc_workaround(args) _foreach_object args
#define _foreach_object(type, itervar, ivartype, obj) for (register char _foreach_outer = 1; _foreach_outer; _foreach_outer = 0) \
        for(ivartype *itervar = (obj) ? (obj)->_->iter(obj) : NULL; _foreach_outer; objRelease(&itervar), _foreach_outer = 0) \
        for(; itervar && itervar->_->valid(itervar); itervar->_->next(itervar))

#define foreach_ssd(...) _foreach_ssd_msvc_workaround((__VA_ARGS__))
#define _foreach_ssd_msvc_workaround(args) _foreach_ssd args
#define _foreach_ssd(type, itervar, idxvar, keyvar, valvar, root) for (register char _foreach_outer = 1; _foreach_outer; _foreach_outer = 0) \
        ssdLockedTransaction(root) \
        for(SSDIterator *itervar = (root) ? (root)->_->_iterLocked(root, _ssdCurrentLockState) : NULL; _foreach_outer; objRelease(&itervar), _foreach_outer = 0) \
        for(int32 idxvar = 0; _foreach_outer; _foreach_outer = 0) \
        for(strref keyvar = 0; _foreach_outer; _foreach_outer = 0) \
        for(stvar *valvar = 0; itervar && itervar->_->iterOut(itervar, &idxvar, &keyvar, &valvar); itervar->_->next(itervar))
