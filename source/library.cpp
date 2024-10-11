#include <lua.hpp>         // Lua core
#include <lauxlib.h>      // Lua auxiliary functions
#include <lualib.h>       // Lua standard libraries
#include <lfs.h>          // LuaFileSystem for file handling
#include <ssl.h>          // SSL library for encryption

extern "C" int luaopen_mylibrary(lua_State *L) {
    // Load LuaFileSystem
    luaL_requiref(L, "lfs", luaopen_lfs, 1);
    lua_pop(L, 1); // Remove the library from the stack

    // Run main Luau script
    luaL_dostring(L, "require('main.luau')");
    
    return 0;
}

int main(int argc, char** argv) {
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);

    // Register your custom Lua library
    luaopen_mylibrary(L);

    // Execute Roblox-related Lua code or executor script
    luaL_dofile(L, "executor.luau");

    lua_close(L);
    return 0;
}
