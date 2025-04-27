#pragma once

#include <stdbool.h>

enum IMAGE_DRIVER { IMAGE_DRIVER_NONE = 0, IMAGE_DRIVER_PNG, IMAGE_DRIVER_SVG };

typedef struct ImageLoad ImageLoad;
bool imageDriverPng(ImageLoad* il);
bool imageDriverSvg(ImageLoad* il);
