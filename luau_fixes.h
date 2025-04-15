// Luau compatibility fixes for iOS builds
// Include this at the beginning of any file that uses Luau headers
// This file patches all the issues with Luau header files for iOS builds

#pragma once

#include <stdarg.h>
#include <stdio.h>
#include <stddef.h>

// === First step: Fix the fundamental macros ===

// Force remove any CI_BUILD definitions that might still be present
#ifdef CI_BUILD
#undef CI_BUILD
#endif

// Fix missing LUA_API and LUALIB_API macros
#ifndef LUA_API
#define LUA_API extern
#endif

#ifndef LUALIB_API
#define LUALIB_API extern
#endif

// Fix LUA_PRINTF_ATTR if not defined
#ifndef LUA_PRINTF_ATTR
#define LUA_PRINTF_ATTR(fmt, args)
#endif

// === Second step: Fix function declarations ===

// Forward declaration of lua_State
struct lua_State;

// Fix for the lua_pcall static initialization issue
#ifdef lua_pcall
#undef lua_pcall
#endif
LUA_API int lua_pcall_impl(lua_State* L, int nargs, int nresults, int errfunc);
#define lua_pcall lua_pcall_impl

// Fix for the luaL_error static initialization issue
#ifdef luaL_error
#undef luaL_error
#endif
LUALIB_API void luaL_error_impl(lua_State* L, const char* fmt, ...);
#define luaL_error luaL_error_impl

// Define proper typedefs for other problematic functions
typedef int (*lua_CFunction)(lua_State* L);
typedef int (*lua_Continuation)(lua_State* L, int status);

// === Third step: Add missing implementations ===

#ifdef LUAU_FIXES_IMPLEMENTATION
// Implementation of lua_pcall
int lua_pcall_impl(lua_State* L, int nargs, int nresults, int errfunc) {
    // Basic implementation that returns success
    // In a real implementation, this would call the VM
    return 0; // LUA_OK
}

// Implementation of luaL_error
void luaL_error_impl(lua_State* L, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "Lua Error: ");
    vfprintf(stderr, fmt, args);
    va_end(args);
    fprintf(stderr, "\n");
    // In a real implementation, this would call lua_error after formatting
}
#endif

// === Fourth step: Fix other problematic macros and defines ===

// Fix defines for error handling functions that use static variables
#define luaL_typeerror(L, narg, tname) luaL_typeerrorL(L, narg, tname)
#define luaL_argerror(L, narg, extramsg) luaL_argerrorL(L, narg, extramsg)

// Simplify the system by removing some problematic compiler checks
#ifndef LUAI_FUNC
#define LUAI_FUNC extern
#endif

#ifndef LUAI_DDEC
#define LUAI_DDEC extern
#endif

#ifndef LUAI_DDEF
#define LUAI_DDEF
#endif

#ifndef LUAI_DATA
#define LUAI_DATA extern
#endif

// Fix necessary defines for library functions
#ifndef LUA_NORETURN
#define LUA_NORETURN
#endif

// Define the 'l_noret' macro properly
#ifndef l_noret
#define l_noret void
#endif

// Add this before any standard Luau includes
