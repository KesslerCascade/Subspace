#include "control/cmds.h"
#include "input/keybinds.h"

void cmdBindKey(ControlMsg* msg, hashtable fields)
{
    strref fname = cfieldString(fields, _S "name");
    int32 fkey   = cfieldValD(int32, fields, _S "key", -1);
    if (strEmpty(fname) || fkey == -1 || fkey >= KEYCODE_MAX)
        return;

    kbBindKey(strC(fname), fkey);
}