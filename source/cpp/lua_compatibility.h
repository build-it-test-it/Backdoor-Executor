// lua_compatibility.h - Compatibility layer for Lua/Luau integration
// This is now a simplified wrapper around the real Lua headers
#pragma once

// Include standard headers
#include <stddef.h>
#include <stdarg.h>
#ifdef __cplusplus
#include <cstddef>
#include <cstdarg>
#endif

// Include the real Lua headers - these will be found in our installed location
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

// Compatibility for older Lua versions or Luau-specific features
#ifdef __cplusplus
extern "C" {
#endif

// Lua 5.4 doesn't have LUA_TVECTOR, add it for compatibility with Luau
#ifndef LUA_TVECTOR
#define LUA_TVECTOR 10
#endif

// Add some compatibility macros for older Lua versions if needed
#ifndef lua_pushcfunction
#define lua_pushcfunction(L, f) lua_pushcclosure(L, (f), 0)
#endif

// Luau-specific: Ensure we have new_lib defined for library creation
#ifndef new_lib
#define new_lib(L, l) (lua_createtable(L, 0, sizeof(l)/sizeof((l)[0]) - 1), luaL_setfuncs(L, l, 0))
#endif

// Some older Lua versions use deprecated functions
#if LUA_VERSION_NUM < 502
// For Lua 5.1 compatibility
#define luaL_setfuncs(L, l, nup) luaL_register(L, NULL, l)
#endif

// Helper for luaL_checkstring to work across Lua versions
#if !defined(luaL_checkstring) && !defined(LUAI_FUNC)
#define luaL_checkstring(L, n) luaL_checklstring(L, (n), NULL)
#endif

// For older Lua versions, provide lua_rawlen compatibility
#if LUA_VERSION_NUM < 502 && !defined(lua_rawlen)
#define lua_rawlen(L, i) lua_objlen(L, (i))
#endif

// Add compatibility with Lua 5.1's global table access
#if LUA_VERSION_NUM >= 502
// In Lua 5.2+, the globals are accessed through LUA_RIDX_GLOBALS
inline void lua_compat_getglobal(lua_State* L, const char* name) {
    lua_getglobal(L, name);
}
inline void lua_compat_setglobal(lua_State* L, const char* name) {
    lua_setglobal(L, name);
}
#else
// In Lua 5.1, the globals table is at LUA_GLOBALSINDEX
inline void lua_compat_getglobal(lua_State* L, const char* name) {
    lua_getfield(L, LUA_GLOBALSINDEX, name);
}
inline void lua_compat_setglobal(lua_State* L, const char* name) {
    lua_setfield(L, LUA_GLOBALSINDEX, name);
}
#define lua_getglobal(L, name) lua_compat_getglobal(L, name)
#define lua_setglobal(L, name) lua_compat_setglobal(L, name)
#endif

#ifdef __cplusplus
}
#endif
