// Essential Lua macro definitions needed by lua.h and lualib.h
#pragma once

// API macros
#ifndef LUA_API
#define LUA_API extern
#endif

#ifndef LUALIB_API
#define LUALIB_API extern
#endif

// Only define LUA_NORETURN if it's not already defined
#ifndef LUA_NORETURN
#define LUA_NORETURN
#endif

// Important: DO NOT define l_noret here, let lua.h define it properly

// Format attributes
#ifndef LUA_PRINTF_ATTR
#define LUA_PRINTF_ATTR(fmt, args)
#endif
