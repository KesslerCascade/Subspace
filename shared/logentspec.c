#include "logentspec.h"

LogEntSpec Log_Scrap = {
    .id         = "Scrap",
    .numParams  = 3,
    .paramTypes = { LP_STRING, LP_INT,   LP_INT        },
    .paramNames = { "source",  "amount", "rawamount" }
};
