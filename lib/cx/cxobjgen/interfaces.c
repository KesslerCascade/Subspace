#include "cxobjgen.h"
#include <cx/container.h>
#include <cx/string.h>

static void fillMethods(sa_Method *methods, Interface *iface)
{
    // add parent interfaces first so the order is correct
    if (iface->parent)
        fillMethods(methods, iface->parent);

    for (int i = 0; i < saSize(iface->methods); i++) {
        saPush(methods, object, iface->methods.a[i], SA_Unique);
    }
}

bool processInterface(Interface *iface)
{
    if (iface->processed)
        return true;

    // process parent interfaces first
    if (iface->parent)
        processInterface(iface->parent);

    fillMethods(&iface->allmethods, iface);

    iface->processed = true;

    return true;
}

bool processInterfaces()
{
    for (int i = 0; i < saSize(ifaces); i++) {
        if (!processInterface(ifaces.a[i])) {
            printf("Error processing interface '%s'\n", strC(ifaces.a[i]->name));
            return false;
        }
    }
    return true;
}
