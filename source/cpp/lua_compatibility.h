// lua_compatibility.h - Enhanced compatibility layer for Lua/Luau integration
#pragma once

// Include standard headers for size_t
#include <stddef.h>  // For size_t in C
#ifdef __cplusplus
#include <cstddef>   // For size_t in C++
#endif

// Define essential compatibility macros for Lua/Luau headers
// These MUST be defined before including any Lua headers

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

// Define core structs used by our libraries
#ifndef luaL_Reg
struct luaL_RegStruct {
    const char *name;
    int (*func)(lua_State *L);
};
typedef struct luaL_RegStruct luaL_Reg;
#endif

// Forward declaration of lua_State to avoid including it
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

// Define important Lua constants
#ifndef LUA_REGISTRYINDEX
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
#endif

// Macro for varargs function declarations (needed for LuaJIT compatibility)
#ifndef va_list
#include <stdarg.h>
#endif

// Forward-declare critical functions that might cause linking issues
#ifdef __cplusplus
extern "C" {
#endif

// Fix problematic static function pointer by declaring it externally
#ifdef lua_pcall
#undef lua_pcall
#endif
extern int lua_pcall(lua_State* L, int nargs, int nresults, int errfunc);

// Fix typeerror and argerror macros
extern void luaL_typeerrorL(lua_State* L, int narg, const char* tname);
extern void luaL_argerrorL(lua_State* L, int narg, const char* extramsg);
#define luaL_typeerror(L, narg, tname) luaL_typeerrorL(L, narg, tname)
#define luaL_argerror(L, narg, extramsg) luaL_argerrorL(L, narg, extramsg)

// Core Lua API functions
LUA_API int lua_gettop(lua_State* L);
LUA_API void lua_settop(lua_State* L, int idx);
LUA_API void lua_pushvalue(lua_State* L, int idx);
LUA_API void lua_remove(lua_State* L, int idx);
LUA_API void lua_insert(lua_State* L, int idx);
LUA_API void lua_replace(lua_State* L, int idx);
LUA_API int lua_checkstack(lua_State* L, int sz);
LUA_API void lua_xmove(lua_State* from, lua_State* to, int n);

// Stack access functions
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

// Push functions
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
LUA_API int lua_pushthread(lua_State* L);

// Get functions
LUA_API void lua_gettable(lua_State* L, int idx);
LUA_API void lua_getfield(lua_State* L, int idx, const char* k);
LUA_API void lua_rawget(lua_State* L, int idx);
LUA_API void lua_rawgeti(lua_State* L, int idx, int n);
LUA_API void lua_createtable(lua_State* L, int narr, int nrec);
LUA_API void* lua_newuserdata(lua_State* L, size_t sz);
LUA_API int lua_getmetatable(lua_State* L, int objindex);
LUA_API void lua_getfenv(lua_State* L, int idx);

// Set functions
LUA_API void lua_settable(lua_State* L, int idx);
LUA_API void lua_setfield(lua_State* L, int idx, const char* k);
LUA_API void lua_rawset(lua_State* L, int idx);
LUA_API void lua_rawseti(lua_State* L, int idx, int n);
LUA_API int lua_setmetatable(lua_State* L, int objindex);
LUA_API int lua_setfenv(lua_State* L, int idx);

// `Load` and `Call` functions (load and run Lua code)
LUA_API void lua_call(lua_State* L, int nargs, int nresults);
LUA_API int lua_pcall(lua_State* L, int nargs, int nresults, int errfunc);
LUA_API int lua_cpcall(lua_State* L, lua_CFunction func, void* ud);
LUA_API int lua_load(lua_State* L, lua_Reader reader, void* dt, const char* chunkname);

// Coroutine functions
LUA_API int lua_yield(lua_State* L, int nresults);
LUA_API int lua_resume(lua_State* L, int narg);
LUA_API int lua_status(lua_State* L);

// Garbage collection functions and options
LUA_API int lua_gc(lua_State* L, int what, int data);

// Miscellaneous functions
LUA_API int lua_error(lua_State* L);
LUA_API int lua_next(lua_State* L, int idx);
LUA_API void lua_concat(lua_State* L, int n);
LUA_API lua_Alloc lua_getallocf(lua_State* L, void** ud);
LUA_API void lua_setallocf(lua_State* L, lua_Alloc f, void* ud);

// Luaaux functions (lauxlib.h)
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

// Common allocators
LUALIB_API void* lua_alloc(size_t size);
LUALIB_API void lua_free(void* block);
LUALIB_API void* lua_realloc(void* block, size_t size);

// For Lua compatibility (defined for forward compatibility)
typedef size_t (*lua_Reader)(lua_State* L, void* data, size_t* size);
typedef int (*lua_Writer)(lua_State* L, const void* p, size_t sz, void* ud);
typedef void* (*lua_Alloc)(void* ud, void* ptr, size_t osize, size_t nsize);

// Luau-specific functions
#define new_lib(L, l) (lua_createtable(L, 0, sizeof(l)/sizeof((l)[0]) - 1), luaL_register(L, NULL, l))

// Function-like macros
#ifndef lua_pop
#define lua_pop(L, n) lua_settop(L, -(n)-1)
#endif

#ifndef lua_newtable
#define lua_newtable(L) lua_createtable(L, 0, 0)
#endif

#ifndef lua_isfunction
#define lua_isfunction(L, n) (lua_type(L, (n)) == LUA_TFUNCTION)
#endif

#ifndef lua_istable
#define lua_istable(L, n) (lua_type(L, (n)) == LUA_TTABLE)
#endif

#ifndef lua_isboolean
#define lua_isboolean(L, n) (lua_type(L, (n)) == LUA_TBOOLEAN)
#endif

#ifndef lua_isnil
#define lua_isnil(L, n) (lua_type(L, (n)) == LUA_TNIL)
#endif

#ifndef lua_isthread
#define lua_isthread(L, n) (lua_type(L, (n)) == LUA_TTHREAD)
#endif

#ifdef __cplusplus
}
#endif
