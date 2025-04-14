// Compatibility header for Luau to provide lauxlib.h compatibility

#ifndef FIND_LAUXLIB_H
#define FIND_LAUXLIB_H

#include "cpp/luau/lua.h"

// Define the luaL_Reg struct used by LuaFileSystem
typedef struct luaL_Reg {
    const char* name;
    lua_CFunction func;
} luaL_Reg;

// Forward declarations of functions needed by lfs.c
void luaL_checktype(lua_State* L, int narg, int t);
void* luaL_checkudata(lua_State* L, int ud, const char* tname);
int luaL_newmetatable(lua_State* L, const char* tname);
int luaL_getmetafield(lua_State* L, int obj, const char* e);
int luaL_callmeta(lua_State* L, int obj, const char* e);
void luaL_register(lua_State* L, const char* libname, const luaL_Reg* l);
int luaL_error(lua_State* L, const char* fmt, ...);

// Version compatibility
#define LUA_VERSION_NUM 501  // Pretend we're using Lua 5.1

#endif // FIND_LAUXLIB_H
