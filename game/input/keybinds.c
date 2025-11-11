#include "keybinds.h"

static bool bindreg_init;
static hashtable bindreg;
static KeyBind* bindkey[KEYCODE_MAX];

bool kbRegisterBind(SubspaceFeature* owner, KeyBind* kb)
{
    if (!bindreg_init) {
        bindreg_init = true;
        htInit(&bindreg, string, ptr, 32);
    }

    kb->owner = owner;
    htInsert(&bindreg, strref, kb->name, ptr, kb);

    return true;
}

bool kbBindKey(strref name, int key)
{
    KeyBind* bind = NULL;
    if (!htFind(bindreg, strref, name, ptr, &bind))
        return false;

    if (bind->key > 0) {
        // unbind previous bind
        KeyBind** tochg = &bindkey[bind->key];
        while (*tochg) {
            if (*tochg == bind) {
                *tochg     = bind->next;
                bind->next = NULL;
                break;
            }
            tochg = &((*tochg)->next);
        }
    }

    bind->key = key;
    if (key > 0) {
        // add to end of chain if necessary
        KeyBind** tochg = &bindkey[key];
        while (*tochg) {
            tochg = &((*tochg)->next);
        }
        *tochg = bind;
    }

    return true;
}

KeyBind* kbGetBinds(int key)
{
    if (key < KEYCODE_MAX)
        return bindkey[key];
    return NULL;
}
