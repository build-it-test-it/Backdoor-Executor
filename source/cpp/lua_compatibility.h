// lua_compatibility.h - Essential macros for Lua compatibility
#pragma once

// Define essential compatibility macros for Lua/Luau headers
// These MUST be defined before including any Lua headers

// Main API export macros
#ifndef LUA_API
#define LUA_API extern
#endif

#ifndef LUALIB_API 
#define LUALIB_API extern
#endif

// Format attribute macro needs to take format and args, but we define it empty for non-GCC
#ifndef LUA_PRINTF_ATTR
#ifdef __GNUC__
#define LUA_PRINTF_ATTR(fmt,args) __attribute__((format(printf, fmt, args)))
#else
#define LUA_PRINTF_ATTR(fmt,args)
#endif
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

// Don't define l_noret at all - let Lua header define it completely
#ifdef l_noret
#undef l_noret
#endif

// Additional compatibility macros
#ifndef lua_check
#define lua_check(e) ((void)0)
#endif

#ifndef luai_apicheck
#define luai_apicheck(L, e) lua_check(e)
#endif

// Forward declaration of lua_State to avoid including it
#ifndef lua_State
typedef struct lua_State lua_State;
#endif

// Forward-declare critical functions that might cause linking issues
#ifdef __cplusplus
extern "C" {
#endif

// Forward declaration of string formatting function - must match exactly the declaration in lua.h
LUA_API LUA_PRINTF_ATTR(2, 3) const char* lua_pushfstringL(lua_State* L, const char* fmt, ...);

#ifdef __cplusplus
}
#endif
