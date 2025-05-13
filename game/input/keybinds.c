#include "keybinds.h"
#include "minicrt.h"

static bool bindreg_init;
static hashtbl bindreg;
static KeyBind* bindkey[KEYCODE_MAX];

bool kbRegisterBind(SubspaceFeature* owner, KeyBind* kb)
{
    if (!bindreg_init) {
        bindreg_init = true;
        hashtbl_init(&bindreg, 32, HT_STRING_KEYS);
    }

    kb->owner = owner;
    hashtbl_add(&bindreg, kb->name, kb);

    return true;
}

bool kbBindKey(const char* name, int key)
{
    KeyBind* bind = (KeyBind*)hashtbl_get(&bindreg, name);

    if (!bind)
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
