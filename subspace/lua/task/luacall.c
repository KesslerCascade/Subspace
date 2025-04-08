// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "luacall.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include <cx/log.h>

_objfactory_guaranteed LuaCall*
LuaCall__create(LuaThread* lt, _In_opt_ strref funcname, bool mustexist, int maxresults, int nargs,
                stvar args[])
{
    LuaCall* self;
    self = objInstCreate(LuaCall);

    self->lt         = lt;
    self->mustexist  = mustexist;
    self->maxresults = maxresults;
    strCopy(&self->funcname, funcname);

    saInit(&self->args, stvar, nargs);
    for (int i = 0; i < nargs; i++) {
        saPush(&self->args, stvar, args[i]);
    }

    self->name = _S"LuaCall";
    objInstInit(self);
    return self;
}

_objinit_guaranteed bool LuaCall_init(_In_ LuaCall* self)
{
    // Autogen begins -----
    saInit(&self->results, stvar, 1);
    return true;
    // Autogen ends -------
}

int LuaCall_pushargs(lua_State* L, sa_stvar args)
{
    int nargs = 0;

    for (int i = 0; i < saSize(args); i++) {
        switch (stGetId(args.a[i].type)) {
        case STypeId_int32:
            lua_pushinteger(L, args.a[i].data.st_int32);
            nargs++;
            break;
        case STypeId_uint32:
            lua_pushinteger(L, args.a[i].data.st_uint32);
            nargs++;
            break;
        case STypeId_int64:
            lua_pushinteger(L, (lua_Integer)args.a[i].data.st_int64);
            nargs++;
            break;
        case STypeId_uint64:
            // technically can't do this since lua_Integer is long long
            lua_pushinteger(L, (lua_Integer)args.a[i].data.st_uint64);
            nargs++;
            break;
        case STypeId_float32:
            lua_pushnumber(L, args.a[i].data.st_float32);
            nargs++;
            break;
        case STypeId_float64:
            lua_pushnumber(L, (lua_Number)args.a[i].data.st_float64);
            nargs++;
            break;
        case STypeId_string:
            lua_pushlstring(L, strC(args.a[i].data.st_string), strLen(args.a[i].data.st_string));
            nargs++;
            break;
        case STypeId_object:
            // if (!LuaCall_pushobject(L, args.a[i].data.st_object))
            lua_pushnil(L);
            nargs++;
            break;
        case STypeId_none:
            // none type should only be used in vararg lists with no other args
            break;
        default:
            // type that we don't handle yet, push a nill so the args don't shift
            lua_pushnil(L);
            nargs++;
        }
    }

    return nargs;
}

int LuaCall_popresults(lua_State* L, int maxresults, sa_stvar* results)
{
    int ntop = lua_gettop(L);
    if (maxresults != -1)
        ntop = clamphigh(ntop, maxresults);

    for (int i = 1; i <= ntop; i++) {
        if (lua_isinteger(L, i)) {
            int64 r = lua_tointeger(L, i);
            saPush(results, stvar, stvar(int64, r));
        } else if (lua_isnumber(L, i)) {
            double r = lua_tonumber(L, i);
            saPush(results, stvar, stvar(float64, r));
        } else if (lua_isstring(L, i)) {
            const char* r = lua_tostring(L, i);
            saPush(results, stvar, stvar(string, (string)r));
        } else {
            // something we can't handle, so leave a gap
            saPush(results, stvar, stvNone);
        }
    }

    lua_pop(L, ntop);
    return saSize(*results);
}

bool LuaCall_exec(lua_State* L, _In_opt_ strref funcname, bool mustexist, sa_stvar args,
                  int maxresults, sa_stvar* results)
{
    bool ret = false;

    // if we're getting variable results, need to start with a clean stack
    if (maxresults == -1)
        lua_settop(L, 0);

    lua_getglobal(L, strC(funcname));
    if (lua_isnil(L, -1)) {
        // didn't find any global with that name
        lua_pop(L, 1);
        if (mustexist) {
            logFmt(Error, _S"luacall: Function ${string} did not exist", stvar(strref, funcname));
        } else {
            ret = true;
        }
        goto out;
    }

    int nargs = LuaCall_pushargs(L, args);
    ret       = (lua_pcall(L, nargs, (maxresults == -1) ? LUA_MULTRET : maxresults, 0) == LUA_OK);
    if (ret) {
        LuaCall_popresults(L, maxresults, results);
    } else {
        logFmt(Error,
               _S"luacall: Execution error: ${string}",
               stvar(string, (string)lua_tostring(L, -1)));
        lua_pop(L, 1);   // remove error
    }

out:
    return ret;
}

uint32 LuaCall_run(_In_ LuaCall* self, _In_ TaskQueue* tq, _In_ TQWorker* worker,
                   _Inout_ TaskControl* tcon)
{
    if (!self->lt->L)
        return TASK_Result_Failure;

    return LuaCall_exec(self->lt->L,
                        self->funcname,
                        self->mustexist,
                        self->args,
                        self->maxresults,
                        &self->results) ?
        TASK_Result_Success :
        TASK_Result_Failure;
}

void LuaCall_destroy(_In_ LuaCall* self)
{
    // Autogen begins -----
    strDestroy(&self->funcname);
    saDestroy(&self->args);
    saDestroy(&self->results);
    // Autogen ends -------
}

_objfactory_guaranteed LuaCallbackDispatch*
LuaCallbackDispatch_create(LuaThread* lt, LuaFuncRef* funcref, int maxresults, int nargs,
                           stvar args[])
{
    LuaCallbackDispatch* self;
    self = objInstCreate(LuaCallbackDispatch);

    self->lt         = lt;
    self->func       = objAcquire(funcref);
    self->maxresults = maxresults;

    saInit(&self->args, stvar, nargs);
    for (int i = 0; i < nargs; i++) {
        saPush(&self->args, stvar, args[i]);
    }

    self->name = _S"LuaCallbackDispatch";
    objInstInit(self);
    return self;
}

extern bool LuaCall_exec(lua_State* L, _In_opt_ strref funcname, bool mustexist, sa_stvar args,
                         int maxresults, sa_stvar* results);   // parent
#define parent_exec(L, funcname, mustexist, args, maxresults, results) \
    LuaCall_exec((LuaCall*)(self), L, funcname, mustexist, args, maxresults, results)
bool LuaCallbackDispatch_exec(lua_State* L, int ref, sa_stvar args, int maxresults,
                              sa_stvar* results)
{
    bool ret = false;

    // if we're getting variable results, need to start with a clean stack
    if (maxresults == -1)
        lua_settop(L, 0);

    // get function from reference
    lua_rawgeti(L, LUA_REGISTRYINDEX, ref);

    int nargs = LuaCall_pushargs(L, args);
    ret       = (lua_pcall(L, nargs, (maxresults == -1) ? LUA_MULTRET : maxresults, 0) == LUA_OK);
    if (ret) {
        LuaCall_popresults(L, maxresults, results);
    } else {
        logFmt(Error,
               _S"luacall: Execution error: ${string}",
               stvar(string, (string)lua_tostring(L, -1)));
        lua_pop(L, 1);   // remove error
    }

    return ret;
}

extern uint32 LuaCall_run(_In_ LuaCall* self, _In_ TaskQueue* tq, _In_ TQWorker* worker,
                          _Inout_ TaskControl* tcon);   // parent
#define parent_run(tq, worker, tcon) LuaCall_run((LuaCall*)(self), tq, worker, tcon)
uint32 LuaCallbackDispatch_run(_In_ LuaCallbackDispatch* self, _In_ TaskQueue* tq,
                               _In_ TQWorker* worker, _Inout_ TaskControl* tcon)
{
    if (!self->lt->L)
        return TASK_Result_Failure;

    return LuaCallbackDispatch_exec(self->lt->L,
                                    self->func->ref,
                                    self->args,
                                    self->maxresults,
                                    &self->results) ?
        TASK_Result_Success :
        TASK_Result_Failure;
}

void LuaCallbackDispatch_destroy(_In_ LuaCallbackDispatch* self)
{
    // Autogen begins -----
    objRelease(&self->func);
    // Autogen ends -------
}

_objfactory_check LuaFuncRef* LuaFuncRef_create(LuaThread* lt)
{
    int ref;

    ref = luaL_ref(lt->L, LUA_REGISTRYINDEX);
    if (!ref)
        return NULL;

    LuaFuncRef* self;
    self = objInstCreate(LuaFuncRef);

    self->lt  = lt;
    self->ref = ref;

    if (!objInstInit(self)) {
        objRelease(&self);
        return NULL;
    }

    return self;
}

void LuaFuncRef_destroy(_In_ LuaFuncRef* self)
{
    // This object can be destroyed by any thread that holds a reference to it.
    // Dispatch the actual reference cleanup to the lua thread.
    if (self->lt->L) {
        LuaFuncRefCleanup* task = luafuncrefcleanupCreate(self->lt, self->ref);
        luaRun(self->lt, task);
    }
}

_objfactory_guaranteed LuaFuncRefCleanup* LuaFuncRefCleanup_create(LuaThread* lt, int ref)
{
    LuaFuncRefCleanup* self;
    self = objInstCreate(LuaFuncRefCleanup);

    self->lt  = lt;
    self->ref = ref;

    self->name = _S"LuaFuncRefCleanup";
    objInstInit(self);
    return self;
}

uint32 LuaFuncRefCleanup_run(_In_ LuaFuncRefCleanup* self, _In_ TaskQueue* tq,
                             _In_ TQWorker* worker, _Inout_ TaskControl* tcon)
{
    if (!self->lt->L)
        return TASK_Result_Failure;

    luaL_unref(self->lt->L, LUA_REGISTRYINDEX, self->ref);
    return TASK_Result_Success;
}

bool luaCallback(stvlist* cvars, stvlist* args)
{
    LuaFuncRef* func = stvlNextObj(cvars, LuaFuncRef);
    if (!func)
        return false;

    // create a lua task to dispatch the call cross-thread
    LuaCallbackDispatch*
        task = luacallbackdispatchCreate(func->lt, func, 0, args->count, args->vars);
    luaRun(func->lt, task);

    // don't wait for the lua function to return, assume true
    return true;
}

// Autogen begins -----
#include "luacall.auto.inc"
// Autogen ends -------
