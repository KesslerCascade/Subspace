#include "minicrt.h"

int _fltused = 0;

void minicrt_init(void)
{
    _sm_init();
}
