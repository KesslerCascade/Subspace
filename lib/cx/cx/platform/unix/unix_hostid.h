#pragma once

enum HOSTID_SOURCES {
    HID_SourceMachineFile  = 4,             // machine-wide hostid file
    HID_SourceUserFile     = 5,             // per-user entropy file
    HID_SourceComputerName = 6,             // Computer Name
};
