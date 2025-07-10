/*
 * Copyright © 2021 Jeremiah Ikosin
 * Distributed under the terms of the MIT license.
 */

#include "utils/cxutils.h"
#include <cx/debug/error.h>
#include <cx/string.h>

//src
int _cxml_read_file(VFS *vfs, strref file_name, char **dest_buffer) {
#define BUFF_SIZE (4096)
    if (!file_name || !dest_buffer) return 0;
    VFSFile *file = vfsOpen(vfs, file_name, FS_Read);
    if (!file){
        //cxml_error("Could not open file (%s): %s\n", strC(file_name), cxErrMsg(cxerr));
        return 0;
    }
    vfsSeek(file, 0, FS_End);
    size_t bytecount = vfsTell(file);
    vfsSeek(file, 0, FS_Set);
    if (bytecount == 0){
        vfsClose(file);
        //cxml_error("Perhaps \"%s\" is an empty file?\n", strC(file_name));
        return 0;
    }
    *dest_buffer = ALLOCR(char, (bytecount+1), "Not enough memory to read file (%s)\n", strC(file_name));

    size_t didread;
    if (!vfsRead(file, (*dest_buffer), bytecount, &didread) || didread < bytecount) {
        vfsClose(file);
        //cxml_error("Error reading file (%s): %s\n", strC(file_name), csErrMsg(cxerr));
        return 0;

    }
    (*dest_buffer)[bytecount] = '\0';
    vfsClose(file);
    return 1;
#undef BUFF_SIZE
}

int _cxml_write_file(VFS *vfs, strref file_name, const char *dest_buffer, size_t len){
    if (!file_name || !dest_buffer) return 0;
    VFSFile *file = vfsOpen(vfs, file_name, FS_Overwrite);
    if (!file){
        //cxml_error("Could not open file (%s): %s\n", strC(file_name), csErrMsg(cxerr));
        return 0;
    }
    size_t didwrite;
    if (!vfsWrite(file, dest_buffer, len, &didwrite) || didwrite < len){
        vfsClose(file);
        //cxml_error("Error writing to file (%s): <errno: %d>\n", file_name, errno);
        return 0;
    }
    vfsClose(file);
    return 1;
}
