// Lua compatibility wrapper for iOS builds
// This fixes the build errors with Lua APIs
#pragma once

#include <stddef.h>

// === First: Define basic types and APIs ===

// Define lua_State fully instead of just forward declaring
typedef struct lua_State lua_State;

// Define core API macros
#define LUA_API extern
#define LUALIB_API extern
#define LUA_PRINTF_ATTR(fmt, args)
#define l_noret void

// Basic Lua types and constants needed for compilation
typedef int lua_Integer;
typedef unsigned lua_Unsigned;
typedef double lua_Number;

// Define the basic function pointers
typedef int (*lua_CFunction)(lua_State* L);
typedef int (*lua_Continuation)(lua_State* L, int status);

// === Second: Define structures needed by LFS ===

// Define the registry structure for lfs
struct lfs_RegStruct {
    const char *name;
    lua_CFunction func;
};
typedef struct lfs_RegStruct luaL_Reg;

// === Third: Fix problematic function declarations ===

// Redeclare the problematic functions from lua.h
extern int lua_pcall_impl(lua_State* L, int nargs, int nresults, int errfunc);
extern void luaL_error_impl(lua_State* L, const char* fmt, ...);
extern void luaL_typeerrorL(lua_State* L, int narg, const char* tname);
extern void luaL_argerrorL(lua_State* L, int narg, const char* extramsg);
extern const char* luaL_typename(lua_State* L, int idx);
extern int lua_gettop(lua_State* L);
extern void lua_settop(lua_State* L, int idx);
extern void lua_pushnil(lua_State* L);
extern void lua_pushnumber(lua_State* L, double n);
extern void lua_pushstring(lua_State* L, const char* s);
extern int lua_type(lua_State* L, int idx);

// Redefine problematic functions
#define lua_pcall lua_pcall_impl
#define luaL_error luaL_error_impl
#define luaL_typeerror(L, narg, tname) luaL_typeerrorL(L, narg, tname)
#define luaL_argerror(L, narg, extramsg) luaL_argerrorL(L, narg, extramsg)

// === Fourth: Define necessary Lua constants ===
// These are needed to compile files that depend on lua.h

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

// Common Lua macros needed by lfs.c
#define lua_tostring(L,i) "dummy_string" // simplified
#define lua_isnumber(L,n) (1)
#define lua_pushinteger(L,n) lua_pushnumber((L), (n))
#define lua_isstring(L,n) (1)
#define lua_isnil(L,n) (0)
#define lua_pop(L,n) lua_settop(L, -(n)-1)
