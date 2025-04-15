// Stub lualib.h with complete functionality for lfs.c to compile
#pragma once

#include "lua.h"

// Registry structure
typedef struct luaL_Reg {
    const char *name;
    lua_CFunction func;
} luaL_Reg;

// Basic API
LUALIB_API void luaL_register(lua_State* L, const char* libname, const luaL_Reg* l);
LUALIB_API const char* luaL_typename(lua_State* L, int idx);
LUALIB_API const char* luaL_checklstring(lua_State* L, int numArg, size_t* l);
LUALIB_API const char* luaL_checkstring(lua_State* L, int numArg); // Added missing function
LUALIB_API double luaL_checknumber(lua_State* L, int numArg);
LUALIB_API int luaL_checkboolean(lua_State* L, int narg);
LUALIB_API int luaL_checkinteger(lua_State* L, int numArg);
LUALIB_API const char* luaL_optlstring(lua_State* L, int numArg, const char* def, size_t* l);
LUALIB_API double luaL_optnumber(lua_State* L, int nArg, double def);
LUALIB_API int luaL_optinteger(lua_State* L, int nArg, int def);
LUALIB_API int luaL_optboolean(lua_State* L, int nArg, int def);
LUALIB_API void luaL_error(lua_State* L, const char* fmt, ...);
LUALIB_API void luaL_typeerror(lua_State* L, int narg, const char* tname);
LUALIB_API void luaL_argerror(lua_State* L, int narg, const char* extramsg);
LUALIB_API void* luaL_checkudata(lua_State* L, int ud, const char* tname); // Added missing function
LUALIB_API int luaL_checkoption(lua_State* L, int narg, const char* def, const char* const lst[]); // Added missing function
LUALIB_API void luaL_getmetatable(lua_State* L, const char* tname); // Added missing function
LUALIB_API int luaL_getmetafield(lua_State* L, int obj, const char* e);

// Standard library open functions
LUALIB_API void luaL_openlibs(lua_State* L);
