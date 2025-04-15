// Luau compatibility fixes header for iOS builds
// This file provides compatibility functions for Luau implementation
#pragma once

#ifndef LUAU_FIXES_H
#define LUAU_FIXES_H

// Define essential macros before including Lua headers
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

// l_noret is used to mark functions that don't return
#ifndef l_noret
#define l_noret void
#endif

// Include the actual lua header after macros are defined
#include "luau/lua.h"
#include "luau/lualib.h"

#ifdef __cplusplus
extern "C" {
#endif

// Function declarations
int lua_pcall_impl(lua_State* L, int nargs, int nresults, int errfunc);
void luaL_error_impl(lua_State* L, const char* fmt, ...);
l_noret luaL_typeerrorL(lua_State* L, int narg, const char* tname);
l_noret luaL_argerrorL(lua_State* L, int narg, const char* extramsg);
const char* lua_pushfstringL(lua_State* L, const char* fmt, ...);
void* lua_newuserdatatagged(lua_State* L, size_t sz, int tag);

// Create compatibility macros for any missing functions
#ifndef LUAU_FIXES_IMPLEMENTATION
#define lua_pcall lua_pcall_impl
#define luaL_error luaL_error_impl
#endif

#ifdef __cplusplus
}
#endif

#endif // LUAU_FIXES_H
