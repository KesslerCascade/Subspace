#pragma once

#define STRINGIFY1(x)     #x
#define VERSION_TO_STR(x) STRINGIFY1(x)

#define SUBSPACE_MAJOR        0
#define SUBSPACE_MAJOR_STRING VERSION_TO_STR(SUBSPACE_MAJOR)
#define SUBSPACE_MINOR        6
#define SUBSPACE_MINOR_STRING VERSION_TO_STR(SUBSPACE_MINOR)
#define SUBSPACE_SERIES       "alpha"

extern const int subspace_version_maj;
extern const int subspace_version_min;
extern const char* subspace_version_series;
extern const char* subspace_version_str;
