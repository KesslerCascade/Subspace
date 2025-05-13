#include "control/cmds.h"
#include "input/keybinds.h"

void cmdBindKey(ControlMsg* msg)
{
    ControlField* fname = controlMsgFindField(msg, "name");
    if (!(fname && fname->h.ftype == CF_STRING))
        return;

    ControlField* fkey = controlMsgFindField(msg, "key");
    if (!(fkey && fkey->h.ftype == CF_INT) || fkey->d.cfd_int >= KEYCODE_MAX)
        return;

    kbBindKey(fname->d.cfd_str, fkey->d.cfd_int);
}