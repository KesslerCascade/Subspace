#pragma once
#include "feature.h"
#include "subspacegame.h"

// order within the list
typedef struct InfoBlockSettings {
    int ssver;
    int ftlver;
    int fps;
} InfoBlockSettings;

void infoBlockRender(void);
