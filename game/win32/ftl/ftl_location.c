#include "ftl/commandgui.h"
#include "ftl/location.h"
#include "ftl/starmap.h"
#include "ftl/worldmanager.h"
#include "hook/disasmtrace.h"

Symbol SYM(Location_event_offset) = {
    SYMNAME("Location->event"),
    .find = { { .type   = SYMBOL_FIND_DISASM,
                .disasm = &WorldManager_CreateLocation_trace },
             { 0 } }
};

Symbol SYM(Location_visited_offset) = {
    SYMNAME("Location->visited"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &StarMap_GetNewLocation_trace }, { 0 } }
};

Symbol SYM(LocationEvent_eventName_offset) = {
    SYMNAME("LocationEvent->eventName"),
    .find = { { .type   = SYMBOL_FIND_DISASM,
                .disasm = &StarMap_GenerateMap_Rock_Crystal_Beacon_trace },
             { 0 } }
};
