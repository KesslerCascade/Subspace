// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "objtypes.h"
// clang-format on
// ==================== Auto-generated section ends ======================

void Param_destroy(_In_ Param* self)
{
    // Autogen begins -----
    strDestroy(&self->type);
    strDestroy(&self->predecr);
    strDestroy(&self->name);
    strDestroy(&self->postdecr);
    saDestroy(&self->annotations);
    // Autogen ends -------
}

_objinit_guaranteed bool Method_init(_In_ Method* self)
{
    // Autogen begins -----
    saInit(&self->params, object, 1);
    return true;
    // Autogen ends -------
}

intptr Method_cmp(_In_ Method* self, Method* other, uint32 flags)
{
    // Uncomment unless this function can compare across different object classes
    devAssert(self->_clsinfo == other->_clsinfo);

    return strCmp(self->name, other->name);
}

void Method_destroy(_In_ Method* self)
{
    // Autogen begins -----
    strDestroy(&self->srcfile);
    strDestroy(&self->returntype);
    strDestroy(&self->predecr);
    strDestroy(&self->name);
    saDestroy(&self->params);
    saDestroy(&self->comments);
    saDestroy(&self->annotations);
    // Autogen ends -------
}

_objinit_guaranteed bool Interface_init(_In_ Interface* self)
{
    // Autogen begins -----
    saInit(&self->methods, object, 1);
    saInit(&self->allmethods, object, 1);
    return true;
    // Autogen ends -------
}

intptr Interface_cmp(_In_ Interface* self, Interface* other, uint32 flags)
{
    // Uncomment unless this function can compare across different object classes
    devAssert(self->_clsinfo == other->_clsinfo);

    return strCmp(self->name, other->name);
}

void Interface_destroy(_In_ Interface* self)
{
    // Autogen begins -----
    strDestroy(&self->name);
    saDestroy(&self->methods);
    saDestroy(&self->allmethods);
    // Autogen ends -------
}

intptr Member_cmp(_In_ Member* self, Member* other, uint32 flags)
{
    // Uncomment unless this function can compare across different object classes
    devAssert(self->_clsinfo == other->_clsinfo);

    return strCmp(self->name, other->name);
}

void Member_destroy(_In_ Member* self)
{
    // Autogen begins -----
    saDestroy(&self->fulltype);
    strDestroy(&self->vartype);
    strDestroy(&self->predecr);
    strDestroy(&self->name);
    strDestroy(&self->postdecr);
    saDestroy(&self->comments);
    saDestroy(&self->annotations);
    strDestroy(&self->initstr);
    // Autogen ends -------
}

_objinit_guaranteed bool Class_init(_In_ Class* self)
{
    // Autogen begins -----
    saInit(&self->implements, object, 1);
    saInit(&self->uses, object, 1);
    saInit(&self->members, object, 1);
    saInit(&self->methods, object, 1);
    saInit(&self->overrides, string, 1);
    saInit(&self->allmembers, object, 1);
    saInit(&self->allmethods, object, 1);
    return true;
    // Autogen ends -------
}

intptr Class_cmp(_In_ Class* self, Class* other, uint32 flags)
{
    // Uncomment unless this function can compare across different object classes
    devAssert(self->_clsinfo == other->_clsinfo);

    return strCmp(self->name, other->name);
}

void Class_destroy(_In_ Class* self)
{
    // Autogen begins -----
    strDestroy(&self->name);
    objRelease(&self->classif);
    saDestroy(&self->implements);
    saDestroy(&self->uses);
    saDestroy(&self->members);
    saDestroy(&self->methods);
    saDestroy(&self->overrides);
    saDestroy(&self->annotations);
    strDestroy(&self->methodprefix);
    saDestroy(&self->allmembers);
    saDestroy(&self->allmethods);
    // Autogen ends -------
}

_objfactory_guaranteed Param* Param_create()
{
    Param *ret;
    ret = objInstCreate(Param);
    objInstInit(ret);
    return ret;
}

_objfactory_guaranteed Method* Method_create()
{
    Method *ret;
    ret = objInstCreate(Method);
    objInstInit(ret);
    return ret;
}

_objfactory_guaranteed Interface* Interface_create()
{
    Interface *ret;
    ret = objInstCreate(Interface);
    objInstInit(ret);
    return ret;
}

_objfactory_guaranteed Member* Member_create()
{
    Member *ret;
    ret = objInstCreate(Member);
    objInstInit(ret);
    return ret;
}

_objfactory_guaranteed Class* Class_create()
{
    Class *ret;
    ret = objInstCreate(Class);
    objInstInit(ret);
    return ret;
}

_Ret_valid_ Method* Method_clone(_In_ Method* self)
{
    Method *ret = methodCreate();
    ret->srcclass = self->srcclass;
    ret->srcif = self->srcif;
    ret->mixinsrc = self->mixinsrc;
    strDup(&ret->srcfile, self->srcfile);
    strDup(&ret->returntype, self->returntype);
    strDup(&ret->predecr, self->predecr);
    strDup(&ret->name, self->name);
    saClone(&ret->params, self->params);
    if (self->comments.a)
        saClone(&ret->comments, self->comments);
    if (self->annotations.a)
        saClone(&ret->annotations, self->annotations);

    ret->isinit = self->isinit;
    ret->isdestroy = self->isdestroy;
    ret->isfactory = self->isfactory;
    ret->internal = self->internal;
    ret->canfail = self->canfail;
    ret->unbound = self->unbound;
    ret->mixin = self->mixin;
    return ret;
}

_objfactory_guaranteed ComplexArrayType* ComplexArrayType_create()
{
    ComplexArrayType *self;
    self = objInstCreate(ComplexArrayType);

    objInstInit(self);

    return self;
}

void ComplexArrayType_destroy(_In_ ComplexArrayType* self)
{
    // Autogen begins -----
    strDestroy(&self->tname);
    strDestroy(&self->tsubtype);
    // Autogen ends -------
}

// Autogen begins -----
#include "objtypes.auto.inc"
// Autogen ends -------
