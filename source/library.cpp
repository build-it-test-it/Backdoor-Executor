#include <lua.hpp>
#include <lauxlib.h>
#include <lualib.h>
#include "lfs.h"  // Include the LuaFileSystem header

extern "C" int luaopen_mylibrary(lua_State *L) {
    luaL_requiref(L, "lfs", luaopen_lfs, 1); // Make sure luaopen_lfs is used correctly
    lua_pop(L, 1); // Remove the library from the stack

    // Your other Lua bindings go here

    return 0; // Return the number of results
}
