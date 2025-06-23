#pragma once
#include "ftl/ftl.h"
#include "hook/function.h"

typedef bool (*FUNCTYPE(Settings_GetDlcEnabled))(void);
DECLFUNC(Settings_GetDlcEnabled);
#define Settings_GetDlcEnabled() FCALL(ftlbase, Settings_GetDlcEnabled)
