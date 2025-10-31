#include "hostid_private.h"
#include "cx/utils/lazyinit.h"
#include "cx/string.h"

static LazyInitState hostIdState;
static HostID hidCache;

static void hostIdInit(void *data)
{
    Digest shactx;

    // Hash several things together to protect the privacy of the end user, while still
    // creating a stable ID that doesn't change over time.
    digestInit(&shactx, DIGEST_SHA256);

    // random junk to seed the hash
    digestUpdate(&shactx, (uint8*)"perfect semantic overload purple porcupine", 42);
    hidCache.source = hostIdPlatformInit(&shactx);

    if (!hidCache.source)
        hidCache.source = hostIdPlatformInitFallback(&shactx);

    digestFinish(&shactx, (uint8*)hidCache.id);
}

_Use_decl_annotations_
void hostId(HostID *id)
{
    lazyInit(&hostIdState, hostIdInit, 0);

    *id = hidCache;
}
