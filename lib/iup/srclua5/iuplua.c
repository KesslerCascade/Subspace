/** \file
* \brief IUP binding for Lua 5.
*
* See Copyright Notice in "iup.h"
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "iup.h"
#include "iup_str.h"
#include "iup_object.h"

#include <lua.h>
#include <lauxlib.h>

#include "iuplua.h"
#include "il.h"


static int CopyUserData2String(lua_State *L)
{
  void* udata = lua_touserdata(L, 1);
  size_t size = luaL_checkinteger(L, 2);
  char* str = malloc(size);
  memcpy(str, udata, size);  /* buffer must contain the terminator */
  lua_pushlstring(L, str, size - 1); /* len */
  free(str);
  return 1;
}

static int CopyString2UserData(lua_State *L)
{
  size_t len;
  const char* str = luaL_checklstring(L, 1, &len);
  void* udata = lua_touserdata(L, 2);
  size_t size = luaL_checkinteger(L, 3);
  if (len >= size) len = size-1;
  memcpy(udata, str, len);
  ((char*)udata)[len] = 0;
  return 0;
}

static int StringCompare(lua_State *L)
{
  const char* str1 = luaL_optstring(L, 1, NULL);
  const char* str2 = luaL_optstring(L, 2, NULL);
  int casesensitive = (int)luaL_optinteger(L, 3, 0);
  int lexicographic = (int)luaL_optinteger(L, 4, 1);
  int ret = IupStringCompare(str1, str2, casesensitive, lexicographic);
  lua_pushinteger(L, ret);
  return 1;
}

static int StringChangeCase(lua_State *L)
{
  int utf8mode = IupGetInt(NULL, "UTF8MODE");
  const char* str = luaL_checkstring(L, 1);
  char* dst_str = iupStrDup(str);
  const char* flag = luaL_checkstring(L, 2);
  int case_flag = IUP_CASE_UPPER;
  if (iupStrEqualNoCase(flag, "UPPER"))
    case_flag = IUP_CASE_UPPER;
  else if (iupStrEqualNoCase(flag, "LOWER"))
    case_flag = IUP_CASE_LOWER;
  else if (iupStrEqualNoCase(flag, "TOGGLE"))
    case_flag = IUP_CASE_TOGGLE;
  else if (iupStrEqualNoCase(flag, "TITLE"))
    case_flag = IUP_CASE_TITLE;
  iupStrChangeCase(dst_str, str, case_flag, utf8mode);
  lua_pushstring(L, dst_str);
  free(dst_str);
  return 1;
}

IUP_SDK_API char* iupStrConvertToUTF8(const char* str, int len, char* utf8_buffer, int *utf8_buffer_max, int utf8mode);
IUP_SDK_API void iupStrUtf8ToAnsi(char* dstr, const char* sstr);
IUP_SDK_API void iupStrAnsiToUtf8(char* dstr, const char* sstr);

static int StringConvertToUTF8(lua_State *L)
{
  int utf8mode = IupGetInt(NULL, "UTF8MODE");
  const char* str = luaL_checkstring(L, 1);
  int len = (int)strlen(str);
  int utf8_buffer_max = 2 * len;
  char* utf8_buffer = malloc(utf8_buffer_max);
  utf8_buffer = iupStrConvertToUTF8(str, len, utf8_buffer, &utf8_buffer_max, utf8mode);
  lua_pushstring(L, utf8_buffer);
  free(utf8_buffer);
  return 1;
}

static int StringUtf8ToAnsi(lua_State *L)
{
  const char* str = luaL_checkstring(L, 1);
  int len = (int)strlen(str);
  char* ansi_buffer = malloc(len+1);
  iupStrUtf8ToAnsi(ansi_buffer, str);
  lua_pushstring(L, ansi_buffer);
  free(ansi_buffer);
  return 1;
}

static int StringAnsiToUtf8(lua_State *L)
{
  const char* str = luaL_checkstring(L, 1);
  int len = (int)strlen(str);
  char* utf8_buffer = malloc(2 * len);
  iupStrAnsiToUtf8(utf8_buffer, str);
  lua_pushstring(L, utf8_buffer);
  free(utf8_buffer);
  return 1;
}


/*******************************  Error Handling *****************************************/


static int show_error_continue_action(Ihandle* ih)
{
  (void)ih;
  return IUP_CLOSE;
}

static int show_error_exit_action(Ihandle* ih)
{
  if (ih) /* just to avoid a warning at return */
    exit(EXIT_FAILURE);
  return IUP_DEFAULT;
}

static int show_error_copy_action(Ihandle* ih)
{
  Ihandle* multi_text = IupGetDialogChild(ih, "TEXT");
  IupSetAttribute(multi_text, "SELECTION", "ALL");
  IupSetAttribute(multi_text, "CLIPBOARD", "COPY");
  return IUP_DEFAULT;
}

IUPLUA_API void iuplua_show_error_message(const char *pname, const char* msg)
{
  Ihandle *multi_text, *lbl, *copy, *button, *box, *dlg, *abort, *buttonbox;
  char* value = IupGetGlobal("LUA_ERROR_LABEL");

  if (!pname) pname = "_@IUP_ERROR";

  lbl = IupLabel("_@IUP_LUAERROR");
  IupSetAttribute(lbl, "EXPAND", "HORIZONTAL");
  if (value) IupSetStrAttribute(lbl, "TITLE", value);

  copy = IupButton("_@IUP_COPY", NULL);
  IupSetStrAttribute(copy, "TIP", "_@IUP_COPYTOCLIPBOARD");
  IupSetStrAttribute(copy, "PADDING", IupGetGlobal("DEFAULTBUTTONPADDING"));
  IupSetCallback(copy, "ACTION", show_error_copy_action);

  button = IupButton("_@IUP_CONTINUE", NULL);
  IupSetStrAttribute(button, "PADDING", IupGetGlobal("DEFAULTBUTTONPADDING"));
  IupSetCallback(button, "ACTION", show_error_continue_action);

  abort = IupButton("_@IUP_EXIT", NULL);
  IupSetStrAttribute(abort, "PADDING", IupGetGlobal("DEFAULTBUTTONPADDING"));
  IupSetCallback(abort, "ACTION", show_error_exit_action);

  multi_text = IupMultiLine(NULL);
  IupSetAttribute(multi_text, "EXPAND", "YES");
  IupSetAttribute(multi_text, "READONLY", "YES");
  IupSetAttribute(multi_text, "FONT", "Courier, 12");
  IupSetAttribute(multi_text, "VISIBLELINES", "10");
  IupSetAttribute(multi_text, "VISIBLECOLUMNS", "50");
  IupSetAttribute(multi_text, "NAME", "TEXT");
  IupSetStrAttribute(multi_text, "VALUE", msg);

  buttonbox = IupHbox(copy, button, abort, NULL);
  IupSetAttribute(buttonbox, "GAP", "50");
  IupSetAttribute(IupNormalizer(button, abort, NULL), "NORMALIZE", "HORIZONTAL");

  box = IupVbox(lbl, multi_text, buttonbox, NULL);
  IupSetAttribute(box, "ALIGNMENT", "ACENTER");
  IupSetAttribute(box, "NMARGIN", "10x10");
  IupSetAttribute(box, "GAP", "10");

  dlg = IupDialog(box);

  IupSetStrAttribute(dlg, "TITLE", pname);
  IupSetAttribute(dlg, "MINBOX", "NO");
  IupSetAttribute(dlg, "MAXBOX", "NO");
  IupSetAttribute(dlg, "PARENTDIALOG", IupGetGlobal("PARENTDIALOG"));
  IupSetAttribute(dlg, "ICON", IupGetGlobal("ICON"));
  IupSetAttributeHandle(dlg, "DEFAULTESC", button);
  IupSetAttributeHandle(dlg, "DEFAULTENTER", button);
  IupSetAttributeHandle(dlg, "STARTFOCUS", button);

  IupPopup(dlg, IUP_CENTERPARENT, IUP_CENTERPARENT);

  IupDestroy(dlg);
}

static int il_error_message(lua_State *L)
{
  const char* msg = lua_tostring(L, 1);
  iuplua_show_error_message(NULL, msg);
  return 0;
}

static void show_error(lua_State *L, const char *msg)
{
  iuplua_push_name(L, "_ERRORMESSAGE");

  if (lua_isnil(L, -1))
  {
    /* Panic mode */
    iuplua_show_error_message(NULL, msg);
    return;
  }

  lua_pushstring(L, msg);
  lua_call(L, 1, 0);  /* iup._ERRORMESSAGE(msg) */
}


/**********************************************************/
/* report, traceback and docall were adapted from "lua.c" */

static int report (lua_State *L, int status)
{
  /* if there was an erro, and there is an error message on the stack */
  if (status != LUA_OK && !lua_isnil(L, -1)) 
  {
    const char *msg = lua_tostring(L, -1);
    if (msg == NULL) msg = "(error with no message)";
    show_error(L, msg);
    lua_pop(L, 1);  /* remove message */
  }
  return status;
}

#if LUA_VERSION_NUM	> 501
#if LUA_VERSION_NUM	> 502
static int traceback (lua_State *L) {
  const char *msg = lua_tostring(L, 1);
  if (msg == NULL) {  /* is error object not a string? */
    if (luaL_callmeta(L, 1, "__tostring") &&  /* does it have a metamethod */
        lua_type(L, -1) == LUA_TSTRING)  /* that produces a string? */
        return 1;  /* that is the message */
    else
      msg = lua_pushfstring(L, "(error object is a %s value)",
      luaL_typename(L, 1));
  }
  luaL_traceback(L, L, msg, 1);  /* append a standard traceback */
  return 1;  /* return the traceback */
}
#else
static int traceback(lua_State *L) {
  const char *msg = lua_tostring(L, 1);
  if (msg)
    luaL_traceback(L, L, msg, 1);
  else if (!lua_isnoneornil(L, 1)) {  /* is there an error object? */
    if (!luaL_callmeta(L, 1, "__tostring"))  /* try its 'tostring' metamethod */
      lua_pushliteral(L, "(no error message)");
  }
  return 1;
}
#endif
#else
static int traceback(lua_State *L) {
  if (!lua_isstring(L, 1))  /* 'message' not a string? */
    return 1;  /* keep it intact */
  lua_getfield(L, LUA_GLOBALSINDEX, "debug");
  if (!lua_istable(L, -1)) {
    lua_pop(L, 1);
    return 1;
  }
  lua_getfield(L, -1, "traceback");
  if (!lua_isfunction(L, -1)) {
    lua_pop(L, 2);
    return 1;
  }
  lua_pushvalue(L, 1);  /* pass error message */
  lua_pushinteger(L, 2);  /* skip this function and traceback */
  lua_call(L, 2, 1);  /* call debug.traceback */
  return 1;
}
#endif

static void push_tracefunc(lua_State *L)
{
  iuplua_get_env(L);
  lua_pushstring(L, "_TRACEBACK");
  lua_gettable(L, -2);
  lua_remove(L, -2);  /* remove global table from stack */

  if (lua_isnil(L, -1))
  {
    lua_pop(L, 1);
    lua_pushcfunction(L, traceback);  /* push traceback function */
  }
}

static int docall (lua_State *L, int narg, int nret) 
{
  int status;
  int base = lua_gettop(L) - narg;  /* function index */
  push_tracefunc(L);  /* push traceback function */
  lua_insert(L, base);  /* put it under chunk and args */
  status = lua_pcall(L, narg, nret, base);
  lua_remove(L, base);  /* remove traceback function */
  /* force a complete garbage collection in case of errors */
  if (status != 0) lua_gc(L, LUA_GCCOLLECT, 0);
  return status;
}

             /*************************************/
             /*              Utilities            */

IUPLUA_SDK_API void iuplua_push_name(lua_State *L, const char* name)
{
  /* push iup.name in stack */
  iuplua_get_env(L);
  lua_pushstring(L, name);
  lua_gettable(L, -2);
  lua_remove(L, -2);  /* remove global table from stack */
}

IUPLUA_API int iuplua_dofile(lua_State *L, const char *filename)
{
  int status = luaL_loadfile(L, filename);
  if (status == LUA_OK)
    status = docall(L, 0, LUA_MULTRET);
  else if (status == LUA_ERRFILE)
  {
    char *dir = getenv("IUPLUA_DIR");
    if (dir)
    {
      char* full_name = iupStrFileMakeFileName(dir, filename);
      if (full_name)
      {
        /* remove the error string from the stack, and try again */
        lua_remove(L, -1);

        status = luaL_loadfile(L, full_name);
        free(full_name);
        if (status == LUA_OK)
          status = docall(L, 0, LUA_MULTRET);
      }
    }
  }
  return report(L, status);
}

/*
IUPLUA_API int iuplua_dostring(lua_State *L, const char *s, const char *chunk_name)
{
  int status = luaL_loadbuffer(L, s, strlen(s), chunk_name);
  if (status == LUA_OK)
    status = docall(L, 0, LUA_MULTRET);
  return report(L, status);
}
*/

IUPLUA_API int iuplua_dobuffer(lua_State *L, const char *s, int len, const char *chunk_name)
{
  int status = luaL_loadbuffer(L, s, len, chunk_name);
  if (status == LUA_OK)
    status = docall(L, 0, LUA_MULTRET);
  return report(L, status);
}

/*
static int il_dofile(lua_State *L)
{
  int old_top = lua_gettop(L);
  const char* filename = luaL_checkstring(L, 1);
  int status = iuplua_dofile(L, filename);
  if (status == LUA_OK)
  {
    int top = lua_gettop(L);
    return top - old_top;
  }
  else
    return 0;
}

static int il_dostring(lua_State *L)
{
  int old_top = lua_gettop(L);
  size_t size;
  const char* str = luaL_checklstring(L, 1, &size);
  int status = iuplua_dobuffer(L, str, (int)size, "=iup.dostring");
  if (status == LUA_OK)
  {
    int top = lua_gettop(L);
    return top - old_top;
  }
  else
    return 0;
}
*/

IUPLUA_SDK_API Ihandle *iuplua_checkihandleornil(lua_State *L, int pos)
{
  if (lua_isnoneornil(L, pos))
    return NULL;
  else
    return iuplua_checkihandle(L, pos);
}

IUPLUA_API int iuplua_isihandle(lua_State *L, int pos)
{
  int ret = 0;
  if (lua_getmetatable(L, pos))   /* t2 = metatable(stack(pos)) */
  {
    /* TODO: luaL_getmetatable(L, "iupHandle"); */
    lua_pushstring(L, "iupHandle");
    lua_gettable(L, LUA_REGISTRYINDEX);  /* t = registry["iupHandle"] */

    if (lua_rawequal(L, -2, -1))   /* check (t2==t)? */
      ret = 1;

    lua_pop(L, 2);   /* Pop registry["iuphandle"] and the metatable */
  }

  return ret;
}

IUPLUA_API Ihandle* iuplua_checkihandle(lua_State *L, int pos)
{
  Ihandle* *ih = (Ihandle**)luaL_checkudata(L, pos, "iupHandle");

  if (!(*ih))
    luaL_argerror(L, pos, "destroyed iupHandle");

  if (!iupObjectCheck(*ih))
    luaL_argerror(L, pos, "invalid Lua object, destroyed iupHandle in C but not in Lua");

  return *ih;
}

#if 0
Ihandle* iuplua_checkihandle_OLD(lua_State *L, int pos)
{
  Ihandle* ih = NULL;

  if (lua_getmetatable(L, pos))   /* t2 = metatable(stack(pos)) */
  {
    /* TODO: luaL_getmetatable(L, "iupHandle"); */
    lua_pushstring(L, "iupHandle");
    lua_gettable(L, LUA_REGISTRYINDEX);  /* t = registry["iupHandle"] */

    if (lua_rawequal(L, -2, -1))   /* check (t2==t)? */
      ih = *(Ihandle**)lua_touserdata(L, pos);

    lua_pop(L, 2);   /* Pop registry["iupHandle"] and the metatable */
  }

  if (!ih)
  {
    const char *msg = lua_pushfstring(L, "iupHandle expected, got %s", luaL_typename(L, pos));
    luaL_argerror(L, pos, msg);
  }

  return ih;
}
#endif

IUPLUA_SDK_API void iuplua_pushihandle_raw(lua_State *L, Ihandle *ih)
{
  if (ih) 
  {
    Ihandle** new_pointer = (Ihandle**)lua_newuserdata(L, sizeof(Ihandle*));
    *new_pointer = ih;
  } 
  else 
    lua_pushnil(L);
}

IUPLUA_API void iuplua_pushihandle(lua_State *L, Ihandle *ih)
{
  if (ih) 
  {
    char* sref = IupGetAttribute(ih, "_IUPLUA_WIDGET_TABLE_REF");
    if (!sref)
    {
      /* was not created in Lua */

      iuplua_plugstate(L, ih);

      /* push iup.RegisterHandle */
      iuplua_push_name(L, "RegisterHandle");
      iuplua_pushihandle_raw(L, ih);
      lua_pushstring(L, IupGetClassName(ih));
      lua_call(L, 2, 1);  /* iup.RegisterHandle(ih, type) */
    }
    else
    {
      /* already created in Lua */
      iuplua_pushihandle_raw(L, ih);

      /* TODO: luaL_getmetatable(L, "iupHandle"); */
      lua_pushstring(L, "iupHandle");
      lua_gettable(L, LUA_REGISTRYINDEX);  /* t = registry["iupHandle"] */

      lua_setmetatable(L, -2);    /* metatable(ih) = t */
    }
  } 
  else 
    lua_pushnil(L);
}

static int ldestroy_cb(Ihandle* ih)
{
  /* called from IupDestroy. */
  lua_State *L = iuplua_getstate(ih);

  char* sref = IupGetAttribute(ih, "_IUPLUA_WIDGET_TABLE_REF");
  if (sref)
  {
    int ref = atoi(sref);

    /* removes the Ihandle* reference from the lua object */
    /* widget.ihandle = nil */
    lua_rawgeti(L, LUA_REGISTRYINDEX, ref);  /* push object */
    lua_pushstring(L, "ihandle");
    lua_pushnil(L);
    lua_settable(L, -3);
    lua_pop(L, 1);

    /* removes the association of the Ihandle* with the lua object */
    luaL_unref(L, LUA_REGISTRYINDEX, ref);  /* this is the complement of SetWidget */
    IupSetAttribute(ih, "_IUPLUA_WIDGET_TABLE_REF", NULL);
  }

  sref = IupGetAttribute(ih, "_IUPLUA_STATE_THREAD");
  if (sref)
  {
    int ref = atoi(sref);
    luaL_unref(L, LUA_REGISTRYINDEX, ref);
    IupSetAttribute(ih, "_IUPLUA_STATE_THREAD", NULL);
  }

  IupSetAttribute(ih, "_IUPLUA_STATE_CONTEXT", NULL);
  IupSetCallback(ih, "LDESTROY_CB", NULL);

  return IUP_DEFAULT;
}

IUPLUA_SDK_API char** iuplua_checkstring_array(lua_State *L, int pos, int n)
{
  int i;
  char **v;

  luaL_checktype(L, pos, LUA_TTABLE);
  if (n == -1) 
    n = iuplua_getn(L, pos);
  else if (n != iuplua_getn(L, pos))
    luaL_argerror(L, pos, "Invalid number of elements (n!=count).");
  if (n<=0) luaL_argerror(L, pos, "Invalid number of elements (n<=0).");

  v = (char **) malloc (n*sizeof(char *));
  for(i=1; i<=n; i++)
  {
    lua_pushinteger(L,i);
    lua_gettable(L,pos);
    v[i-1] = (char*)lua_tostring(L, -1);
    lua_pop(L, 1);
  }
  return v;
}

IUPLUA_SDK_API int* iuplua_checkint_array(lua_State *L, int pos, int n)
{
  int i;
  int *v;

  luaL_checktype(L, pos, LUA_TTABLE);
  if (n == -1) 
    n = iuplua_getn(L, pos);
  else if (n != iuplua_getn(L, pos))
    luaL_argerror(L, pos, "Invalid number of elements (n!=count).");
  if (n<=0) luaL_argerror(L, pos, "Invalid number of elements (n<=0).");

  v = (int *) malloc (n*sizeof(int));
  for(i=1; i<=n; i++)
  {
    lua_pushinteger(L,i);
    lua_gettable(L,pos);
    v[i - 1] = (int)lua_tointeger(L, -1);
    lua_pop(L, 1);
  }
  return v;
}

IUPLUA_SDK_API float* iuplua_checkfloat_array(lua_State *L, int pos, int n)
{
  int i;
  float* v;

  luaL_checktype(L, pos, LUA_TTABLE);
  if (n == -1)
    n = iuplua_getn(L, pos);
  else if (n != iuplua_getn(L, pos))
    luaL_argerror(L, pos, "Invalid number of elements (n!=count).");
  if (n<=0) luaL_argerror(L, pos, "Invalid number of elements (n<=0).");
    
  v = (float *) malloc (n*sizeof(float));
  for(i=1; i<=n; i++)
  {
    lua_pushinteger(L,i);
    lua_gettable(L,pos);
    v[i-1] = (float)lua_tonumber(L, -1);
    lua_pop(L, 1);
  }
  return v;
}

IUPLUA_SDK_API double* iuplua_checkdouble_array(lua_State *L, int pos, int n)
{
  int i;
  double* v;

  luaL_checktype(L, pos, LUA_TTABLE);
  if (n == -1)
    n = iuplua_getn(L, pos);
  else if (n != iuplua_getn(L, pos))
    luaL_argerror(L, pos, "Invalid number of elements (n!=count).");
  if (n <= 0) luaL_argerror(L, pos, "Invalid number of elements (n<=0).");

  v = (double *)malloc(n*sizeof(double));
  for (i = 1; i <= n; i++)
  {
    lua_pushinteger(L, i);
    lua_gettable(L, pos);
    v[i - 1] = (double)lua_tonumber(L, -1);
    lua_pop(L, 1);
  }
  return v;
}

IUPLUA_SDK_API unsigned char* iuplua_checkuchar_array(lua_State *L, int pos, int n)
{
  int i;
  unsigned char *v;

  luaL_checktype(L, pos, LUA_TTABLE);
  if (n == -1)
    n = iuplua_getn(L, pos);
  else if (n != iuplua_getn(L, pos))
    luaL_argerror(L, pos, "Invalid number of elements (n!=count).");
  if (n<=0) luaL_argerror(L, pos, "Invalid number of elements (n<=0).");

  v = (unsigned char *) malloc (n*sizeof(unsigned char));
  for(i=1; i<=n; i++)
  {
    lua_pushinteger(L,i);
    lua_gettable(L,pos);
    v[i-1] = (unsigned char)lua_tointeger(L, -1);
    lua_pop(L, 1);
  }
  return v;
}

IUPLUA_SDK_API Ihandle ** iuplua_checkihandle_array(lua_State *L, int pos, int n)
{
  int i;
  Ihandle **v;

  luaL_checktype(L, pos, LUA_TTABLE);
  if (n == -1)
    n = iuplua_getn(L, pos);
  else if (n != iuplua_getn(L, pos))
    luaL_argerror(L, pos, "Invalid number of elements (n!=count).");
  if (n<=0) luaL_argerror(L, pos, "Invalid number of elements (n<=0).");

  v = (Ihandle **) malloc ((n+1)*sizeof(Ihandle *));
  for (i=1; i<=n; i++)
  {
    lua_pushinteger(L,i);
    lua_gettable(L,pos);
    v[i-1] = iuplua_checkihandle(L, -1);
    lua_pop(L, 1);
  }
  v[i-1] = NULL;
  return v;
}

             /*************************************/
             /*         used by callbacks         */

#if LUA_VERSION_NUM == 501
static void luaL_checkversion(lua_State *L) 
{
  if (lua_pushthread(L) == 0)
    luaL_error(L, "Must call iup.Open in main thread");

  lua_setfield(L, LUA_REGISTRYINDEX, "MAINTHREAD");
}
#endif

static lua_State* get_main_thread(lua_State* L)
{
  lua_State* main_L;
#if LUA_VERSION_NUM == 501
  lua_getfield(L, LUA_REGISTRYINDEX, "MAINTHREAD");
#else
  lua_rawgeti(L, LUA_REGISTRYINDEX, LUA_RIDX_MAINTHREAD);
#endif
  main_L = lua_tothread(L, -1);
  lua_pop(L, 1);
  return main_L;
}

IUPLUA_SDK_API void iuplua_plugstate(lua_State *L, Ihandle *ih)
{
  /* always store the main thread */
  IupSetAttribute(ih, "_IUPLUA_STATE_CONTEXT",(char *)get_main_thread(L));

  if (IupGetGlobal("IUPLUA_THREADED"))
  {
    int ref;
    lua_pushthread(L);
    ref = luaL_ref(L, LUA_REGISTRYINDEX);   /* keep ref for L, L maybe a thread */
    IupSetInt(ih, "_IUPLUA_STATE_THREAD", ref);
  }
}

IUPLUA_SDK_API lua_State* iuplua_getstate(Ihandle *ih)
{
  return (lua_State *) IupGetAttribute(ih, "_IUPLUA_STATE_CONTEXT");
}

IUPLUA_SDK_API lua_State* iuplua_call_start(Ihandle *ih, const char* name)
{
  lua_State *L = iuplua_getstate(ih);

  /* prepare to call iup.CallMethod(name, ih, ...) */

  iuplua_push_name(L, "CallMethod");
  lua_pushstring(L, name);
  iuplua_pushihandle(L, ih);

  /* here there was 3 value pushed on the stack, 
     if this changes must update callback where LUA_MULTRET is used. */
  return L;
}

static lua_State* iuplua_call_global_start(const char* name)
{
  /* prepare to call iup.CallGlobalMethod(name, ...) */

  lua_State *L = (lua_State *) IupGetGlobal("_IUP_LUA_DEFAULT_STATE");
  if (L)
  {
    iuplua_push_name(L, "CallGlobalMethod");
    lua_pushstring(L, name);
  }

  return L;
}

IUPLUA_SDK_API int iuplua_call(lua_State* L, int nargs)
{
  int status = docall(L, nargs + 2, 1);  /* always 1 result */
  report(L, status);

  if (status != LUA_OK)
    return IUP_DEFAULT;
  else
  {
    int tmp = (int)lua_isnil(L, -1) ? IUP_DEFAULT : (int)lua_tointeger(L, -1);
    lua_pop(L, 1);  /* remove the result */
    return tmp;    
  }
}

static int iuplua_call_global(lua_State* L, int nargs)
{
  return iuplua_call(L, nargs-1); /* remove the ih from the parameter count */
}

IUPLUA_SDK_API char* iuplua_call_ret_s(lua_State *L, int nargs)
{
  int status = docall(L, nargs + 2, 1);  /* always 1 result */
  report(L, status);

  if (status != LUA_OK)
    return NULL;
  else
  {
    char* tmp = lua_isnil(L, -1) ? NULL: (char*)lua_tostring(L,-1);
    lua_pop(L, 1);  /* remove the result */
    return tmp;    
  }
}

IUPLUA_SDK_API double iuplua_call_ret_d(lua_State *L, int nargs)
{
  int status = docall(L, nargs + 2, 1);  /* always 1 result */
  report(L, status);

  if (status != LUA_OK)
    return 0;
  else
  {
    double tmp = lua_isnil(L, -1) ? 0: (double)lua_tonumber(L,-1);
    lua_pop(L, 1);  /* remove the result */
    return tmp;    
  }
}

IUPLUA_SDK_API int iuplua_call_raw(lua_State* L, int nargs, int nresults)
{
  int status = docall(L, nargs, nresults);  /* always n results, or LUA_MULTRET */
  report(L, status);
  return status;
}

IUPLUA_SDK_API void iuplua_register_cb(lua_State *L, const char* name, lua_CFunction func, const char* type)
{
  iuplua_push_name(L, "RegisterCallback");

  lua_pushstring(L, name);
  lua_pushcfunction(L, func);
  lua_pushstring(L, type);
  lua_call(L, 3, 0);  /* iup.RegisterCallback(name, func, type) */
}

/* iup.SetCallback(ih, name, c_func, lua_func) */
static int SetCallback(lua_State *L)
{
  Icallback c_func;

  Ihandle* ih = iuplua_checkihandle(L, 1);
  const char* name = luaL_checkstring(L, 2);

  if (!lua_iscfunction(L, 3))
    luaL_argerror(L, 3, "Invalid function when set callback.");
  c_func = (Icallback)lua_tocfunction(L, 3);

  if (lua_isnil(L, 4))  /* lua_func is only used here to remove the callback */
    IupSetCallback(ih, name, (Icallback)NULL);
  else
    IupSetCallback(ih, name, c_func);

  /* lua_func, when not nil, has always the same name of a C callback in lowercase */

  return 0;
}

static int SetFunction(lua_State *L)
{
  const char* name = luaL_checkstring(L, 1);

  if (lua_isnil(L, 2))
    IupSetFunction(name, NULL);
  else
  {
    Icallback func;

    if (!lua_iscfunction(L, 2))
      luaL_argerror(L, 2, "must be a C function");

    func = (Icallback)lua_tocfunction(L, 2);
    IupSetFunction(name, func);
  }

  return 0;
}

             /*************************************/
             /*        metatable                  */

/* iup.NewClass(class_name) 
  Calls:
    iup.NewClass("iupHandle") -- this is an Ihandle* with enhancements
    iup.NewClass("iupWidget") -- this is a Lua object for control construction, see iup.WIDGET and iup.BOX
*/
static int NewClass(lua_State *L)
{
  lua_newtable(L);     /* push t */
  lua_pushstring(L, "class");  /* push "class" */
  lua_pushvalue(L, 1); /* push again the class_name to the stack, because settable will remove it */
  lua_settable(L, -3); /* t.class = class_name */
  lua_settable(L, LUA_REGISTRYINDEX);  /* registry[class_name] = t */
  return 0;
}

/* iup.SetClass(t, class_name) 
  Calls:
    iup.SetClass(ih, "iupHandle")  --Called only in iup.RegisterHandle and WIDGET.constructor
    iup.SetClass(widget, "iupWidget")  --Called whenever a new control class is created.
*/
static int SetClass(lua_State *L)
{
  lua_gettable(L, LUA_REGISTRYINDEX);  /* t2 = registry[class_name] */
  if (lua_isnil(L, -1))
  {
    lua_pushstring(L, "Invalid class name.");
    lua_error(L);
  }
  lua_setmetatable(L, -2);  /* metatable(t) = t2 */
  return 0;
}

/* class_name = iup.GetClass(t) */
static int GetClass(lua_State *L)
{
  if (lua_istable(L, 1) || lua_isuserdata(L, 1)) 
  {
    lua_getmetatable(L, 1); /* t2 = metatable(t) */
    if (lua_istable(L, -1)) 
    {
      lua_pushstring(L, "class");
      lua_gettable(L, -2);    /* class_name = t2.class */ 
      return 1;
    }
  }
  lua_pushnil(L);
  return 1;
}

/* iup.SetMethod(class_name, method, function)
   For ex:
     iup.SetMethod("iupHandle", "__index", ihandle_gettable)
*/
static int SetMethod(lua_State *L)
{
  lua_pushvalue(L, 1); /* push class_name */
  lua_gettable(L, LUA_REGISTRYINDEX);  /* t = registry[class_name] */
  if (lua_isnil(L, -1)) 
  {
    lua_pushstring(L, "Invalid class name.");
    lua_error(L);
  }
  lua_pushvalue(L, -3); /* push method */
  lua_pushvalue(L, -3); /* push function */
  lua_settable(L, -3);  /* t.method = function */
  return 0;
}

static int IsContainer(lua_State *L)
{
  Ihandle *ih = iuplua_checkihandle(L, 1);
  if (ih)
  {        
    lua_pushboolean(L, ih->iclass->childtype != IUP_CHILDNONE);
    return 1;
  }
  return 0;
}

static int ihandle_tostring (lua_State *L) 
{
  Ihandle *ih = iuplua_checkihandle(L, 1);
  if (!ih)
    lua_pushstring (L, "(not an IUP handle)");
  else
    lua_pushfstring (L, "IUP(%s): %p", IupGetClassName(ih), ih);
  return 1;
}

static int ihandle_compare(lua_State *L)
{
  Ihandle *a = iuplua_checkihandle(L, 1);
  Ihandle *b = iuplua_checkihandle(L, 2);
  if(a == b)
    lua_pushboolean(L, 1);
  else
    lua_pushboolean(L, 0);
  return 1;
}

             /*************************************/
             /*       widget <-> Ihandle*         */

/* local widget = iup.GetWidget(ih) */
static int GetWidget(lua_State *L)
{
  /* Pushes a Lua object that is associated with an Ihandle* */
  /* Used by the "__index" metamethod of the iupHandle */
  Ihandle * ih = iuplua_checkihandle(L, 1);
  char* sref = IupGetAttribute(ih, "_IUPLUA_WIDGET_TABLE_REF");
  if (!sref)
    lua_pushnil(L);
  else
    lua_rawgeti(L, LUA_REGISTRYINDEX, atoi(sref));
  return 1;
}

/* iup.SetWidget(ih, widget) */
static int SetWidget(lua_State *L)
{
  /* Saves the Lua object reference as an attribute, 
     i.e. associates a Lua object with the Ihandle*.
     Used by the "iupWidget" constructor and by RegisterHandle */
  Ihandle * ih = iuplua_checkihandle(L, 1);
  char* sref = IupGetAttribute(ih, "_IUPLUA_WIDGET_TABLE_REF");
  if (!sref)
  {
    int ref = luaL_ref(L, LUA_REGISTRYINDEX);
    IupSetInt(ih, "_IUPLUA_WIDGET_TABLE_REF", ref);
    IupSetCallback(ih, "LDESTROY_CB", ldestroy_cb);
  }
  return 0;
}

             /*************************************/
             /*          registration             */

IUPLUA_SDK_API int iuplua_opencall_internal(lua_State * L)
{
  int ret = 0;
  const char* s;
  iuplua_get_env(L);
  lua_pushliteral(L,"_IUPOPEN_CALL");
  lua_gettable(L, -2);
  s = lua_tostring(L, -1);
  if (s && strcmp(s, "INTERNAL")==0)
    ret = 1;
  lua_pop(L, 2);  /* remove global table and <global table>._IUPOPEN_CALL from stack */
  return ret;
}

/* iup[name] = func */ 
IUPLUA_SDK_API void iuplua_register(lua_State *L, lua_CFunction func, const char* name)
{
  lua_pushcfunction(L, func);
  lua_setfield(L, -2, name);
}

/* iup[name] = s */ 
IUPLUA_SDK_API void iuplua_regstring(lua_State *L, const char* s, const char* name)
{
  lua_pushstring(L, s); 
  lua_setfield(L, -2, name);
}

/* global table */
static const char* iup_globaltable = "iup";

IUPLUA_SDK_API void iuplua_get_env(lua_State *L)
{
  lua_getglobal(L, iup_globaltable);
}

IUPLUA_SDK_API void iuplua_register_lib(lua_State *L, const luaL_Reg* funcs)
{
#if LUA_VERSION_NUM < 502
  luaL_register(L, iup_globaltable, funcs);
#else
  iuplua_get_env(L);
  if (lua_istable(L, -1))
    luaL_setfuncs(L, funcs, 0);
  else
  {
    if (!lua_isnil(L, -1))
      luaL_error(L, "name conflict for module \"%s\"", iup_globaltable);

    lua_newtable(L);
    luaL_setfuncs(L, funcs, 0);
    lua_pushvalue(L, -1);
    lua_setglobal(L, iup_globaltable);
  }
#endif
}

IUPLUA_SDK_API void iuplua_register_funcs(lua_State *L, const luaL_Reg* funcs)
{
#if LUA_VERSION_NUM < 502
  luaL_register(L, NULL, funcs);
#else
  luaL_setfuncs(L, funcs, 0);
#endif
}


/*****************************************************************************
* Common Callbacks                                                     *
****************************************************************************/

static int help_cb(Ihandle *self)
{
  lua_State *L = iuplua_call_start(self, "help_cb");
  return iuplua_call(L, 0);
}

static int getfocus_cb(Ihandle *self)
{
  lua_State *L = iuplua_call_start(self, "getfocus_cb");
  return iuplua_call(L, 0);
}

static int k_any(Ihandle *self, int c)
{
  lua_State *L = iuplua_call_start(self, "k_any");
  lua_pushinteger(L, c);
  return iuplua_call(L, 1);
}

static int killfocus_cb(Ihandle *self)
{
  lua_State *L = iuplua_call_start(self, "killfocus_cb");
  return iuplua_call(L, 0);
}

static int multitouch_cb(Ihandle *ih, int count, int* pid, int* px, int* py, int* pstate)
{
  int i;
  lua_State *L = iuplua_call_start(ih, "multitouch_cb");
  lua_pushinteger(L, count);
  
  lua_createtable(L, count, 0);
  for (i = 0; i < count; i++)
  {
    lua_pushinteger(L,i+1);
    lua_pushinteger(L,pid[i]);
    lua_settable(L,-3);
  }
  
  lua_createtable(L, count, 0);
  for (i = 0; i < count; i++)
  {
    lua_pushinteger(L,i+1);
    lua_pushinteger(L,px[i]);
    lua_settable(L,-3);
  }
  
  lua_createtable(L, count, 0);
  for (i = 0; i < count; i++)
  {
    lua_pushinteger(L,i+1);
    lua_pushinteger(L,py[i]);
    lua_settable(L,-3);
  }
  
  
  lua_createtable(L, count, 0);
  for (i = 0; i < count; i++)
  {
    lua_pushinteger(L,i+1);
    lua_pushinteger(L,pstate[i]);
    lua_settable(L,-3);
  }
  
  return iuplua_call(L, 5);
}

static int attribchanged_cb(Ihandle *self, char * p0)
{
  lua_State *L = iuplua_call_start(self, "attribchanged_cb");
  lua_pushstring(L, p0);
  return iuplua_call(L, 1);
}

static int layoutchanged_cb(Ihandle *self, Ihandle* elem)
{
  lua_State *L = iuplua_call_start(self, "layoutchanged_cb");
  iuplua_pushihandle(L, elem);
  return iuplua_call(L, 1);
}

static void entry_point(void)
{
  lua_State *L = iuplua_call_global_start("entry_point");
  if (L)
    iuplua_call_global(L, 0);
}

static void exit_cb(void)
{
  lua_State *L = iuplua_call_global_start("exit_cb");
  if (L)
    iuplua_call_global(L, 0);
}

static void globalwheel_cb(float delta, int x, int y, char* status)
{
  lua_State *L = iuplua_call_global_start("globalwheel_cb");
  if (L)
  {
    lua_pushnumber(L, delta);
    lua_pushinteger(L, x);
    lua_pushinteger(L, y);
    lua_pushstring(L, status);
    iuplua_call_global(L, 4);
  }
}

static void globalbutton_cb(int button, int pressed, int x, int y, char* status)
{
  lua_State *L = iuplua_call_global_start("globalbutton_cb");
  if (L)
  {
    lua_pushinteger(L, button);
    lua_pushinteger(L, pressed);
    lua_pushinteger(L, x);
    lua_pushinteger(L, y);
    lua_pushstring(L, status);
    iuplua_call_global(L, 5);
  }
}

static void globalmotion_cb(int x, int y, char* status)
{
  lua_State *L = iuplua_call_global_start("globalmotion_cb");
  if (L)
  {
    lua_pushinteger(L, x);
    lua_pushinteger(L, y);
    lua_pushstring(L, status);
    iuplua_call_global(L, 3);
  }
}

static void globalkeypress_cb(int key, int pressed)
{
  lua_State *L = iuplua_call_global_start("globalkeypress_cb");
  if (L)
  {
    lua_pushinteger(L, key);
    lua_pushinteger(L, pressed);
    iuplua_call_global(L, 2);
  }
}

static void globalctrlfunc_cb(int key)
{
  lua_State *L = iuplua_call_global_start("globalctrlfunc_cb");
  if (L)
  {
    lua_pushinteger(L, key);
    iuplua_call_global(L, 1);
  }
}

static int globalidle_cb(void)
{
  lua_State *L = iuplua_call_global_start("idle_action");
  if (L)
    return iuplua_call_global(L, 0);
  else
    return IUP_DEFAULT;
}

static int idle_cb(void)
{
  int ret = 0;
  lua_State *L = (lua_State *) IupGetGlobal("_IUP_LUA_DEFAULT_STATE");
  if (L)
  {
    lua_getglobal(L, "_IUP_LUA_IDLE_FUNC_");
    lua_call(L, 0, 1);  /* _IUP_LUA_IDLE_FUNC_() */
    ret = (int)lua_tointeger(L, -1);
    lua_pop(L, 1);
  }
  return ret;
}

static int SetIdle(lua_State *L)
{
  if (lua_isnoneornil(L,1))
    IupSetFunction("IDLE_ACTION", NULL);
  else
  {
    luaL_checktype(L, 1, LUA_TFUNCTION);
    lua_pushvalue(L,1);
    lua_setglobal(L, "_IUP_LUA_IDLE_FUNC_");
    IupSetFunction("IDLE_ACTION", (Icallback)idle_cb);
  }
  return 0;
}


/*****************************************************************************
* Iuplua bind functions                                                     *
****************************************************************************/


static int GetFromC(lua_State *L)
{
  Ihandle *n;
  const char *a;
  if (!lua_istable(L, -1)) 
  {
    lua_pushstring(L, "iup.GetFromC: wrong arguments to function"); 
    lua_error(L);
    return 0;
  }
  lua_pushinteger(L, 1);
  lua_gettable(L, -2);
  if (!lua_isstring(L, -1)) 
  {
    lua_pushstring(L, "iup.GetFromC: wrong arguments to function"); 
    lua_error(L);
    return 0;
  }
  a = lua_tostring(L, -1);
  n = IupGetHandle((char*)a);
  if(n)
    iuplua_pushihandle(L, n);
  else
    lua_pushnil(L);
  return 1;
}


/*****************************************************************************
 *   iupkey                                                                  *
 ****************************************************************************/


static void register_key(char *name, int code, void* user_data)
{
  lua_State *L = (lua_State*)user_data;
  lua_pushinteger(L, code); 
  lua_setfield(L, -2, name);
}

/* from iupkey.c */
void iupKeyForEach(void (*func)(char *name, int code, void* user_data), void* user_data);

IUPLUA_API int iupkey_open(lua_State *L)
{
  (void)L;
  /* does nothing, kept for backward compatibility */
  return 0;
}


/*****************************************************************************
* iuplua_open                                                               *
****************************************************************************/


static int il_open(lua_State * L)
{
  int ret, argc = 0; 
  char **argv = NULL;

  lua_getglobal(L, "arg");
  if (lua_istable(L, -1))
  {
    int i;
    argc = iuplua_getn(L, -1);
    argv = malloc(sizeof(char*)*argc);
    for(i=1; i<=argc; i++)
    {
      lua_pushinteger(L,i);
      lua_gettable(L,-2);
      argv[i-1] = (char*)lua_tostring(L, -1);
      lua_pop(L, 1);
    }
  }
  lua_pop(L, 1);

  ret = IupOpen(&argc, &argv);
  if (ret == IUP_ERROR)
  {
    if (argv) free(argv);
    lua_pushstring(L, "IupOpen: could not initialize"); 
    lua_error(L);
    return 0;
  }

  if (argv) free(argv);
  lua_pushinteger(L, ret);
  return 1;
}

IUP_SDK_API void iupDlgListDestroySelected(const char* name, void* value);
IUP_SDK_API void iupNamesDestroyHandlesSelected(const char* name, void* value);

IUPLUA_API int iuplua_close(lua_State * L)
{
  lua_pushnil(L);
  lua_setglobal(L, "_IUP_LUA_IDLE_FUNC_");
  IupSetGlobal("_IUP_LUA_DEFAULT_STATE", NULL);

  if (iuplua_opencall_internal(L))
    IupClose();
  else
  {
    /* destroy all dialogs and its children that has that attribute set */
    iupDlgListDestroySelected("_IUPLUA_STATE_CONTEXT", L);

    /* destroy all handles that has that attribute set */
    iupNamesDestroyHandlesSelected("_IUPLUA_STATE_CONTEXT", L);
  }

#if LUA_VERSION_NUM < 502
  /* TODO (??):
     package.loaded["iup"] = nil */
#endif
  lua_pushnil(L);
  lua_setglobal(L, iup_globaltable);

  return 0; /* nothing in stack */
}

static void setinfo (lua_State *L) 
{
  /* global table is at the stack */

  lua_pushliteral (L, IUP_COPYRIGHT);
  lua_setfield(L, -2, "_COPYRIGHT");

  lua_pushliteral (L, IUP_DESCRIPTION);
  lua_setfield(L, -2, "_DESCRIPTION");

  lua_pushliteral (L, IUP_NAME);
  lua_setfield(L, -2, "_NAME");

  lua_pushstring (L, IupVersion());
  lua_setfield(L, -2, "_VERSION");

  lua_pushliteral (L, IUP_VERSION_DATE);
  lua_setfield(L, -2, "_VERSION_DATE");

  lua_pushinteger (L, IupVersionNumber());
  lua_setfield(L, -2, "_VERSION_NUMBER");
}

IUPLUA_API int iuplua_open(lua_State * L)
{
  int ret;

  struct luaL_Reg funcs[] = {
    {"key_open", iupkey_open},
    {"Open", il_open},
    {"Close", iuplua_close},
    {"SetIdle", SetIdle},
    {"GetFromC", GetFromC},
    {"GetWidget", GetWidget},
    {"SetWidget", SetWidget},
    {"NewClass", NewClass},
    {"SetClass", SetClass},
    {"GetClass", GetClass},
    {"IsContainer", IsContainer},
    {"SetMethod", SetMethod},
    {"SetCallback", SetCallback},
    {"SetFunction", SetFunction},
    {"ihandle_compare", ihandle_compare},
    {"ihandle_tostring", ihandle_tostring},
    {"_ERRORMESSAGE", il_error_message},
//    {"dostring", il_dostring},
//    {"dofile", il_dofile},
    { "StringCompare", StringCompare },
    { "StringChangeCase", StringChangeCase },
    { "StringConvertToUTF8", StringConvertToUTF8 },
    { "StringAnsiToUtf8", StringAnsiToUtf8},
    { "StringUtf8ToAnsi", StringUtf8ToAnsi },
    { "CopyUserData2String", CopyUserData2String },
    { "CopyString2UserData", CopyString2UserData },

    { NULL, NULL },
  };

  luaL_checkversion(L);
  
  if (!il_open(L))
    return 0;

  ret = (int)lua_tointeger(L, -1); /* retrieve IupOpen return value */
  lua_pop(L, -1);

  /* Registers functions in iup namespace */
  iuplua_register_lib(L, funcs);  /* leave global table at the top of the stack */
  iupluaapi_open(L);

  /* set version info */
  setinfo(L);

  /* register if IupOpen was called here or from outside IupLua */
  /* iup._IUPOPEN_CALL = EXTERNAL|INTERNAL                      */
  if (ret == IUP_OPENED) 
    lua_pushliteral (L, "EXTERNAL");
  else                   
    lua_pushliteral (L, "INTERNAL");
  lua_setfield(L, -2, "_IUPOPEN_CALL");

  /* used by global callbacks in Lua */
  IupSetGlobal("_IUP_LUA_DEFAULT_STATE", (char *) L);  

#ifdef IUPLUA_USELOH        
#include "iuplua.loh"
#include "constants.loh"
#else
#ifdef IUPLUA_USELH
#include "iuplua.lh"
#include "constants.lh"
#else
  iuplua_dofile(L, "iuplua.lua");
  iuplua_dofile(L, "constants.lua");
#endif
#endif

  /* Register the common callbacks */
  iuplua_register_cb(L, "HELP_CB", (lua_CFunction)help_cb, NULL);
  iuplua_register_cb(L, "GETFOCUS_CB", (lua_CFunction)getfocus_cb, NULL);
  iuplua_register_cb(L, "K_ANY", (lua_CFunction)k_any, NULL);
  iuplua_register_cb(L, "KILLFOCUS_CB", (lua_CFunction)killfocus_cb, NULL);
  iuplua_register_cb(L, "MULTITOUCH_CB", (lua_CFunction)multitouch_cb, NULL);

  /* Register global callbacks */
  iuplua_register_cb(L, "ENTRY_POINT", (lua_CFunction)entry_point, NULL);
  iuplua_register_cb(L, "EXIT_CB", (lua_CFunction)exit_cb, NULL);
  iuplua_register_cb(L, "GLOBALWHEEL_CB", (lua_CFunction)globalwheel_cb, NULL);
  iuplua_register_cb(L, "GLOBALBUTTON_CB", (lua_CFunction)globalbutton_cb, NULL);
  iuplua_register_cb(L, "GLOBALMOTION_CB", (lua_CFunction)globalmotion_cb, NULL);
  iuplua_register_cb(L, "GLOBALKEYPRESS_CB", (lua_CFunction)globalkeypress_cb, NULL);
  iuplua_register_cb(L, "GLOBALCTRLFUNC_CB", (lua_CFunction)globalctrlfunc_cb, NULL);
  iuplua_register_cb(L, "IDLE_ACTION", (lua_CFunction)globalidle_cb, NULL);

  /* Other callbacks */
  iuplua_register_cb(L, "ATTRIBCHANGED_CB", (lua_CFunction)attribchanged_cb, NULL);
  iuplua_register_cb(L, "LAYOUTCHANGED_CB", (lua_CFunction)layoutchanged_cb, NULL);

  /* Register Keys */
  iupKeyForEach(register_key, (void*)L);

  /* Iup Modules initialization */
  iupbuttonlua_open(L);
  iupluadraw_open(L);  /* must be before canvas */
  iupcanvaslua_open(L);
  iupdialoglua_open(L);
  iupfilllua_open(L);
  iupframelua_open(L);
  iupfiledlglua_open(L);
  iuphboxlua_open(L);
  iupitemlua_open(L);
  iupimagelua_open(L);
  iuplabellua_open(L);
  iuplistlua_open(L);
  iupmenulua_open(L);
  iupmultilinelua_open(L);
  iupradiolua_open(L);
  iupseparatorlua_open(L);
  iupsubmenulua_open(L);
  iuptextlua_open(L);
  iuptogglelua_open(L);
  iupvboxlua_open(L);
  iupzboxlua_open(L);
  iuptimerlua_open(L);
  iupsboxlua_open(L);
  iupsplitlua_open(L);
  iupspinlua_open(L);
  iupspinboxlua_open(L);
  iupscrollboxlua_open(L);
  iupgridboxlua_open(L);
  iupmultiboxlua_open(L);
  iupexpanderlua_open(L);
  iuplinklua_open(L);
  iupcboxlua_open(L);
  iupdetachboxlua_open(L);
  iupbackgroundboxlua_open(L);
  iupgclua_open(L);
  iupgetparamlua_open(L);
  iupvallua_open(L);
  iuptabslua_open(L);
  iupfontdlglua_open(L);
  iupmessagedlglua_open(L);
  iupcolordlglua_open(L);
  iupimagergbalua_open(L);
  iupimagergblua_open(L);
  iupprogressbarlua_open(L);
  iupnormalizerlua_open(L);
  iupuserlua_open(L);
  iupthreadlua_open(L);
  iuptreelua_open(L);
  iupclipboardlua_open(L);
  iupprogressdlglua_open(L);
  iupflatlabellua_open(L);
  iupflatbuttonlua_open(L);
  iupflattogglelua_open(L);
  iupdropbuttonlua_open(L);
  iupflatframelua_open(L);
  iupflatseparatorlua_open(L);
  iupflatlistlua_open(L);
  iupflatvallua_open(L);
  iupflattreelua_open(L);
  iupspacelua_open(L);
  iupconfiglua_open(L);
  iupanimatedlabellua_open(L);
  iupcalendarlua_open(L);
  iupdatepicklua_open(L);
  iupflattabslua_open(L);
  iupflatscrollboxlua_open(L);
  iupgaugelua_open(L);
  iupdiallua_open(L);
  iupcolorbarlua_open(L);
  iupcolorbrowserlua_open(L);

  return 0; /* nothing in stack */
}

/* obligatory to use require"iuplua" */
IUPLUA_SDK_API int luaopen_iuplua(lua_State* L)
{
  return iuplua_open(L);
}

