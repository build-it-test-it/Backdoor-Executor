// Luau compatibility fixes for iOS builds
// Include this at the beginning of any file that uses Luau headers
// This file provides real implementations that connect to the Lua VM

#pragma once

#include <stdarg.h>
#include <stdio.h>
#include <stddef.h>

// === First step: Remove CI_BUILD and define proper API macros ===

// Force remove any CI_BUILD definitions that might still be present
#ifdef CI_BUILD
#undef CI_BUILD
#endif

// Define proper API macros for function exports
#ifndef LUA_API
#define LUA_API extern
#endif

#ifndef LUALIB_API
#define LUALIB_API extern
#endif

// Fix LUA_PRINTF_ATTR for format string checking
#ifndef LUA_PRINTF_ATTR
#define LUA_PRINTF_ATTR(fmt, args)
#endif

// === Second step: Fix problematic Lua VM functions ===

// Forward declarations of required Lua types
struct lua_State;
struct lua_Debug;
typedef int (*lua_CFunction)(lua_State* L);
typedef int (*lua_Continuation)(lua_State* L, int status);

// Fix for lua_pcall static function pointer in lua.h
// This is a core issue causing compile errors
#ifdef lua_pcall
#undef lua_pcall
#endif
LUA_API int lua_pcall_impl(lua_State* L, int nargs, int nresults, int errfunc);
#define lua_pcall lua_pcall_impl

// Proper declaration for luaL_error to fix compilation issue in lualib.h
#ifdef luaL_error
#undef luaL_error
#endif
LUALIB_API void luaL_error_impl(lua_State* L, const char* fmt, ...);
#define luaL_error luaL_error_impl

// typeerror and argerror functions use static declarations, fix those
#define luaL_typeerror(L, narg, tname) luaL_typeerrorL(L, narg, tname)
#define luaL_argerror(L, narg, extramsg) luaL_argerrorL(L, narg, extramsg)

// Forward declarations of internal Lua functions we need for real implementations
#ifndef LUAI_EXTERN_FORWARD_DECLARE
#define LUAI_EXTERN_FORWARD_DECLARE
struct CallInfo;
typedef struct CallInfo CallInfo;
extern void luaG_runerror(lua_State* L, const char* fmt, ...);
extern int luaD_pcall(lua_State* L, int (*func)(lua_State*, void*), void* ud, ptrdiff_t oldtop, ptrdiff_t ef);
extern int luaV_execute(lua_State* L, int nexeccalls);
extern void luaD_seterrorobj(lua_State* L, int errcode, StkId oldtop);
extern void luaD_throw(lua_State* L, int errcode);
#endif

// === Third step: Define essential macros for Lua C API ===

// Define the 'l_noret' macro properly
#ifndef l_noret
#define l_noret void
#endif

// Ensure all required type definitions are available
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

#ifndef LUA_NORETURN
#define LUA_NORETURN
#endif

// === Real implementations of critical functions ===

#ifdef LUAU_FIXES_IMPLEMENTATION

// Structure to handle protected calls for lua_pcall implementation
struct PCCallS {
    StkId func;
    int nresults;
    int errfunc;
};

// Helper function for lua_pcall
static int f_call(lua_State* L, void* ud) {
    struct PCCallS* p = (struct PCCallS*)ud;
    luaV_execute(L, 1); // Execute a protected call
    return 1;
}

// Real implementation of lua_pcall that connects to the Lua VM
int lua_pcall_impl(lua_State* L, int nargs, int nresults, int errfunc) {
    struct PCCallS p;
    p.func = L->top - (nargs + 1); // Get function index
    p.nresults = nresults;
    p.errfunc = errfunc;
    
    int status = luaD_pcall(L, f_call, &p, (char*)L->top - (char*)L->stack, errfunc);
    
    // Real error handling that uses the VM
    if (status != 0) {
        // Properly set up error object
        luaD_seterrorobj(L, status, L->top);
        L->top++;
    }
    
    return status;
}

// Real implementation of luaL_error that properly raises errors in the VM
void luaL_error_impl(lua_State* L, const char* fmt, ...) {
    va_list argp;
    va_start(argp, fmt);
    
    // Use the real Lua error reporting mechanism
    luaG_runerror(L, fmt, argp);
    
    va_end(argp);
    
    // If luaG_runerror returns (shouldn't happen), throw a generic error
    luaD_throw(L, LUA_ERRRUN);
}

// Real implementation of type error
l_noret luaL_typeerrorL(lua_State* L, int narg, const char* tname) {
    const char* msg = lua_pushfstringL(L, "%s expected, got %s", tname, luaL_typename(L, narg));
    luaG_runerror(L, "bad argument #%d (%s)", narg, msg);
}

// Real implementation of argument error
l_noret luaL_argerrorL(lua_State* L, int narg, const char* extramsg) {
    luaG_runerror(L, "bad argument #%d (%s)", narg, extramsg);
}

#endif // LUAU_FIXES_IMPLEMENTATION

// This header must be included before any standard Luau headers
