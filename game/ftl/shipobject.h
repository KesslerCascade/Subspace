#pragma once
#include "ftl/ftl.h"
#include "ftl/struct.h"
#include "hook/function.h"

typedef struct ShipInfo {
    // map<basic_string, int>
    rb_tree augList;
    // map<basic_string, int>
    rb_tree equipList;
    int augCount;
} ShipInfo;

typedef struct AugListItem {
    rb_tree_node node;
    basic_string key;
    int value;
} AugListItem;

typedef int (*FUNCTYPE(ShipObject_HasEquipment))(ShipObject* ship, basic_string* blueName);
DECLFUNC(ShipObject_HasEquipment);
#define ShipObject_HasEquipment(self, blueName) \
    FCALL(ftlbase, ShipObject_HasEquipment, self, blueName)

typedef int (*FUNCTYPE(ShipObject_HasAugmentation))(ShipObject* ship, basic_string* augId);
DECLFUNC(ShipObject_HasAugmentation);
#define ShipObject_HasAugmentation(self, augId) \
    FCALL(ftlbase, ShipObject_HasAugmentation, self, augId)

typedef float (*FUNCTYPE(ShipObject_GetAugmentationValue))(ShipObject* ship, basic_string* augId);
DECLFUNC(ShipObject_GetAugmentationValue);
#define ShipObject_GetAugmentationValue(self, augId) \
    FCALL(ftlbase, ShipObject_GetAugmentationValue, self, augId)

DECLSYM(ShipObject_shipInfoList);
// vector<ShipInfo>
#define ShipObject_shipInfoList ((vector*)symAddr(ftlbase, ShipObject_shipInfoList))