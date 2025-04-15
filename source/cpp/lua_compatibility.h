// lua_compatibility.h - Essential macros for Lua compatibility
#pragma once

// Define essential compatibility macros for Lua/Luau headers
// These MUST be defined before including any Lua headers

#ifndef LUA_API
#define LUA_API extern
#endif

#ifndef LUALIB_API 
#define LUALIB_API extern
#endif

#ifndef LUA_PRINTF_ATTR
#define LUA_PRINTF_ATTR(fmt, args)
#endif

// LUA_NORETURN definition based on compiler
#ifndef LUA_NORETURN
#ifdef __GNUC__
#define LUA_NORETURN __attribute__((__noreturn__))
#elif defined(_MSC_VER)
#define LUA_NORETURN __declspec(noreturn)
#else
#define LUA_NORETURN
#endif
#endif

// l_noret must be defined without the LUA_NORETURN suffix first
// The Lua headers will redefine it with the suffix
#ifndef l_noret
#define l_noret void
#endif

// Additional compatibility macros
#ifndef lua_check
#define lua_check(e) ((void)0)
#endif

#ifndef luai_apicheck
#define luai_apicheck(L, e) lua_check(e)
#endif
