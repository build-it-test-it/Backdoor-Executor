// Lua compatibility wrapper for iOS builds
// This file provides compatibility without conflicts
#pragma once

// Only define these types and macros if they're not already defined
#ifndef lua_State
typedef struct lua_State lua_State;
#endif

// Only define API macros if not already defined
#ifndef LUA_API
#define LUA_API extern
#endif

#ifndef LUALIB_API
#define LUALIB_API extern
#endif

#ifndef LUA_PRINTF_ATTR
#define LUA_PRINTF_ATTR(fmt, args)
#endif

#ifndef l_noret
#define l_noret void
#endif

// Define the registry structure for lfs only if not already defined
#ifndef luaL_Reg
struct lfs_RegStruct {
    const char *name;
    int (*func)(lua_State *L);
};
typedef struct lfs_RegStruct luaL_Reg;
#endif

// Forward declare our implementation functions
#ifndef lua_pcall_impl_defined
#define lua_pcall_impl_defined
extern int lua_pcall_impl(lua_State* L, int nargs, int nresults, int errfunc);
extern void luaL_error_impl(lua_State* L, const char* fmt, ...);
extern void luaL_typeerrorL(lua_State* L, int narg, const char* tname);
extern void luaL_argerrorL(lua_State* L, int narg, const char* extramsg);
#endif

// Conditionally redefine problematic functions only if not already defined
#ifndef lua_pcall
#define lua_pcall lua_pcall_impl
#endif

#ifndef luaL_error
#define luaL_error luaL_error_impl
#endif

#ifndef luaL_typeerror
#define luaL_typeerror(L, narg, tname) luaL_typeerrorL(L, narg, tname)
#endif

#ifndef luaL_argerror
#define luaL_argerror(L, narg, extramsg) luaL_argerrorL(L, narg, extramsg)
#endif

// Ensure core Lua constants are defined only if not already defined
#ifndef LUA_REGISTRYINDEX
#define LUA_REGISTRYINDEX (-10000)
#endif

#ifndef LUA_ENVIRONINDEX
#define LUA_ENVIRONINDEX (-10001)
#endif

#ifndef LUA_GLOBALSINDEX
#define LUA_GLOBALSINDEX (-10002)
#endif

// Provide type constants only if not already defined
#ifndef LUA_TNONE
#define LUA_TNONE (-1)
#endif

#ifndef LUA_TNIL
#define LUA_TNIL 0
#endif

#ifndef LUA_TBOOLEAN
#define LUA_TBOOLEAN 1
#endif

#ifndef LUA_TLIGHTUSERDATA
#define LUA_TLIGHTUSERDATA 2
#endif

#ifndef LUA_TNUMBER
#define LUA_TNUMBER 3
#endif

// Don't define these macros if they're already defined by Lua
#ifndef lua_isnumber
#define lua_isnumber(L,n) (1)
#endif

#ifndef lua_isstring
#define lua_isstring(L,n) (1)
#endif

#ifndef lua_isnil
#define lua_isnil(L,n) (0)
#endif

#ifndef lua_tostring
#define lua_tostring(L,i) "dummy_string"
#endif

#ifndef lua_pushinteger
#define lua_pushinteger(L,n) lua_pushnumber((L), (n))
#endif

#ifndef lua_pop
#define lua_pop(L,n) lua_settop(L, -(n)-1)
#endif
