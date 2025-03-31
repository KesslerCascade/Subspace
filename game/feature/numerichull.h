#pragma once
#include "ftl/shipmanager.h"
#include "ftl/textlibrary.h"
#include "subspacegame.h"

extern SubspaceFeature NumericHull_feature;

void numericHullBeforeRenderHealth(ShipManager* sm, bool renderText);
void numericHullAfterRenderHealth(bool renderText);
void numericHullBeforeRenderTarget(ShipManager* smtgt);
void numericHullAfterRenderTarget(void);

bool numericHullCheckText(TextLibrary* tlib, basic_string* text, basic_string* out);