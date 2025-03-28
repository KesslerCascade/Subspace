#pragma once

enum HOSTID_SOURCES {
    HID_SourceBIOSUUID        = 10,     // SMBIOS UUID
    HID_SourceMachineRegistry = 20,     // Per-machine registry key
    HID_SourceUserRegistry    = 30,     // Per-user registry key
    HID_SourceCrypto          = 40,     // Windows CryptoAPI MachineGUID
    HID_SourceComputerName    = 50,     // Computer Name
};
