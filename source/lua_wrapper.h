// Standalone Lua wrapper for executor - No conflict with real Lua
// This file should NOT be included along with real Lua headers
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// Forward declarations of essential types
typedef struct lua_State lua_State;
typedef int (*lua_CFunction)(lua_State* L);

// Essential function declarations - different names from real Lua to avoid conflicts
extern int executor_lua_pcall(lua_State* L, int nargs, int nresults, int errfunc);
extern void executor_luaL_error(lua_State* L, const char* fmt, ...);
extern const char* executor_luaL_typename(lua_State* L, int idx);
extern int executor_lua_gettop(lua_State* L);
extern void executor_lua_settop(lua_State* L, int idx);
extern void executor_lua_pushnil(lua_State* L);
extern void executor_lua_pushnumber(lua_State* L, double n);
extern void executor_lua_pushstring(lua_State* L, const char* s);
extern void executor_lua_createtable(lua_State* L, int narr, int nrec);
extern void executor_lua_setfield(lua_State* L, int idx, const char* k);
extern int executor_lua_type(lua_State* L, int idx);

// Redirect to our implementation with macros
#define lua_pcall executor_lua_pcall
#define luaL_error executor_luaL_error
#define luaL_typename executor_luaL_typename
#define lua_gettop executor_lua_gettop
#define lua_settop executor_lua_settop
#define lua_pushnil executor_lua_pushnil
#define lua_pushnumber executor_lua_pushnumber
#define lua_pushstring executor_lua_pushstring
#define lua_createtable executor_lua_createtable
#define lua_setfield executor_lua_setfield
#define lua_type executor_lua_type

// Constants that don't conflict with real Lua
#define EXECUTOR_LUA_REGISTRYINDEX (-10000)
#define EXECUTOR_LUA_ENVIRONINDEX (-10001)
#define EXECUTOR_LUA_GLOBALSINDEX (-10002)

#define EXECUTOR_LUA_TNONE (-1)
#define EXECUTOR_LUA_TNIL 0
#define EXECUTOR_LUA_TBOOLEAN 1
#define EXECUTOR_LUA_TLIGHTUSERDATA 2
#define EXECUTOR_LUA_TNUMBER 3
#define EXECUTOR_LUA_TSTRING 5

// Helper macros that won't conflict
#define lua_isnil(L,n) (executor_lua_type(L,n) == EXECUTOR_LUA_TNIL)
#define lua_isnumber(L,n) (executor_lua_type(L,n) == EXECUTOR_LUA_TNUMBER)
#define lua_pushinteger(L,n) executor_lua_pushnumber(L, (double)(n))
#define lua_pop(L,n) executor_lua_settop(L, -(n)-1)

// Registry structure that won't conflict with real Lua
struct ExecutorLuaReg {
    const char* name;
    lua_CFunction func;
};

#ifdef __cplusplus
}
#endif
