#pragma once

#define clamplow(val, low) ((val)>(low)?(val):(low))
#define clamphigh(val, high) ((val)<(high)?(val):(high))
#define clamp(val, low, high) ((val)>(low)?((val)<(high)?(val):(high)):(low))

#ifndef min
#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))
#endif
