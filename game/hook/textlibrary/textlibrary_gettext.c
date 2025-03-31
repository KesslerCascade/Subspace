#include "feature/numerichull.h"
#include "ftl/textlibrary.h"
#include "subspacegame.h"

basic_string* subspace_TextLibrary_GetText(TextLibrary* self, basic_string* text, basic_string* out, basic_string* lang) {
    if (NumericHull_feature.enabled && numericHullCheckText(self, text, out))
        return out;   // out was overridden

    return TextLibrary_GetText(self, text, out, lang);
}