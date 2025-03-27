#include "ftl/ftl.h"
#include "hook/function.h"

extern DisasmTrace sil_main_trace;
extern DisasmTrace sil_main_CApp_trace;

typedef int (*FUNCTYPE(sil_main))(int argc, const char **argv);
DECLFUNC(sil_main);