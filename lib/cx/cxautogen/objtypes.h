#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>

typedef struct Param Param;
typedef struct Param_WeakRef Param_WeakRef;
typedef struct Method Method;
typedef struct Method_WeakRef Method_WeakRef;
typedef struct Interface Interface;
typedef struct Interface_WeakRef Interface_WeakRef;
typedef struct Member Member;
typedef struct Member_WeakRef Member_WeakRef;
typedef struct Class Class;
typedef struct Class_WeakRef Class_WeakRef;
typedef struct ComplexArrayType ComplexArrayType;
typedef struct ComplexArrayType_WeakRef ComplexArrayType_WeakRef;
saDeclarePtr(Param);
saDeclarePtr(Param_WeakRef);
saDeclarePtr(Method);
saDeclarePtr(Method_WeakRef);
saDeclarePtr(Interface);
saDeclarePtr(Interface_WeakRef);
saDeclarePtr(Member);
saDeclarePtr(Member_WeakRef);
saDeclarePtr(Class);
saDeclarePtr(Class_WeakRef);
saDeclarePtr(ComplexArrayType);
saDeclarePtr(ComplexArrayType_WeakRef);
saDeclareType(sarray_string, sa_string);

typedef struct Method_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    _Ret_valid_ Method* (*clone)(_In_ void* self);
    intptr (*cmp)(_In_ void* self, void* other, uint32 flags);
} Method_ClassIf;
extern Method_ClassIf Method_ClassIf_tmpl;

typedef struct Interface_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    intptr (*cmp)(_In_ void* self, void* other, uint32 flags);
} Interface_ClassIf;
extern Interface_ClassIf Interface_ClassIf_tmpl;

typedef struct Member_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    intptr (*cmp)(_In_ void* self, void* other, uint32 flags);
} Member_ClassIf;
extern Member_ClassIf Member_ClassIf_tmpl;

typedef struct Class_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    intptr (*cmp)(_In_ void* self, void* other, uint32 flags);
} Class_ClassIf;
extern Class_ClassIf Class_ClassIf_tmpl;

typedef struct Param {
    union {
        ObjIface* _;
        void* _is_Param;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    string type;
    string predecr;
    string name;
    string postdecr;
    sa_sarray_string annotations;
} Param;
extern ObjClassInfo Param_clsinfo;
#define Param(inst) ((Param*)(unused_noeval((inst) && &((inst)->_is_Param)), (inst)))
#define ParamNone ((Param*)NULL)

typedef struct Param_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_Param_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} Param_WeakRef;
#define Param_WeakRef(inst) ((Param_WeakRef*)(unused_noeval((inst) && &((inst)->_is_Param_WeakRef)), (inst)))

_objfactory_guaranteed Param* Param_create();
// Param* paramCreate();
#define paramCreate() Param_create()


typedef struct Method {
    union {
        Method_ClassIf* _;
        void* _is_Method;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    Class* srcclass;
    Interface* srcif;
    Class* mixinsrc;
    string srcfile;
    string returntype;
    string predecr;
    string name;
    sa_Param params;
    sa_string comments;
    sa_sarray_string annotations;
    bool isinit;
    bool isdestroy;
    bool isfactory;
    bool canfail;
    bool internal;
    bool unbound;
    bool standalone;
    bool mixin;
} Method;
extern ObjClassInfo Method_clsinfo;
#define Method(inst) ((Method*)(unused_noeval((inst) && &((inst)->_is_Method)), (inst)))
#define MethodNone ((Method*)NULL)

typedef struct Method_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_Method_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} Method_WeakRef;
#define Method_WeakRef(inst) ((Method_WeakRef*)(unused_noeval((inst) && &((inst)->_is_Method_WeakRef)), (inst)))

_objfactory_guaranteed Method* Method_create();
// Method* methodCreate();
#define methodCreate() Method_create()

// Method* methodClone(Method* self);
#define methodClone(self) (self)->_->clone(Method(self))
// intptr methodCmp(Method* self, Method* other, uint32 flags);
#define methodCmp(self, other, flags) (self)->_->cmp(Method(self), other, flags)

typedef struct Interface {
    union {
        Interface_ClassIf* _;
        void* _is_Interface;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    string name;
    Interface* parent;
    sa_Method methods;
    bool included;
    bool processed;
    bool classif;
    sa_Method allmethods;
} Interface;
extern ObjClassInfo Interface_clsinfo;
#define Interface(inst) ((Interface*)(unused_noeval((inst) && &((inst)->_is_Interface)), (inst)))
#define InterfaceNone ((Interface*)NULL)

typedef struct Interface_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_Interface_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} Interface_WeakRef;
#define Interface_WeakRef(inst) ((Interface_WeakRef*)(unused_noeval((inst) && &((inst)->_is_Interface_WeakRef)), (inst)))

_objfactory_guaranteed Interface* Interface_create();
// Interface* interfaceCreate();
#define interfaceCreate() Interface_create()

// intptr interfaceCmp(Interface* self, Interface* other, uint32 flags);
#define interfaceCmp(self, other, flags) (self)->_->cmp(Interface(self), other, flags)

typedef struct Member {
    union {
        Member_ClassIf* _;
        void* _is_Member;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    sa_string fulltype;
    string vartype;
    string predecr;
    string name;
    string postdecr;
    sa_string comments;
    sa_sarray_string annotations;
    Class* mixinsrc;
    string initstr;
    bool init;
    bool destroy;
} Member;
extern ObjClassInfo Member_clsinfo;
#define Member(inst) ((Member*)(unused_noeval((inst) && &((inst)->_is_Member)), (inst)))
#define MemberNone ((Member*)NULL)

typedef struct Member_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_Member_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} Member_WeakRef;
#define Member_WeakRef(inst) ((Member_WeakRef*)(unused_noeval((inst) && &((inst)->_is_Member_WeakRef)), (inst)))

_objfactory_guaranteed Member* Member_create();
// Member* memberCreate();
#define memberCreate() Member_create()

// intptr memberCmp(Member* self, Member* other, uint32 flags);
#define memberCmp(self, other, flags) (self)->_->cmp(Member(self), other, flags)

typedef struct Class {
    union {
        Class_ClassIf* _;
        void* _is_Class;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    string name;
    Class* parent;
    Interface* classif;
    sa_Interface implements;
    sa_Class uses;
    sa_Member members;
    sa_Method methods;
    sa_string overrides;
    sa_sarray_string annotations;
    bool abstract;
    bool mixin;
    bool hasinit;
    bool hasdestroy;
    bool initcanfail;
    bool included;
    bool processed;
    bool hasautoinit;
    bool hasautodtors;
    string methodprefix;
    sa_Member allmembers;        // runtime stuff
    sa_Method allmethods;
} Class;
extern ObjClassInfo Class_clsinfo;
#define Class(inst) ((Class*)(unused_noeval((inst) && &((inst)->_is_Class)), (inst)))
#define ClassNone ((Class*)NULL)

typedef struct Class_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_Class_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} Class_WeakRef;
#define Class_WeakRef(inst) ((Class_WeakRef*)(unused_noeval((inst) && &((inst)->_is_Class_WeakRef)), (inst)))

_objfactory_guaranteed Class* Class_create();
// Class* classCreate();
#define classCreate() Class_create()

// intptr classCmp(Class* self, Class* other, uint32 flags);
#define classCmp(self, other, flags) (self)->_->cmp(Class(self), other, flags)

typedef struct ComplexArrayType {
    union {
        ObjIface* _;
        void* _is_ComplexArrayType;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    string tname;
    string tsubtype;
} ComplexArrayType;
extern ObjClassInfo ComplexArrayType_clsinfo;
#define ComplexArrayType(inst) ((ComplexArrayType*)(unused_noeval((inst) && &((inst)->_is_ComplexArrayType)), (inst)))
#define ComplexArrayTypeNone ((ComplexArrayType*)NULL)

typedef struct ComplexArrayType_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_ComplexArrayType_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} ComplexArrayType_WeakRef;
#define ComplexArrayType_WeakRef(inst) ((ComplexArrayType_WeakRef*)(unused_noeval((inst) && &((inst)->_is_ComplexArrayType_WeakRef)), (inst)))

_objfactory_guaranteed ComplexArrayType* ComplexArrayType_create();
// ComplexArrayType* complexarraytypeCreate();
#define complexarraytypeCreate() ComplexArrayType_create()


