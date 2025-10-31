#include "struct.h"
#include "ftl/stdlib.h"

void basic_string_reset(basic_string* str)
{
    str->stackbuf[0] = 0;
    str->buf         = str->stackbuf;
    str->len         = 0;
}

void basic_string_set(basic_string* str, const char* src)
{
    int len = strlen(src);
    if (len < sizeof(str->stackbuf)) {
        strcpy(str->stackbuf, src);
        str->buf = str->stackbuf;
    } else {
        str->buf = ftl_malloc(len + 1);
        strcpy(str->buf, src);
        *(unsigned long*)str->stackbuf = len + 1;   // allocated capacity
    }

    str->len = len;
}

void basic_string_destroy(basic_string* str)
{
    if (str->buf != str->stackbuf)
        ftl_free(str->buf);
    str->stackbuf[0] = 0;
    str->buf = str->stackbuf;
    str->len = 0;
}