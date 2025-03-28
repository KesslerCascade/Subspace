#include "stvar.h"
#include "cx/container/sarray.h"

void stvlInit(stvlist *list, int count, stvar *vars)
{
    list->count = count;
    list->vars = vars;
    list->cursor = 0;
}

// Initialize a list from an sarray of stvars
void _stvlInitSA(stvlist *list, stvar *vara)
{
    // slightly ugly here since we want stvar to be usable without including sarray.h
    sarrayref(stvar) vararray = { .a = vara };
    list->count = saSize(vararray);
    list->vars = vara;
    list->cursor = 0;
}

// Get the next variable of the specific type, if it exists
bool _stvlNext(stvlist *list, stype type, stgeneric *out)
{
    for (int i = list->cursor; i < list->count; i++) {
        if (stEq(type, list->vars[i].type)) {
            memcpy(stGenPtr(type, *out), stGenPtr(type, list->vars[i].data), stGetSize(type));
            list->cursor = i + 1;
            return true;
        }
    }
    return false;
}

void *_stvlNextPtr(stvlist *list, stype type)
{
    // make sure this is a type that stores a pointer in stvars
    if(!(stEq(type, stType(ptr)) ||
         stHasFlag(type, Object) ||
         stHasFlag(type, PassPtr)))
        return NULL;

    for(int i = list->cursor; i < list->count; i++) {
        if(stEq(type, list->vars[i].type)) {
            list->cursor = i + 1;
            return list->vars[i].data.st_ptr;
        }
    }
    return NULL;
}

// Rewind the list
void stvlRewind(stvlist *list)
{
    list->cursor = 0;
}
