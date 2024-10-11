#include <lua.hpp>
#include <lauxlib.h>
#include <lualib.h>
#include "lfs.h"        // LuaFileSystem for file handling
#include <sqlite3.h>    // SQLite for database operations
#include <json/json.h>  // JSON parsing
#include <zip.h>        // LibZip for compressed files handling

extern "C" int luaopen_mylibrary(lua_State *L) {
    // Load LuaFileSystem
    luaL_requiref(L, "lfs", luaopen_lfs, 1);
    lua_pop(L, 1); // Remove the library from the stack

    // Additional library setup for Roblox executor (if needed)
    
    // Run main Lua script
    luaL_dostring(L, "require('main.lua')");
    
    return 0;
}

int main(int argc, char** argv) {
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);

    // Register your custom Lua library
    luaopen_mylibrary(L);

    // Execute Roblox-related Lua code or executor script
    luaL_dofile(L, "executor.lua");

    lua_close(L);
    return 0;
}
