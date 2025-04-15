// Lua isolation header - Include this when you need Lua functionality
#pragma once

// This header safely includes all Lua headers and prevents conflicts with Objective-C

// Guard against including both Lua and Objective-C in the same translation unit
#ifdef __OBJC__
    #error "lua_isolation.h should not be included in Objective-C++ files. Use the bridge interface instead."
#endif

// Include real Lua headers directly
#include "../luau/lua.h"
#include "../luau/lualib.h"
#include "../luau/luaconf.h"
#include "../luau/lauxlib.h"
#include "../luau/lstate.h"

// Export the important types and functions that might be needed by the bridge
namespace LuaBridge {
    // Use the actual lua_State type
    using LuaState = lua_State;
    
    // Functions to safely execute Lua code without exposing Lua types
    bool ExecuteScript(lua_State* L, const char* script, const char* chunkname = "");
    const char* GetLastError(lua_State* L);
    
    // Memory management
    void CollectGarbage(lua_State* L);
    
    // Create a safely wrapped C function to expose to the bridge
    void RegisterFunction(lua_State* L, const char* name, int (*func)(lua_State*));
}
