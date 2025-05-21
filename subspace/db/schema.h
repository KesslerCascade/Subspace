#pragma once

#include "subspace.h"
typedef struct Database Database;

#define DB_CURRENT_SCHEMA_VER 0

// these are all SYNCHRONOUS functions that must be run single-threaded

typedef struct sqlite3 sqlite3;
bool dbCreateSchema(sqlite3 *db);
bool dbUpgradeSchema(sqlite3* db, int from);
