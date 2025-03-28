#include "cxobjgen.h"
#include <cx/container.h>
#include <cx/fs/file.h>
#include <cx/string.h>

static bool isObjectType(Param *param)
{
    if (!strEq(param->predecr, _S"*"))
        return false;

    if (strEq(param->type, _S"ObjInst"))
        return true;

    if (htHasKey(clsidx, string, param->type))
        return true;

    if(htHasKey(weakrefidx, string, param->type))
        return true;

    if(saFind(fwdclass, string, param->type) != -1)
        return true;

    return false;
}

static void writeComments(StreamBuffer *bf, sa_string comments, int column, bool spacer)
{
    string ln = 0;
    string spaces = 0;

    if (column > 0) {
        uint8 *tmp = strBuffer(&spaces, column);
        memset(tmp, ' ', column);
    }

    if (spacer && saSize(comments) > 0) {
        strConcat(&ln, spaces, _S"//");
        sbufPWriteLine(bf, ln);
    }
    for (int i = 0; i < saSize(comments); ++i) {
        strNConcat(&ln, spaces, _S"// ", comments.a[i]);
        sbufPWriteLine(bf, ln);
    }
    strDestroy(&ln);
    strDestroy(&spaces);
}

static void writeFuncComment(StreamBuffer *bf, strref name, Class *cls, Method *m)
{
    if (!name)
        name = m->name;

    string ln = 0, temp = 0;
    if (!m->standalone)
        strNConcat(&ln, _S"// ", m->returntype, m->predecr, _S" ", name, _S"(", cls->name, _S"* self");
    else
        strNConcat(&ln, _S"// ", m->returntype, m->predecr, _S" ", name, _S"(");

    for (int j = 0; j < saSize(m->params); j++) {
        Param *p = m->params.a[j];
        string ptype = p->type;
        string ppre = p->predecr;

        if (strEq(ptype, _S"object") && strEmpty(ppre)) {
            ptype = cls->name;
            ppre = _S"*";
        } else if(strEq(ptype, _S"weak") && strEmpty(ppre)) {
            strNConcat(&temp, _S"Weak(", cls->name, _S")");
            ptype = temp;
            ppre = _S"*";
        }

        if (!m->standalone || j > 0)
            strNConcat(&ln, ln, _S", ");
        strNConcat(&ln, ln, ptype, ppre, _S" ", p->name, p->postdecr);
    }
    strAppend(&ln, _S");");
    sbufPWriteLine(bf, ln);
    strDestroy(&temp);
    strDestroy(&ln);
}

static void writeUnbound(StreamBuffer *bf, Class *cls, Class *cur, sa_Method *done)
{
    string ln = 0, implname = 0, callname = 0, annos = 0, temp = 0;

    for (int i = 0; i < saSize(cur->methods); i++) {
        Method *m = cur->methods.a[i];
        if (!m->unbound)
            continue;

        if (saFind(*done, object, m) != -1)
            continue;

        if (cls == cur) {
            methodImplName(&implname, cls, m->name);
            methodAnnotations(&annos, m);
            if (!m->standalone)
                strNConcat(&ln, annos, m->returntype, m->predecr, _S" ", implname, _S"(_In_ ", cls->name, _S"* self");
            else
                strNConcat(&ln, annos, m->returntype, m->predecr, _S" ", implname, _S"(");

            for (int j = 0; j < saSize(m->params); j++) {
                Param *p = m->params.a[j];
                string ptype = p->type;
                string ppre = p->predecr;

                if (strEq(ptype, _S"object") && strEmpty(ppre)) {
                    ptype = cls->name;
                    ppre = _S"*";
                } else if(strEq(ptype, _S"weak") && strEmpty(ppre)) {
                    strNConcat(&temp, _S"Weak(", cls->name, _S")");
                    ptype = temp;
                    ppre = _S"*";
                }

                if (!m->standalone || j > 0)
                    strNConcat(&ln, ln, _S", ");
                paramAnnotations(&annos, p);
                strNConcat(&ln, ln, annos, ptype, ppre, _S" ", p->name, p->postdecr);
            }
            strAppend(&ln, _S");");
            sbufPWriteLine(bf, ln);

            methodCallName(&callname, cls, m->name);
            writeFuncComment(bf, callname, cls, m);
            writeComments(bf, m->comments, 0, true);
            strNConcat(&ln, _S"#define ", callname, m->standalone ? _S"(" : _S"(self");
            for (int j = 0; j < saSize(m->params); j++) {
                if (!m->standalone || j > 0)
                    strNConcat(&ln, ln, _S", ", m->params.a[j]->name);
                else
                    strNConcat(&ln, ln, m->params.a[j]->name);
            }
            if (m->standalone)
                strNConcat(&ln, ln, _S") ", implname, _S"(");
            else
                strNConcat(&ln, ln, _S") ", implname, _S"(", cur->name, _S"(self)");
            for (int j = 0; j < saSize(m->params); j++) {
                string extra1 = 0, extra2 = 0;
                // use cast macro for classes that we know about
                if (isObjectType(m->params.a[j])) {
                    strConcat(&extra1, m->params.a[j]->type, _S"(");
                    extra2 = _S")";
                }
                if (!m->standalone || j > 0)
                    strNConcat(&ln, ln, _S", ", extra1, m->params.a[j]->name, extra2);
                else
                    strNConcat(&ln, ln, extra1, m->params.a[j]->name, extra2);
                strDestroy(&extra1);
            }
            strAppend(&ln, _S")");
            sbufPWriteLine(bf, ln);
            sbufPWriteEOL(bf);
        } else {
            // only emit standalone functions for the current class
            if (m->standalone)
                continue;

            methodCallName(&callname, cls, m->name);
            writeFuncComment(bf, callname, cls, m);
            writeComments(bf, m->comments, 0, true);
            strNConcat(&ln, _S"#define ", callname, _S"(self");
            for (int j = 0; j < saSize(m->params); j++) {
                strNConcat(&ln, ln, _S", ", m->params.a[j]->name);
            }
            methodImplName(&implname, cur, m->name);
            strNConcat(&ln, ln, _S") ", implname, _S"(", cur->name, _S"(self)");
            for (int j = 0; j < saSize(m->params); j++) {
                // use cast macro for classes that we know about
                if (isObjectType(m->params.a[j]))
                    strNConcat(&ln, ln, _S", ", m->params.a[j]->type, _S"(", m->params.a[j]->name, _S")");
                else
                    strNConcat(&ln, ln, _S", ", m->params.a[j]->name);
            }
            strAppend(&ln, _S")");
            sbufPWriteLine(bf, ln);
            sbufPWriteEOL(bf);
        }

        saPush(done, object, m);
    }

    strDestroy(&implname);
    strDestroy(&callname);
    strDestroy(&annos);
    strDestroy(&temp);
    strDestroy(&ln);
    if (cur->parent)
        writeUnbound(bf, cls, cur->parent, done);
}

void writeIfDecl(StreamBuffer *bf, Interface *iface)
{
    string ln = 0, tmp = 0, annos = 0;

    strNConcat(&ln, _S"typedef struct ", iface->name, _S" {");
    sbufPWriteLine(bf, ln);

    sbufPWriteLine(bf, _S"    ObjIface* _implements;");
    sbufPWriteLine(bf, _S"    ObjIface* _parent;");
    sbufPWriteLine(bf, _S"    size_t _size;");
    sbufPWriteEOL(bf);

    for (int i = 0; i < saSize(iface->allmethods); i++) {
        Method *m = iface->allmethods.a[i];
        writeComments(bf, m->comments, 4, false);
        methodAnnotations(&annos, m);
        strNConcat(&ln, _S"    ", annos, m->returntype, m->predecr, _S" ", _S"(*", m->name, _S")(_In_ void* self");
        for (int j = 0; j < saSize(m->params); j++) {
            Param *p = m->params.a[j];
            string ptype = p->type;
            string ppre = p->predecr;

            if ((strEq(ptype, _S"object") || strEq(ptype, _S"weak")) && strEmpty(ppre)) {
                ptype = _S"void";
                ppre = _S"*";
            }

            paramAnnotations(&annos, p);
            strNConcat(&tmp, _S", ", annos, ptype, ppre, _S" ", p->name, p->postdecr);
            strAppend(&ln, tmp);
        }
        strAppend(&ln, _S");");
        sbufPWriteLine(bf, ln);
    }

    strNConcat(&ln, _S"} ", iface->name, _S";");
    sbufPWriteLine(bf, ln);
    strNConcat(&ln, _S"extern ", iface->name, _S" ", iface->name, _S"_tmpl;");
    sbufPWriteLine(bf, ln);
    sbufPWriteEOL(bf);

    strDestroy(&tmp);
    strDestroy(&annos);
    strDestroy(&ln);
}

void writeForwardDecl(StreamBuffer *bf, string name)
{
    string ln = 0;
    strNConcat(&ln, _S"typedef struct ", name, _S" ", name, _S";");
    sbufPWriteLine(bf, ln);
    strDestroy(&ln);
}

void writeForwardWeakRefDecl(StreamBuffer *bf, string name)
{
    string ln = 0;
    strNConcat(&ln, _S"typedef struct ", name, _S"_WeakRef ", name, _S"_WeakRef;");
    sbufPWriteLine(bf, ln);
    strDestroy(&ln);
}

void writeSArrayDecl(StreamBuffer *bf, string name)
{
    string ln = 0;
    strNConcat(&ln, _S"saDeclarePtr(", name, _S");");
    sbufPWriteLine(bf, ln);
    strDestroy(&ln);
}

void writeSArrayWeakRefDecl(StreamBuffer *bf, string name)
{
    string ln = 0;
    strNConcat(&ln, _S"saDeclarePtr(", name, _S"_WeakRef);");
    sbufPWriteLine(bf, ln);
    strDestroy(&ln);
}

void writeComplexArrayDecl(StreamBuffer *bf, ComplexArrayType *cat)
{
    string ln = 0;
    strNConcat(&ln, _S"saDeclareType(", cat->tname, _S", sa_", cat->tsubtype, _S");");
    sbufPWriteLine(bf, ln);
    strDestroy(&ln);
}

static void writeClassMember(StreamBuffer *bf, Class *cls, Member *m)
{
    string ln = 0;
    string predecr = 0;

    strDup(&predecr, m->predecr);

    if (saSize(m->comments) > 1)
        writeComments(bf, m->comments, 4, false);

    if (!strEq(m->vartype, _S"hashtable") &&
        saSize(m->fulltype) > 0 &&
        !strEq(m->fulltype.a[0], _S"sarray")) {
        for (int i = 0; i < saSize(m->fulltype); i++) {
            if (strEq(m->fulltype.a[i], _S"object") || strEq(m->fulltype.a[i], _S"weak")) {
                strPrepend(_S"*", &predecr);
            }
        }
    }

    strNConcat(&ln, _S"    ", m->vartype, predecr, _S" ", m->name, m->postdecr, _S";");

    if (saSize(m->comments) == 1)
        strNConcat(&ln, ln, _S"        // ", m->comments.a[0]);

    sbufPWriteLine(bf, ln);

    strDestroy(&predecr);
    strDestroy(&ln);
}

static void writeClassTypeMarkers(StreamBuffer *bf, Class *cls)
{
    string ln = 0;
    strNConcat(&ln, _S"        void* _is_", cls->name, _S";");
    sbufPWriteLine(bf, ln);
    strDestroy(&ln);

    if (cls->parent)
        writeClassTypeMarkers(bf, cls->parent);
}

static void writeClassWeakRefTypeMarkers(StreamBuffer *bf, Class *cls)
{
    string ln = 0;
    strNConcat(&ln, _S"        void* _is_", cls->name, _S"_WeakRef;");
    sbufPWriteLine(bf, ln);
    strDestroy(&ln);

    if(cls->parent)
        writeClassWeakRefTypeMarkers(bf, cls->parent);
}

void writeClassDecl(StreamBuffer *bf, Class *cls)
{
    string ln = 0, mname = 0;

    strNConcat(&ln, _S"typedef struct ", cls->name, _S" {");
    sbufPWriteLine(bf, ln);

    if (!cls->mixin) {
        sbufPWriteLine(bf, _S"    union {");
        if (cls->classif)
            strNConcat(&ln, _S"        ", cls->name, _S"_ClassIf* _;");
        else
            strDup(&ln, _S"        ObjIface* _;");
        sbufPWriteLine(bf, ln);
        writeClassTypeMarkers(bf, cls);
        sbufPWriteLine(bf, _S"        void* _is_ObjInst;");
        sbufPWriteLine(bf, _S"    };");
        sbufPWriteLine(bf, _S"    ObjClassInfo* _clsinfo;");
        sbufPWriteLine(bf, _S"    atomic(uintptr) _ref;");
        sbufPWriteLine(bf, _S"    atomic(ptr) _weakref;");
        sbufPWriteEOL(bf);
    }

    for (int i = 0; i < saSize(cls->allmembers); i++) {
        Member *m = cls->allmembers.a[i];
        writeClassMember(bf, cls, m);
    }

    strNConcat(&ln, _S"} ", cls->name, _S";");
    sbufPWriteLine(bf, ln);
    if (!cls->mixin) {
        strNConcat(&ln, _S"extern ObjClassInfo ", cls->name, _S"_clsinfo;");
        sbufPWriteLine(bf, ln);
    }
    strNConcat(&ln, _S"#define ", cls->name, _S"(inst) ((", cls->name,
               _S"*)(unused_noeval((inst) && &((inst)->_is_", cls->name, _S")), (inst)))");
    sbufPWriteLine(bf, ln);
    strNConcat(&ln, _S"#define ", cls->name, _S"None ((", cls->name,
               _S"*)NULL)");
    sbufPWriteLine(bf, ln);
    sbufPWriteEOL(bf);

    if (cls->mixin)
        return;

    strNConcat(&ln, _S"typedef struct ", cls->name, _S"_WeakRef {");
    sbufPWriteLine(bf, ln);

    sbufPWriteLine(bf, _S"    union {");
    sbufPWriteLine(bf, _S"        ObjInst* _inst;");
    writeClassWeakRefTypeMarkers(bf, cls);
    sbufPWriteLine(bf, _S"        void* _is_ObjInst_WeakRef;");
    sbufPWriteLine(bf, _S"    };");
    sbufPWriteLine(bf, _S"    atomic(uintptr) _ref;");
    sbufPWriteLine(bf, _S"    RWLock _lock;");

    strNConcat(&ln, _S"} ", cls->name, _S"_WeakRef;");
    sbufPWriteLine(bf, ln);
    strNConcat(&ln, _S"#define ", cls->name, _S"_WeakRef(inst) ((", cls->name,
               _S"_WeakRef*)(unused_noeval((inst) && &((inst)->_is_", cls->name, _S"_WeakRef)), (inst)))");
    sbufPWriteLine(bf, ln);
    sbufPWriteEOL(bf);

    sa_Method unboundDone;
    saInit(&unboundDone, object, 16);
    writeUnbound(bf, cls, cls, &unboundDone);
    saDestroy(&unboundDone);

    for (int i = 0; i < saSize(cls->allmethods); i++) {
        Method *m = cls->allmethods.a[i];
        if (m->internal)
            continue;
        methodCallName(&mname, cls, m->name);
        writeFuncComment(bf, mname, cls, m);
        writeComments(bf, m->comments, 0, true);
        strNConcat(&ln, _S"#define ", mname, _S"(self");
        for (int j = 0; j < saSize(m->params); j++) {
            strNConcat(&ln, ln, _S", ", m->params.a[j]->name);
        }
        strNConcat(&ln, ln, _S") (self)->_->", m->name, _S"(", cls->name, _S"(self)");
        for (int j = 0; j < saSize(m->params); j++) {
            // use cast macro for classes that we know about
            if (isObjectType(m->params.a[j]))
                strNConcat(&ln, ln, _S", ", m->params.a[j]->type, _S"(", m->params.a[j]->name, _S")");
            else
                strNConcat(&ln, ln, _S", ", m->params.a[j]->name);
        }
        strAppend(&ln, _S")");
        sbufPWriteLine(bf, ln);
    }
    sbufPWriteEOL(bf);

    strDestroy(&mname);
    strDestroy(&ln);
}

bool writeHeader(string fname)
{
    string hname = 0;
    pathSetExt(&hname, fname, _S"h");

    FSFile *file = fsOpen(hname, FS_Overwrite);
    if (!file) {
        fprintf(stderr, "Failed to open %s for writing", lazyPlatformPath(hname));
        return false;
    }
    StreamBuffer* bf = sbufCreate(1024);
    if (!sbufFSFileCRegisterPush(bf, file, true))
        return false;
    if (!sbufPRegisterPush(bf, NULL, NULL))
        return false;

    string ln = 0;
    sbufPWriteLine(bf, _S"#pragma once");
    sbufPWriteLine(bf, _S"// This header file is auto-generated!");
    sbufPWriteLine(bf, _S"// Do not make changes to this file or they will be overwritten.");
    sbufPWriteLine(bf, _S"// clang-format off");
    sbufPWriteLine(bf, _S"#include <cx/obj.h>");
    for (int i = 0; i < saSize(includes); i++) {
        strNConcat(&ln, _S"#include ", includes.a[i]);
        sbufPWriteLine(bf, ln);
    }
    sbufPWriteEOL(bf);

    for(int i = 0; i < saSize(fwdclass); i++) {
        writeForwardDecl(bf, fwdclass.a[i]);
        writeForwardWeakRefDecl(bf, fwdclass.a[i]);
    }
    for (int i = 0; i < saSize(structs); i++) {
        writeForwardDecl(bf, structs.a[i]);
    }

    for (int i = 0; i < saSize(classes); i++) {
        if(!classes.a[i]->included) {
            writeForwardDecl(bf, classes.a[i]->name);
            writeForwardWeakRefDecl(bf, classes.a[i]->name);
        }
    }
    for(int i = 0; i < saSize(classes); i++) {
        if(!classes.a[i]->included) {
            writeSArrayDecl(bf, classes.a[i]->name);
            writeSArrayWeakRefDecl(bf, classes.a[i]->name);
        }
    }
    for (int i = 0; i < saSize(artypes); i++) {
        writeComplexArrayDecl(bf, artypes.a[i]);
    }
    if (!strEmpty(cpassthrough)) {
        sbufPWriteEOL(bf);
        sbufPWriteStr(bf, cpassthrough);
    }
    sbufPWriteEOL(bf);

    for (int i = 0; i < saSize(ifaces); i++) {
        if (!ifaces.a[i]->included)
            writeIfDecl(bf, ifaces.a[i]);
    }

    for (int i = 0; i < saSize(classes); i++) {
        if (!classes.a[i]->included)
            writeClassDecl(bf, classes.a[i]);
    }

    strDestroy(&ln);
    strDestroy(&hname);
    sbufPFinish(bf);
    sbufRelease(&bf);
    return true;
}
