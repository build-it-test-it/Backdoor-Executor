extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <stdlib.h> // For getenv
}

#include <iostream>

// Function to retrieve the app name from an environment variable
std::string getAppName() {
    const char* appName = getenv("APP_NAME"); // Replace with your method to get app name
    return appName ? std::string(appName) : "DefaultAppName"; // Fallback name
}

// Function to load and execute the main Lua script
extern "C" int luaopen_mylibrary(lua_State* L) {
    // Load the LuaFileSystem library
    luaL_requiref(L, "lfs", luaopen_lfs, 1); 
    lua_pop(L, 1);  // Remove library from the stack

    // Get the app name
    std::string appName = getAppName();

    // Push the app name to Lua
    lua_pushstring(L, appName.c_str());
    lua_setglobal(L, "appName"); // Make it accessible as a global variable in Lua

    if (luaL_dofile(L, "source/main.lua") != LUA_OK) { // Load and run the Lua script
        const char* error = lua_tostring(L, -1);
        lua_pushfstring(L, "Error: %s", error); // Push the error message to Lua
        return 1; // Return the error message
    }
    return 0; // Return success
}
