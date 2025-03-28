#include "cx/sys/hostid_private.h"
#include "cx/string.h"
#include <mbedtls/entropy.h>

#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/stat.h>

static bool readIdFile(const char *name, mbedtls_md_context_t *shactx, bool exact)
{
    bool ret = false;
    int fd = open(name, O_RDONLY);
    if (fd < 0)
        return false;

    uint8 buf[64];
    ssize_t sz;

    sz = read(fd, buf, sizeof(buf));
    if (exact) {
        if (sz != sizeof(buf))
            goto out;
    } else {
        while(sz > 0 && isspace(buf[sz-1]))
            sz--;       // eat trailing line feed
    }

    if (sz == 0)
        goto out;

    mbedtls_md_update(shactx, buf, sz);
    ret = true;

out:
    close(fd);
    return ret;
}

static bool getPerUserId(mbedtls_md_context_t *shactx)
{
    bool ret = false;
    const char *home = getenv("HOME");

    if (!home)
        return false;

    string cxdir = 0;
    string userfile = 0;
    strConcat(&cxdir, (string)home, _S"/.cx");
    strConcat(&userfile, cxdir, _S"/hostid");

    if (readIdFile(strC(userfile), shactx, true)) {
        ret = true;
    } else {
        struct stat sb;
        if (!stat(strC(cxdir), &sb) || !S_ISDIR(sb.st_mode)) {
            mkdir(strC(cxdir), 0755);
        }

        int fd = open(strC(userfile), O_WRONLY | O_CREAT | O_TRUNC, 0755);
        if (fd >= 0) {
            mbedtls_entropy_context entropy;
            uint8 randbuf[64];

            mbedtls_entropy_init(&entropy);
            mbedtls_entropy_func(&entropy, (unsigned char*)randbuf, sizeof(randbuf));
            mbedtls_entropy_free(&entropy);

            if (write(fd, randbuf, 64) == 64) {
                mbedtls_md_update(shactx, randbuf, sizeof(randbuf));
                ret = true;
            }
            close(fd);
        }
    }

    strDestroy(&userfile);
    strDestroy(&cxdir);
    return ret;
}

int32 hostIdPlatformInit(mbedtls_md_context_t *shactx)
{
    if (readIdFile("/etc/hostid", shactx, false))
        return HID_SourceMachineFile;
    else if (readIdFile("/etc/machine-id", shactx, false))
        return HID_SourceMachineFile;
    else if (getPerUserId(shactx))
        return HID_SourceUserFile;

    return 0;
}

int32 hostIdPlatformInitFallback(mbedtls_md_context_t *shactx)
{
    // ultimate panic fallback
    char buf[256];
    gethostname(buf, 255);
    buf[255] = 0;           // just in case it's exactly 255
    mbedtls_md_update(shactx, (uint8*)buf, cstrLen(buf));
    return HID_SourceComputerName;
}
