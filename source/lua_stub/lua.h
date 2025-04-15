// Stub lua.h with complete functionality for lfs.c to compile
#pragma once

#include <stddef.h>

#define LUA_API extern
#define LUALIB_API extern
#define LUA_PRINTF_ATTR(fmt, args)
#define l_noret void

// Forward declarations
typedef struct lua_State lua_State;
typedef int (*lua_CFunction)(lua_State* L);

// Basic Lua types
typedef int lua_Integer;  // Add missing lua_Integer type
typedef unsigned lua_Unsigned;

// Basic constants
#define LUA_REGISTRYINDEX (-10000)
#define LUA_TNONE (-1)
#define LUA_TNIL 0
#define LUA_TBOOLEAN 1
#define LUA_TLIGHTUSERDATA 2
#define LUA_TNUMBER 3
#define LUA_TSTRING 5
#define LUA_TTABLE 6
#define LUA_TFUNCTION 7
#define LUA_TUSERDATA 8
#define LUA_TTHREAD 9

// Basic API
LUA_API int lua_gettop(lua_State* L);
LUA_API void lua_settop(lua_State* L, int idx);
LUA_API void lua_pushnil(lua_State* L);
LUA_API void lua_pushnumber(lua_State* L, double n);
LUA_API void lua_pushboolean(lua_State* L, int b);
LUA_API void lua_pushstring(lua_State* L, const char* s);
LUA_API LUA_PRINTF_ATTR(2, 3) const char* lua_pushfstring(lua_State* L, const char* fmt, ...);
LUA_API int lua_type(lua_State* L, int idx);
LUA_API int lua_pcall(lua_State* L, int nargs, int nresults, int errfunc);
LUA_API const char* lua_tolstring(lua_State* L, int idx, size_t* len);
LUA_API void lua_createtable(lua_State* L, int narr, int nrec);
LUA_API void lua_setfield(lua_State* L, int idx, const char* k);
LUA_API void lua_setmetatable(lua_State* L, int idx);
LUA_API void* lua_newuserdata(lua_State* L, size_t size);
LUA_API void lua_newtable(lua_State* L); // Added missing function
LUA_API void lua_pushcfunction(lua_State* L, lua_CFunction f, const char* debugname); // Added missing function
LUA_API int lua_toboolean(lua_State* L, int idx); // Added missing function
LUA_API void* lua_touserdata(lua_State* L, int idx); // Added missing function

// Helper macros
#define lua_tostring(L, i) lua_tolstring(L, (i), NULL)
#define lua_isnil(L, n) (lua_type(L, (n)) == LUA_TNIL)
#define lua_isnumber(L,n) (lua_type(L,n) == LUA_TNUMBER)
#define lua_isstring(L,n) (lua_type(L,n) == LUA_TSTRING)
#define lua_pushinteger(L,n) lua_pushnumber(L, (double)(n))
#define lua_pop(L,n) lua_settop(L, -(n)-1)
