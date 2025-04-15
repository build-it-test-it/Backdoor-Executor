// Standalone Lua wrapper for executor - For use in non-Lua files only
#pragma once

// If real Lua headers are already included, this file does nothing
#ifndef _lua_already_included
#define _lua_already_included

#ifdef __cplusplus
extern "C" {
#endif

// Basic type definitions
typedef struct lua_State lua_State;
typedef int (*lua_CFunction)(lua_State* L);

// API function declarations
extern int lua_pcall(lua_State* L, int nargs, int nresults, int errfunc);
extern void luaL_error(lua_State* L, const char* fmt, ...);
extern int lua_gettop(lua_State* L);
extern void lua_settop(lua_State* L, int idx);
extern void lua_pushnil(lua_State* L);
extern void lua_pushnumber(lua_State* L, double n);
extern void lua_pushboolean(lua_State* L, int b);
extern void lua_pushstring(lua_State* L, const char* s);
extern void lua_createtable(lua_State* L, int narr, int nrec);
extern void lua_setfield(lua_State* L, int idx, const char* k);
extern int lua_type(lua_State* L, int idx);
extern const char* luaL_typename(lua_State* L, int idx);

// Basic constants
#define LUA_REGISTRYINDEX (-10000)
#define LUA_ENVIRONINDEX (-10001)
#define LUA_GLOBALSINDEX (-10002)

#define LUA_TNONE (-1)
#define LUA_TNIL 0
#define LUA_TBOOLEAN 1
#define LUA_TLIGHTUSERDATA 2
#define LUA_TNUMBER 3
#define LUA_TSTRING 5

// Helper macros
#define lua_isnil(L,n) (lua_type(L,n) == LUA_TNIL)
#define lua_isnumber(L,n) (lua_type(L,n) == LUA_TNUMBER)
#define lua_pushinteger(L,n) lua_pushnumber(L, (double)(n))
#define lua_pop(L,n) lua_settop(L, -(n)-1)
#define lua_tostring(L,i) "dummy_string"

// Registry structure
struct lfs_RegStruct {
    const char *name;
    lua_CFunction func;
};
typedef struct lfs_RegStruct luaL_Reg;

#ifdef __cplusplus
}
#endif

#endif // _lua_already_included
