// lua_compatibility.h - Comprehensive compatibility layer for Lua/Luau integration
#pragma once

// Include standard headers for size_t
#include <stddef.h>  // For size_t in C
#include <stdarg.h>  // For va_list
#ifdef __cplusplus
#include <cstddef>   // For size_t in C++
#include <cstdarg>   // For va_list in C++
#endif

// Main API export macros
#ifndef LUA_API
#define LUA_API extern
#endif

#ifndef LUALIB_API 
#define LUALIB_API extern
#endif

// Format attribute macro needs to take format and args, but we define it empty for non-GCC
#ifndef LUA_PRINTF_ATTR
#ifdef __GNUC__
#define LUA_PRINTF_ATTR(fmt,args) __attribute__((format(printf, fmt, args)))
#else
#define LUA_PRINTF_ATTR(fmt,args)
#endif
#endif

// LUA_NORETURN definition based on compiler
#ifndef LUA_NORETURN
#ifdef __GNUC__
#define LUA_NORETURN __attribute__((__noreturn__))
#elif defined(_MSC_VER)
#define LUA_NORETURN __declspec(noreturn)
#else
#define LUA_NORETURN
#endif
#endif

// Define l_noret for compatibility
#ifndef l_noret
#define l_noret void
#endif

// Missing configuration constants from luaconf.h
#ifndef LUAI_USER_ALIGNMENT_T
#define LUAI_USER_ALIGNMENT_T double
#endif

#ifndef LUA_EXTRA_SIZE
#define LUA_EXTRA_SIZE 0
#endif

#ifndef LUA_SIZECLASSES
#define LUA_SIZECLASSES 32
#endif

#ifndef LUA_MEMORY_CATEGORIES
#define LUA_MEMORY_CATEGORIES 8
#endif

#ifndef LUA_UTAG_LIMIT
#define LUA_UTAG_LIMIT 8
#endif

// Additional compatibility macros
#ifndef lua_check
#define lua_check(e) ((void)0)
#endif

#ifndef luai_apicheck
#define luai_apicheck(L, e) lua_check(e)
#endif

// Forward declaration of lua_State
#ifndef lua_State
typedef struct lua_State lua_State;
#endif

// Basic types needed for Lua integration
typedef long lua_Integer;
typedef unsigned long lua_Unsigned;
typedef double lua_Number;

// Function pointer types
typedef int (*lua_CFunction)(lua_State* L);
typedef int (*lua_Continuation)(lua_State* L, int status);

// Forward declare other important types
typedef size_t (*lua_Reader)(lua_State* L, void* data, size_t* size);
typedef int (*lua_Writer)(lua_State* L, const void* p, size_t sz, void* ud);
typedef void* (*lua_Alloc)(void* ud, void* ptr, size_t osize, size_t nsize);

// luaL_Reg structure definition - must be before its uses
typedef struct luaL_Reg {
    const char *name;
    lua_CFunction func;
} luaL_Reg;

// Lua constants
#define LUA_REGISTRYINDEX (-10000)
#define LUA_ENVIRONINDEX (-10001)
#define LUA_GLOBALSINDEX (-10002)

#define LUA_TNONE (-1)
#define LUA_TNIL 0
#define LUA_TBOOLEAN 1
#define LUA_TLIGHTUSERDATA 2
#define LUA_TNUMBER 3
#define LUA_TVECTOR 4
#define LUA_TSTRING 5
#define LUA_TTABLE 6
#define LUA_TFUNCTION 7
#define LUA_TUSERDATA 8
#define LUA_TTHREAD 9

// GC constants
#define LUA_GCSTOP       0
#define LUA_GCRESTART    1
#define LUA_GCCOLLECT    2
#define LUA_GCCOUNT      3
#define LUA_GCCOUNTB     4
#define LUA_GCSTEP       5
#define LUA_GCSETPAUSE   6
#define LUA_GCSETSTEPMUL 7

// Define C API functions
#ifdef __cplusplus
extern "C" {
#endif

// Core Lua API functions
LUA_API lua_State* lua_newstate(lua_Alloc f, void* ud);
LUA_API void lua_close(lua_State* L);
LUA_API lua_State* lua_newthread(lua_State* L);
LUA_API lua_CFunction lua_atpanic(lua_State* L, lua_CFunction panicf);

// Basic stack manipulation
LUA_API int lua_gettop(lua_State* L);
LUA_API void lua_settop(lua_State* L, int idx);
LUA_API void lua_pushvalue(lua_State* L, int idx);
LUA_API void lua_remove(lua_State* L, int idx);
LUA_API void lua_insert(lua_State* L, int idx);
LUA_API void lua_replace(lua_State* L, int idx);
LUA_API int lua_checkstack(lua_State* L, int sz);
LUA_API void lua_xmove(lua_State* from, lua_State* to, int n);

// Access functions (stack -> C)
LUA_API int lua_isnumber(lua_State* L, int idx);
LUA_API int lua_isstring(lua_State* L, int idx);
LUA_API int lua_iscfunction(lua_State* L, int idx);
LUA_API int lua_isuserdata(lua_State* L, int idx);
LUA_API int lua_type(lua_State* L, int idx);
LUA_API const char* lua_typename(lua_State* L, int tp);
LUA_API int lua_equal(lua_State* L, int idx1, int idx2);
LUA_API int lua_rawequal(lua_State* L, int idx1, int idx2);
LUA_API int lua_lessthan(lua_State* L, int idx1, int idx2);
LUA_API lua_Number lua_tonumber(lua_State* L, int idx);
LUA_API lua_Integer lua_tointeger(lua_State* L, int idx);
LUA_API int lua_toboolean(lua_State* L, int idx);
LUA_API const char* lua_tostring(lua_State* L, int idx);
LUA_API size_t lua_strlen(lua_State* L, int idx);
LUA_API lua_CFunction lua_tocfunction(lua_State* L, int idx);
LUA_API void* lua_touserdata(lua_State* L, int idx);
LUA_API lua_State* lua_tothread(lua_State* L, int idx);
LUA_API const void* lua_topointer(lua_State* L, int idx);

// Push functions (C -> stack)
LUA_API void lua_pushnil(lua_State* L);
LUA_API void lua_pushnumber(lua_State* L, lua_Number n);
LUA_API void lua_pushinteger(lua_State* L, lua_Integer n);
LUA_API void lua_pushlstring(lua_State* L, const char* s, size_t l);
LUA_API void lua_pushstring(lua_State* L, const char* s);
LUA_API const char* lua_pushfstring(lua_State* L, const char* fmt, ...) LUA_PRINTF_ATTR(2, 3);
LUA_API const char* lua_pushvfstring(lua_State* L, const char* fmt, va_list argp);
LUA_API void lua_pushcclosure(lua_State* L, lua_CFunction fn, int n);
LUA_API void lua_pushboolean(lua_State* L, int b);
LUA_API void lua_pushlightuserdata(lua_State* L, void* p);
LUA_API void lua_pushliteral(lua_State* L, const char* s);
LUA_API int lua_pushthread(lua_State* L);

// Get functions (Lua -> stack)
LUA_API void lua_gettable(lua_State* L, int idx);
LUA_API void lua_getfield(lua_State* L, int idx, const char* k);
LUA_API void lua_rawget(lua_State* L, int idx);
LUA_API void lua_rawgeti(lua_State* L, int idx, int n);
LUA_API void lua_createtable(lua_State* L, int narr, int nrec);
LUA_API void* lua_newuserdata(lua_State* L, size_t sz);
LUA_API int lua_getmetatable(lua_State* L, int objindex);
LUA_API void lua_getfenv(lua_State* L, int idx);
LUA_API void lua_getglobal(lua_State* L, const char* name);

// Set functions (stack -> Lua)
LUA_API void lua_settable(lua_State* L, int idx);
LUA_API void lua_setfield(lua_State* L, int idx, const char* k);
LUA_API void lua_rawset(lua_State* L, int idx);
LUA_API void lua_rawseti(lua_State* L, int idx, int n);
LUA_API int lua_setmetatable(lua_State* L, int objindex);
LUA_API int lua_setfenv(lua_State* L, int idx);
LUA_API void lua_setglobal(lua_State* L, const char* name);

// `Load` and `Call` functions (load and run Lua code)
LUA_API void lua_call(lua_State* L, int nargs, int nresults);
LUA_API int lua_pcall(lua_State* L, int nargs, int nresults, int errfunc);
LUA_API int lua_cpcall(lua_State* L, lua_CFunction func, void* ud);
LUA_API int lua_load(lua_State* L, lua_Reader reader, void* dt, const char* chunkname);
LUA_API int lua_dump(lua_State* L, lua_Writer writer, void* data);

// Coroutine functions
LUA_API int lua_yield(lua_State* L, int nresults);
LUA_API int lua_resume(lua_State* L, int narg);
LUA_API int lua_status(lua_State* L);

// Garbage collection functions
LUA_API int lua_gc(lua_State* L, int what, int data);

// Miscellaneous functions
LUA_API int lua_error(lua_State* L);
LUA_API int lua_next(lua_State* L, int idx);
LUA_API void lua_concat(lua_State* L, int n);
LUA_API lua_Alloc lua_getallocf(lua_State* L, void** ud);
LUA_API void lua_setallocf(lua_State* L, lua_Alloc f, void* ud);

// Some useful macros
#define lua_pop(L,n)                lua_settop(L, -(n)-1)
#define lua_newtable(L)             lua_createtable(L, 0, 0)
#define lua_register(L,n,f)         (lua_pushcfunction(L, (f)), lua_setglobal(L, (n)))
#define lua_pushcfunction(L,f)      lua_pushcclosure(L, (f), 0)
#define lua_isfunction(L,n)         (lua_type(L, (n)) == LUA_TFUNCTION)
#define lua_istable(L,n)            (lua_type(L, (n)) == LUA_TTABLE)
#define lua_islightuserdata(L,n)    (lua_type(L, (n)) == LUA_TLIGHTUSERDATA)
#define lua_isnil(L,n)              (lua_type(L, (n)) == LUA_TNIL)
#define lua_isboolean(L,n)          (lua_type(L, (n)) == LUA_TBOOLEAN)
#define lua_isthread(L,n)           (lua_type(L, (n)) == LUA_TTHREAD)
#define lua_isnone(L,n)             (lua_type(L, (n)) == LUA_TNONE)
#define lua_isnoneornil(L, n)       (lua_type(L, (n)) <= 0)
#define lua_pushliteral(L, s)       lua_pushlstring(L, "" s, (sizeof(s)/sizeof(char))-1)

// Debug API
LUA_API int lua_getstack(lua_State* L, int level, void* ar);
LUA_API int lua_getinfo(lua_State* L, const char* what, void* ar);
LUA_API const char* lua_getlocal(lua_State* L, const void* ar, int n);
LUA_API const char* lua_setlocal(lua_State* L, const void* ar, int n);
LUA_API const char* lua_getupvalue(lua_State* L, int funcindex, int n);
LUA_API const char* lua_setupvalue(lua_State* L, int funcindex, int n);
LUA_API int lua_sethook(lua_State* L, void* func, int mask, int count);
LUA_API void* lua_gethook(lua_State* L);
LUA_API int lua_gethookmask(lua_State* L);
LUA_API int lua_gethookcount(lua_State* L);

// Standard library functions (lauxlib.h)
LUALIB_API void luaL_openlib(lua_State* L, const char* libname, const luaL_Reg* l, int nup);
LUALIB_API void luaL_register(lua_State* L, const char* libname, const luaL_Reg* l);
LUALIB_API int luaL_getmetafield(lua_State* L, int obj, const char* e);
LUALIB_API int luaL_callmeta(lua_State* L, int obj, const char* e);
LUALIB_API int luaL_typerror(lua_State* L, int narg, const char* tname);
LUALIB_API int luaL_argerror(lua_State* L, int numarg, const char* extramsg);
LUALIB_API const char* luaL_checklstring(lua_State* L, int numArg, size_t* l);
LUALIB_API const char* luaL_optlstring(lua_State* L, int numArg, const char* def, size_t* l);
LUALIB_API lua_Number luaL_checknumber(lua_State* L, int numArg);
LUALIB_API lua_Number luaL_optnumber(lua_State* L, int nArg, lua_Number def);
LUALIB_API lua_Integer luaL_checkinteger(lua_State* L, int numArg);
LUALIB_API lua_Integer luaL_optinteger(lua_State* L, int nArg, lua_Integer def);
LUALIB_API void luaL_checkstack(lua_State* L, int sz, const char* msg);
LUALIB_API void luaL_checktype(lua_State* L, int narg, int t);
LUALIB_API void luaL_checkany(lua_State* L, int narg);
LUALIB_API int luaL_newmetatable(lua_State* L, const char* tname);
LUALIB_API void* luaL_checkudata(lua_State* L, int ud, const char* tname);
LUALIB_API void luaL_where(lua_State* L, int lvl);
LUALIB_API int luaL_error(lua_State* L, const char* fmt, ...) LUA_PRINTF_ATTR(2, 3);
LUALIB_API int luaL_checkoption(lua_State* L, int narg, const char* def, const char* const lst[]);
LUALIB_API int luaL_ref(lua_State* L, int t);
LUALIB_API void luaL_unref(lua_State* L, int t, int ref);
LUALIB_API int luaL_loadfile(lua_State* L, const char* filename);
LUALIB_API int luaL_loadbuffer(lua_State* L, const char* buff, size_t sz, const char* name);
LUALIB_API int luaL_loadstring(lua_State* L, const char* s);
LUALIB_API lua_State* luaL_newstate(void);
LUALIB_API const char* luaL_gsub(lua_State* L, const char* s, const char* p, const char* r);
LUALIB_API const char* luaL_findtable(lua_State* L, int idx, const char* fname, int szhint);
LUALIB_API void luaL_getmetatable(lua_State* L, const char* name);
LUALIB_API const char* luaL_typename(lua_State* L, int idx);
LUALIB_API const char* luaL_checkstring(lua_State* L, int idx);
LUALIB_API void luaL_argcheck(lua_State* L, int cond, int narg, const char* extramsg);

// Custom macros and functions for compatibility
#define new_lib(L, l) (lua_createtable(L, 0, sizeof(l)/sizeof((l)[0]) - 1), luaL_register(L, NULL, l))

// This definition is different to avoid conflicts with multiple definitions
#define luaL_argcheck(L, cond, narg, extramsg) ((void)((cond) || luaL_argerror(L, (narg), (extramsg))))

#ifdef __cplusplus
}
#endif
