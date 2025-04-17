// Minimal stub version of lualib.h
#ifndef LUALIB_H
#define LUALIB_H

#include "lua.h"

LUALIB_API int luaopen_base(lua_State* L);
LUALIB_API int luaopen_table(lua_State* L);
LUALIB_API int luaopen_string(lua_State* L);
LUALIB_API int luaopen_math(lua_State* L);
LUALIB_API int luaopen_debug(lua_State* L);
LUALIB_API int luaopen_package(lua_State* L);
LUALIB_API int luaopen_coroutine(lua_State* L);
LUALIB_API int luaopen_utf8(lua_State* L);
LUALIB_API int luaopen_buffer(lua_State* L);
LUALIB_API int luaopen_vector(lua_State* L);

LUALIB_API void luaL_openlibs(lua_State* L);

#endif /* LUALIB_H */
