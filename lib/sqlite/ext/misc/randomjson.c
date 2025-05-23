/*
** 2023-04-28
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
******************************************************************************
**
** This SQLite extension implements a the random_json(SEED) and
** random_json5(SEED) functions.  Given a numeric SEED value, these
** routines generate pseudo-random JSON or JSON5, respectively.  The
** same value is always generated for the same seed.
**
** These SQL functions are intended for testing.  They do not have any
** practical real-world use, that we know of.
**
** COMPILE:
**
**     gcc --shared -fPIC -o randomjson.so -I. ext/misc/randomjson.c
**
** USING FROM THE CLI:
**
**     .load ./randomjson
**     SELECT random_json(1);
*/
#include "sqlite3ext.h"
SQLITE_EXTENSION_INIT1
#include <assert.h>
#include <string.h>
#include <stdlib.h>

/* Pseudo-random number generator */
typedef struct Prng {
  unsigned int x, y;
} Prng;

/* Reseed the PRNG */
static void prngSeed(Prng *p, unsigned int iSeed){
  p->x = iSeed | 1;
  p->y = iSeed;
}

/* Extract a random number */
static unsigned int prngInt(Prng *p){
  p->x = (p->x>>1) ^ ((1+~(p->x&1)) & 0xd0000001);
  p->y = p->y*1103515245 + 12345;
  return p->x ^ p->y;
}

static const char *azJsonAtoms[] = {
/* JSON                      JSON-5 */
  "0",                       "0",
  "1",                       "1",
  "-1",                      "-1",
  "2",                       "+2",
  "3",                       "3",
  "2.5",                     "2.5",
  "0.75",                    ".75",
  "-4.0e2",                  "-4.e2",
  "5.0e-3",                  "+5e-3",
  "0",                       "0x0",
  "512",                     "0x200",
  "256",                     "+0x100",
  "-2748",                   "-0xabc",
  "true",                    "true",
  "false",                   "false",
  "null",                    "null",
  "9.0e999",                 "Infinity",
  "-9.0e999",                "-Infinity",
  "9.0e999",                 "+Infinity",
  "null",                    "NaN",
  "-0.0005123",              "-0.0005123",
  "4.35e-3",                 "+4.35e-3",
  "\"gem\\\"hay\"",          "\"gem\\\"hay\"",
  "\"icy'joy\"",             "'icy\\'joy\'",
  "\"keylog\"",              "\"key\\\nlog\"",
  "\"mix\\\\\\tnet\"",       "\"mix\\\\\\tnet\"",
  "{}",                      "{}",
  "[]",                      "[]",
  "[]",                      "[/*empty*/]",
  "{}",                      "{//empty\n}",
  "\"ask\"",                 "\"ask\"",
  "\"bag\"",                 "\"bag\"",
  "\"can\"",                 "\"can\"",
  "\"day\"",                 "\"day\"",
  "\"end\"",                 "'end'",
  "\"fly\"",                 "\"fly\"",
  "\"\"",                    "\"\"",
};
static const char *azJsonTemplate[] = {
  /* JSON                                      JSON-5 */
  "{\"a\":%,\"b\":%,\"c\":%}",                 "{a:%,b:%,c:%}",
  "{\"a\":%,\"b\":%,\"c\":%,\"d\":%,\"e\":%}", "{a:%,b:%,c:%,d:%,e:%}",
  "{\"a\":%,\"b\":%,\"c\":%,\"d\":%,\"\":%}",  "{a:%,b:%,c:%,d:%,\"\":%}",
  "{\"d\":%}",                                 "{d:%}",
  "{\"eeee\":%, \"ffff\":%}",                  "{eeee:% /*and*/, ffff:%}",
  "{\"$g\":%,\"_h_\":%}",                      "{$g:%,_h_:%,}",
  "{\"x\":%,\n  \"y\":%}",                     "{\"x\":%,\n  \"y\":%}",
  "{\"a b c d\":%,\"e\":%,\"f\":%,\"x\":%,\"y\":%}",
                                           "{\"a b c d\":%,e:%,f:%,x:%,y:%}",
  "{\"Z\":%}",                                 "{Z:%,}",
  "[%]",                                       "[%,]",
  "[%,%]",                                     "[%,%]",
  "[%,%,%]",                                   "[%,%,%,]",
  "[%,%,%,%]",                                 "[%,%,%,%]",
  "[%,%,%,%,%]",                               "[%,%,%,%,%]",
};

#define count(X)  (sizeof(X)/sizeof(X[0]))

#define STRSZ 10000

static void jsonExpand(
  const char *zSrc,
  char *zDest,
  Prng *p,
  int eType,            /* 0 for JSON, 1 for JSON5 */
  unsigned int r        /* Growth probability 0..1000.  0 means no growth */
){
  unsigned int i, j, k;
  const char *z;
  size_t n;

  j = 0;
  if( zSrc==0 ){
    k = prngInt(p)%(count(azJsonTemplate)/2);
    k = k*2 + eType;
    zSrc = azJsonTemplate[k];
  }
  if( strlen(zSrc)>=STRSZ/10 ) r = 0;
  for(i=0; zSrc[i]; i++){
    if( zSrc[i]!='%' ){
      if( j<STRSZ ) zDest[j++] = zSrc[i];
      continue;
    }
    if( r==0 || (r<1000 && (prngInt(p)%1000)<=r) ){
      /* Fill in without values without any new % */
      k = prngInt(p)%(count(azJsonAtoms)/2);
      k = k*2 + eType;
      z = azJsonAtoms[k];
    }else{
      /* Add new % terms */
      k = prngInt(p)%(count(azJsonTemplate)/2);
      k = k*2 + eType;
      z = azJsonTemplate[k];
    }
    n = strlen(z);
    if( j+n<STRSZ ){
      memcpy(&zDest[j], z, n);
      j += n;
    }
  }
  zDest[STRSZ-1] = 0;
  if( j<STRSZ ) zDest[j] = 0;
}

static void randJsonFunc(
  sqlite3_context *context,
  int argc,
  sqlite3_value **argv
){
  unsigned int iSeed;
  int eType = *(int*)sqlite3_user_data(context);
  Prng prng;
  char z1[STRSZ+1], z2[STRSZ+1];

  iSeed = (unsigned int)sqlite3_value_int(argv[0]);
  prngSeed(&prng, iSeed);
  jsonExpand(0, z2, &prng, eType, 1000);
  jsonExpand(z2, z1, &prng, eType, 1000);
  jsonExpand(z1, z2, &prng, eType, 100);
  jsonExpand(z2, z1, &prng, eType, 0);
  sqlite3_result_text(context, z1, -1, SQLITE_TRANSIENT);
}

#ifdef _WIN32
__declspec(dllexport)
#endif
int sqlite3_randomjson_init(
  sqlite3 *db, 
  char **pzErrMsg, 
  const sqlite3_api_routines *pApi
){
  static int cOne = 1;
  static int cZero = 0;
  int rc = SQLITE_OK;
  SQLITE_EXTENSION_INIT2(pApi);
  (void)pzErrMsg;  /* Unused parameter */
  rc = sqlite3_create_function(db, "random_json", 1,
                   SQLITE_UTF8|SQLITE_INNOCUOUS|SQLITE_DETERMINISTIC,
                   &cZero, randJsonFunc, 0, 0);
  if( rc==SQLITE_OK ){
    rc = sqlite3_create_function(db, "random_json5", 1,
                   SQLITE_UTF8|SQLITE_INNOCUOUS|SQLITE_DETERMINISTIC,
                   &cOne, randJsonFunc, 0, 0);
  }
  return rc;
}
