#include "textlibrary.h"
#include "subspacegame.h"

basic_string* subspace_TextLibrary_GetText(TextLibrary* self, basic_string* text, basic_string* out, basic_string* lang) {
    if (gs.overrideHullText && text && !strcmp(text->buf, "status_hull")) {
        char buf[32];
        snprintf(buf, 32, "HULL: %d", gs.hull);
        basic_string_set(out, buf);
        return out;
    }

    return TextLibrary_GetText(self, text, out, lang);
}