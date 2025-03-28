#include "textlibrary.h"
#include "subspaceclient.h"

basic_string* subspace_TextLibrary_GetText(TextLibrary* self, basic_string* text, basic_string* out, basic_string* lang) {
    if(tgd.overrideHullText && text && !strcmp(text->buf, "status_hull")) {
        char buf[32];
        snprintf(buf, 32, "HULL: %d", tgd.hull);
        basic_string_set(out, buf);
        return out;
    }

    return TextLibrary_GetText(self, text, out, lang);
}