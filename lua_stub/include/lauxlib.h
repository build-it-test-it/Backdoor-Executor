// Minimal stub version of lauxlib.h
#ifndef LAUXLIB_H
#define LAUXLIB_H

#include "lua.h"

#define LUALIB_API	LUA_API

typedef struct luaL_Reg {
  const char *name;
  lua_CFunction func;
} luaL_Reg;

LUALIB_API void luaL_register(lua_State *L, const char *libname, const luaL_Reg *l);
LUALIB_API void *luaL_checkudata(lua_State *L, int ud, const char *tname);
LUALIB_API void luaL_error(lua_State *L, const char *fmt, ...);
LUALIB_API int luaL_newmetatable(lua_State *L, const char *tname);
LUALIB_API const char *luaL_checkstring(lua_State *L, int numArg);
LUALIB_API int luaL_checkinteger(lua_State *L, int numArg);
LUALIB_API lua_State *luaL_newstate(void);
LUALIB_API void luaL_checktype(lua_State *L, int narg, int t);

#endif /* LAUXLIB_H */
