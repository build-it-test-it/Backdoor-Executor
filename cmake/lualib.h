/*
** Standard library header for Lua
** See Copyright Notice in lua.h
*/

#ifndef lualib_h
#define lualib_h

#include "lua.h"

/* Key to file-handle type */
#define LUA_FILEHANDLE          "FILE*"

#define LUA_COLIBNAME   "coroutine"
LUA_API int (luaopen_base) (lua_State *L);

#define LUA_TABLIBNAME  "table"
LUA_API int (luaopen_table) (lua_State *L);

#define LUA_IOLIBNAME   "io"
LUA_API int (luaopen_io) (lua_State *L);

#define LUA_OSLIBNAME   "os"
LUA_API int (luaopen_os) (lua_State *L);

#define LUA_STRLIBNAME  "string"
LUA_API int (luaopen_string) (lua_State *L);

#define LUA_MATHLIBNAME "math"
LUA_API int (luaopen_math) (lua_State *L);

#define LUA_DBLIBNAME   "debug"
LUA_API int (luaopen_debug) (lua_State *L);

#define LUA_LOADLIBNAME "package"
LUA_API int (luaopen_package) (lua_State *L);

/* open all previous libraries */
LUA_API void (luaL_openlibs) (lua_State *L);

#ifndef lua_assert
#define lua_assert(x)   ((void)0)
#endif

#endif
