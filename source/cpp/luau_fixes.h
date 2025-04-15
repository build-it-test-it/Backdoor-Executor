// Luau compatibility fixes header for iOS builds
// This file provides compatibility functions for Luau implementation
#pragma once

#ifndef LUAU_FIXES_H
#define LUAU_FIXES_H

// First we define typedefs to avoid circular dependencies
struct lua_State;
typedef int (*lua_CFunction)(struct lua_State* L);

// Define LUA_NORETURN properly (it's used in l_noret definition)
#ifdef __GNUC__
#define LUA_NORETURN __attribute__((__noreturn__))
#elif defined(_MSC_VER)
#define LUA_NORETURN __declspec(noreturn)
#else
#define LUA_NORETURN
#endif

// Define essential macros needed by Lua headers
#ifndef LUA_API
#define LUA_API extern
#endif

#ifndef LUALIB_API 
#define LUALIB_API extern
#endif

// Define special macros needed by Luau headers
#ifndef LUA_PRINTF_ATTR
#define LUA_PRINTF_ATTR(fmt, args)
#endif

// Include the lua headers after macros are defined
#include "luau/lua.h"
#include "luau/lualib.h"

#ifdef __cplusplus
extern "C" {
#endif

// Function implementations to be provided in luau_fixes.cpp
int lua_pcall_impl(lua_State* L, int nargs, int nresults, int errfunc);
void luaL_error_impl(lua_State* L, const char* fmt, ...);
const char* lua_pushfstringL(lua_State* L, const char* fmt, ...);
void* lua_newuserdatatagged(lua_State* L, size_t sz, int tag);

// Provide implementations for the error functions
// We should prefer to forward declare these rather than redefine them
#ifndef LUAU_FIXES_IMPLEMENTATION
#define lua_pcall lua_pcall_impl
#define luaL_error luaL_error_impl
#endif

// Custom implementations - implemented in luau_fixes.cpp
void fixLuaFunction_typeerror(lua_State* L, int narg, const char* tname);
void fixLuaFunction_argerror(lua_State* L, int narg, const char* extramsg);

// Override the existing macro to call our implementation
#ifndef LUAU_FIXES_IMPLEMENTATION
#undef luaL_typeerror
#define luaL_typeerror(L, narg, tname) fixLuaFunction_typeerror(L, narg, tname)

#undef luaL_argerror
#define luaL_argerror(L, narg, extramsg) fixLuaFunction_argerror(L, narg, extramsg)
#endif

#ifdef __cplusplus
}
#endif

#endif // LUAU_FIXES_H
