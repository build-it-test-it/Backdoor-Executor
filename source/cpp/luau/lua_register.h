// Compatibility header for lua_register and related functions
// This provides compatibility layers between standard Lua and Luau

#pragma once

#include "lua.h"
#include "lualib.h"

// Define lua_pushcfunction with debug name compatibility
// Standard Lua: lua_pushcfunction(L, f)
// Luau: lua_pushcfunction(L, f, debugname)
#ifndef lua_pushcfunction_compat
    #define lua_pushcfunction_compat(L, f, debugname) lua_pushcclosure(L, f, debugname, 0, NULL)
    // Override the standard macro if it exists, otherwise this will be used
    #ifdef lua_pushcfunction
        #undef lua_pushcfunction
    #endif
    #define lua_pushcfunction lua_pushcfunction_compat
#endif

// Emulate the standard Lua lua_register macro by setting a global function
// In standard Lua, lua_register(L,n,f) is a macro for (lua_pushcfunction(L, f), lua_setglobal(L, n))
inline void lua_register_compat(lua_State* L, const char* name, lua_CFunction func) {
    // Push the C function with a debug name (required for Luau)
    lua_pushcfunction(L, func, name);
    // Set it as a global with the given name
    lua_setglobal(L, name);
}

// Define lua_register as our compatibility function
#define lua_register lua_register_compat
