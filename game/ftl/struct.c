#include "struct.h"
#include "ftl/mem.h"

void basic_string_set(basic_string* str, const char* src)
{
    int len = strlen(src);
    if (len < sizeof(str->stackbuf)) {
        strcpy(str->stackbuf, src);
        str->buf = str->stackbuf;
    } else {
        str->buf = malloc(len + 1);
        strcpy(str->buf, src);
    }

    str->len = len;
}

void basic_string_destroy(basic_string* str)
{
    if (str->buf != str->stackbuf)
        free(str->buf);
    str->buf = str->stackbuf;
    str->len = 0;
}