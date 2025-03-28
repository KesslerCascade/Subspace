#include "hostid_private.h"
#include "cx/utils/lazyinit.h"
#include "cx/string.h"

#include <mbedtls/entropy.h>

static LazyInitState hostIdState;
static HostID hidCache;

static void hostIdInit(void *data)
{
    mbedtls_md_context_t shactx;

    // Hash several things together to protect the privacy of the end user, while still
    // creating a stable ID that doesn't change over time.
    mbedtls_md_init(&shactx);
    mbedtls_md_setup(&shactx, mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), 0);

    // random junk to seed the hash
    mbedtls_md_update(&shactx, (uint8*)"perfect semantic overload purple porcupine", 16);
    hidCache.source = hostIdPlatformInit(&shactx);

    if (!hidCache.source)
        hidCache.source = hostIdPlatformInitFallback(&shactx);

    mbedtls_md_finish(&shactx, (uint8*)hidCache.id);
    mbedtls_md_free(&shactx);
}

_Use_decl_annotations_
void hostId(HostID *id)
{
    lazyInit(&hostIdState, hostIdInit, 0);

    *id = hidCache;
}
