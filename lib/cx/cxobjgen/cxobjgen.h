#pragma once

#include "objtypes.h"
#include <cx/fs/fs.h>
#include <cx/fs/path.h>
#include <cx/serialize/lineparse.h>
#include <cx/serialize/sbfsfile.h>
#include <cx/serialize/sbstring.h>

extern sa_Interface ifaces;
extern hashtable ifidx;
extern sa_Class classes;
extern hashtable clsidx;
extern hashtable weakrefidx;
extern sa_string includes;
extern sa_string implincludes;
extern sa_string deps;
extern sa_string structs;
extern sa_string fwdclass;
extern sa_ComplexArrayType artypes;
extern hashtable knownartypes;
extern string cpassthrough;
extern bool needmixinimpl;

uint8 *lazyPlatformPath(string path);
bool parseFile(string fname, string *realfn, sa_string searchpath, bool included, bool required);
bool processInterfaces();
bool processClasses();
bool writeHeader(string fname);
bool writeImpl(string fname, bool mixinimpl);
bool getAnnotation(sa_string *out, sa_sarray_string annotations, string afind);

void methodImplName(string *out, Class *cls, string mname);
void methodCallName(string *out, Class *cls, string mname);
void mixinMemberName(string *out, Class *cls);
void methodAnnotations(string *out, Method *m);
void paramAnnotations(string *out, Param *p);
