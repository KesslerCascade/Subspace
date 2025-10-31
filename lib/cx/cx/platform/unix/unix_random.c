#include "cx/platform/os.h"

/* Test for Linux getrandom() support.
 * Since there is no wrapper in the libc yet, use the generic syscall wrapper
 * available in GNU libc and compatible libc's (eg uClibc).
 */
#if ((defined(__linux__) && defined(__GLIBC__)) || defined(__midipix__))
#include <unistd.h>
#include <sys/syscall.h>
#if defined(SYS_getrandom)
#define HAVE_GETRANDOM
#include <errno.h>

static bool getrandom_wrapper(void *buf, size_t buflen, unsigned int flags)
{
    return syscall(SYS_getrandom, buf, buflen, flags) == buflen;
}
#endif /* SYS_getrandom */
#endif /* __linux__ || __midipix__ */

#if defined(__FreeBSD__) || defined(__DragonFly__)
#include <sys/param.h>
#if (defined(__FreeBSD__) && __FreeBSD_version >= 1200000) || \
    (defined(__DragonFly__) && __DragonFly_version >= 500700)
#include <errno.h>
#include <sys/random.h>
#define HAVE_GETRANDOM
static int getrandom_wrapper(void *buf, size_t buflen, unsigned int flags)
{
    return getrandom(buf, buflen, flags) == buflen;
}
#endif /* (__FreeBSD__ && __FreeBSD_version >= 1200000) ||
          (__DragonFly__ && __DragonFly_version >= 500700) */
#endif /* __FreeBSD__ || __DragonFly__ */

/*
 * Some BSD systems provide KERN_ARND.
 * This is equivalent to reading from /dev/urandom, only it doesn't require an
 * open file descriptor, and provides up to 256 bytes per call (basically the
 * same as getentropy(), but with a longer history).
 *
 * Documentation: https://netbsd.gw.com/cgi-bin/man-cgi?sysctl+7
 */
#if (defined(__FreeBSD__) || defined(__NetBSD__)) && !defined(HAVE_GETRANDOM)
#include <sys/param.h>
#include <sys/sysctl.h>
#if defined(KERN_ARND)
#define HAVE_SYSCTL_ARND

static bool sysctl_arnd_wrapper(unsigned char *buf, size_t buflen)
{
    int name[2];
    size_t len;

    name[0] = CTL_KERN;
    name[1] = KERN_ARND;

    while (buflen > 0) {
        len = buflen > 256 ? 256 : buflen;
        if (sysctl(name, 2, buf, &len, NULL, 0) == -1) {
            return false;
        }
        buflen -= len;
        buf += len;
    }
    return true;
}
#endif /* KERN_ARND */
#endif /* __FreeBSD__ || __NetBSD__ */

#include <stdio.h>

bool osGenRandom(uint8* buffer, uint32 size)
{

#if defined(HAVE_GETRANDOM)
    return getrandom_wrapper(buffer, size, 0);
    /* Fall through if the system call isn't known. */
#endif /* HAVE_GETRANDOM */

#if defined(HAVE_SYSCTL_ARND)
    return sysctl_arnd_wrapper(buffer, size);
#else

    FILE *file = fopen("/dev/urandom", "rb");;
    if (file == NULL)
        return false;

    size_t read_len = fread(buffer, 1, size, file);
    if (read_len != size) {
        fclose(file);
        return false;
    }

    fclose(file);
    return true;
#endif /* HAVE_SYSCTL_ARND */
}
