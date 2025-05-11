#include "settip.h"
#include <cx/string.h>

void setTip(Ihandle* ih, strref tip, strref title, int icon)
{
    bool useballoon = !osIsWine();   // balloon tooltips don't work well on wine

    IupSetAttribute(ih, "TIP", strC(tip));
    if (!strEmpty(title)) {
        IupSetAttribute(ih, "TIPBALLOON", useballoon ? "YES" : "NO");
        IupSetAttribute(ih, "TIPBALLOONTITLE", strC(title));
        IupSetInt(ih, "TIPBALLOONTITLEICON", icon);
    }
    IupSetInt(ih, "TIPDELAY", 30000);
}
