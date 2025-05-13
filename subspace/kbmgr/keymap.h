#pragma once

#include <iupkey.h>
#include <cx/container.h>

extern hashtable keymap_iuptodisplay;
extern hashtable keymap_iuptostr;
extern hashtable keymap_strtoiup;
extern hashtable keymap_iuptoftl;

void keymapInit();
