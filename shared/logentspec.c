#include "logentspec.h"

LogEntSpec Log_Scrap = {
    .id         = "Scrap",
    .numParams  = 3,
    .priority   = 15, // Should come after events; since those often give scrap rewards
    .paramTypes = { LP_STRING, LP_INT,   LP_INT      },
    .paramNames = { "source",  "amount", "rawamount" }
};

LogEntSpec Log_HullDamage = {
    .id         = "HullDamage",
    .numParams  = 2,
    .paramTypes = { LP_STRING, LP_INT   },
    .paramNames = { "source",  "amount" }
};

LogEntSpec Log_Victory = { .id = "Victory", .numParams = 0 };
LogEntSpec Log_Defeat  = { .id         = "Defeat",
                           .numParams  = 1,
                           .paramTypes = { LP_STRING },
                           .paramNames = { "reason" } };
LogEntSpec Log_Abandon = { .id         = "Abandon",
                           .numParams  = 1,
                           .paramTypes = { LP_STRING },
                           .paramNames = { "reason" } };

LogEntSpec Log_Jump = { .id = "Jump", .priority = 2, .numParams = 0 };
LogEntSpec Log_Wait = { .id = "Wait", .priority = 2, .numParams = 0 };

LogEntSpec Log_Event = {
    .id         = "Event",
    .numParams  = 3,
    .priority   = 10, // make sure this is sent AFTER LoadGame and such
    .paramTypes = { LP_STRING, LP_INT,    LP_INT  },
    .paramNames = { "name",    "initial", "visit" }
};

LogEntSpec Log_Ship = { .id         = "Ship",
                        .numParams  = 1,
                        .priority   = 11,   // should always be sent AFTER the Event
                        .paramTypes = { LP_STRING },
                        .paramNames = { "name" } };

LogEntSpec Log_Sector = {
    .id         = "Sector",
    .numParams  = 2,
    .priority   = 1, // before Jump / Wait
    .paramTypes = { LP_STRING, LP_INT },
    .paramNames = { "type",    "seed" }
};

LogEntSpec Log_Start = {
    .id         = "Start",
    .numParams  = 4,
    .priority   = 0,
    .paramTypes = { LP_STRING, LP_STRING, LP_INT, LP_INT       },
    .paramNames = { "ship",    "name",    "seed", "difficulty" }
};
