#include "settip.h"
#include <cx/string.h>

void setTip(Ihandle* ih, strref tip, strref title, int icon)
{
    bool useballoon = !osIsWine();   // balloon tooltips don't work well on wine

    IupSetAttribute(ih, "TIP", strC(tip));
    IupSetInt(ih, "TIPDELAY", 30000);
}
