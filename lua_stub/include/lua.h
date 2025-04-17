// Minimal stub version of lua.h
#ifndef LUA_H
#define LUA_H

#include <stddef.h>

#define LUA_API extern
#define LUALIB_API extern

typedef struct lua_State lua_State;
typedef int (*lua_CFunction)(lua_State* L);

#define LUA_TNONE           (-1)
#define LUA_TNIL            0
#define LUA_TBOOLEAN        1
#define LUA_TLIGHTUSERDATA  2
#define LUA_TNUMBER         3
#define LUA_TSTRING         4
#define LUA_TTABLE          5
#define LUA_TFUNCTION       6
#define LUA_TUSERDATA       7
#define LUA_TTHREAD         8
#define LUA_TVECTOR         9

#define LUA_MULTRET (-1)
#define LUA_REGISTRYINDEX (-10000)
#define LUA_ENVIRONINDEX (-10001)
#define LUA_GLOBALSINDEX (-10002)

LUA_API lua_State* lua_newstate(void* f, void* ud);
LUA_API void lua_close(lua_State* L);
LUA_API int lua_gettop(lua_State* L);
LUA_API void lua_settop(lua_State* L, int idx);
LUA_API void lua_pushvalue(lua_State* L, int idx);
LUA_API void lua_pushnil(lua_State* L);
LUA_API void lua_pushnumber(lua_State* L, double n);
LUA_API void lua_pushstring(lua_State* L, const char* s);
LUA_API void lua_pushboolean(lua_State* L, int b);
LUA_API int lua_pcall(lua_State* L, int nargs, int nresults, int errfunc);

#endif /* LUA_H */
