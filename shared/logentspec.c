#include "logentspec.h"

LogEntSpec Log_Scrap = {
    .id         = "Scrap",
    .numParams  = 3,
    .paramTypes = { LP_STRING, LP_INT,   LP_INT        },
    .paramNames = { "source",  "amount", "rawamount" }
};

LogEntSpec Log_Victory = { .id = "Victory", .numParams = 0 };
LogEntSpec Log_Defeat  = { .id         = "Defeat",
                           .numParams  = 1,
                           .paramTypes = { LP_STRING },
                           .paramNames = "reason" };
LogEntSpec Log_Abandon = { .id         = "Abandon",
                           .numParams  = 1,
                           .paramTypes = { LP_STRING },
                           .paramNames = "reason" };
