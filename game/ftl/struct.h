#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "ftl/types.h"

// Helper macro for getting members of structures where the offsets are found at runtime through
// disassembly. This uses the symbol resolution system to automatically calculate and cache the
// offset as a pseudo-symbol.

#define MEMBER(base, ParentType, parent, MemberType, member) \
    (*(MemberType*)((((uintptr_t)(parent)) + (symAddr(base, ParentType##_##member##_offset)))))

#define MEMBER_FIXED(base, ParentType, parent, MemberType, offset) \
    (*(MemberType*)((((uintptr_t)(parent)) + (offset))))

typedef struct GL_Color {
    float r;
    float g;
    float b;
    float a;
} GL_Color;

typedef struct Pointf {
    float x, y;
} Pointf;

typedef struct basic_string {
    char* buf;
    unsigned long len;
    char stackbuf[16];
} basic_string;

void basic_string_reset(basic_string* str);
void basic_string_set(basic_string* str, const char* src);
void basic_string_destroy(basic_string* str);

typedef struct vector {
    void* start;            // first element
    void* finish;           // just past last element
    void* end_of_storage;   // just past end of allocated space
} vector;

typedef struct rb_tree_node rb_tree_node;
typedef struct rb_tree_node {
    int color;
    rb_tree_node* parent;
    rb_tree_node* left;
    rb_tree_node* right;
} rb_tree_node;

typedef struct rb_tree_header {
    rb_tree_node h;
    size_t node_count;
} rb_tree_header;

typedef struct rb_tree {
    int pad;   // void* ?
    rb_tree_header h;
} rb_tree;

typedef struct TextString {
    basic_string data;
    bool isLiteral;
} TextString;

typedef struct Damage {
    int iDamage;
    int iShieldPiercing;
    int fireChance;
    int breachChance;
    int stunDamage;
    int iIonDamage;
    int iSystemDamage;
    int iPersDamage;
    bool bHullBuster;
    int ownerId;
    int selfId;
    bool bLockdown;
    bool crystalShard;
    bool bFriendlyFire;
    int iStun;
} Damage;

typedef struct EquipmentBoxItem {
    ProjectileFactory* pWeapon;
    Drone* pDrone;
    CrewMember* pCrew;
    AugmentBlueprint* augment;
} EquipmentBoxItem;
