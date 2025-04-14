// Luau auxiliary compatibility functions for standard Lua API
// This file provides functions and macros that exist in standard Lua but not in Luau

#pragma once

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#ifdef __cplusplus
extern "C" {
#endif

// Implement luaL_dostring if not already defined
// In standard Lua, it loads and executes a string
#ifndef luaL_dostring
inline int luaL_dostring(lua_State* L, const char* str) {
    // Use the standard luau_load function
    if (luau_load(L, "chunk", str, strlen(str), 0) != 0)
        return 1;  // Error in compilation
    
    // Execute the chunk
    return lua_pcall(L, 0, 0, 0);  // Call with 0 args and expect 0 results
}
#endif

// Implement luaL_requiref if not already defined
// In standard Lua 5.2+, it registers a module
#ifndef luaL_requiref
inline void luaL_requiref(lua_State* L, const char* modname, lua_CFunction openf, int glb) {
    // Push the C function that opens the library
    lua_pushcfunction(L, openf, modname);
    // Call it with the module name as argument
    lua_pushstring(L, modname);
    lua_call(L, 1, 1);
    
    // Register it in package.loaded
    lua_getglobal(L, "package");
    if (lua_type(L, -1) == LUA_TTABLE) {
        lua_getfield(L, -1, "loaded");
        if (lua_type(L, -1) == LUA_TTABLE) {
            lua_pushvalue(L, -3);  // The module result
            lua_setfield(L, -2, modname);  // package.loaded[modname] = module
        }
        lua_pop(L, 1);  // Pop package.loaded
    }
    lua_pop(L, 1);  // Pop package
    
    // If glb is true, register it as a global
    if (glb) {
        lua_pushvalue(L, -1);  // The module result
        lua_setglobal(L, modname);  // _G[modname] = module
    }
}
#endif

// Fix lua_pushcfunction to work correctly with Luau
// We directly use lua_pushcclosurek to avoid macro expansion issues
#ifdef lua_pushcfunction
#undef lua_pushcfunction
#endif
inline void lua_pushcfunction_direct(lua_State* L, lua_CFunction fn, const char* debugname) {
    lua_pushcclosurek(L, fn, debugname, 0, NULL);
}
#define lua_pushcfunction lua_pushcfunction_direct

#ifdef __cplusplus
}
#endif
