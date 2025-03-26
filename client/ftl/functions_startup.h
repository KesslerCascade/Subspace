#include "ftl/ftl.h"
#include "hook/function.h"

typedef int (*FUNCTYPE(sil_main))(int argc, const char **argv);
DECLFUNC(sil_main);