#include "cx/platform/os.h"

#include <stdio.h>

bool osGenRandom(uint8* buffer, uint32 size)
{
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
}