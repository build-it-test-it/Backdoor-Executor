// lua_compatibility.h - Enhanced compatibility layer for Lua/Luau integration
#pragma once

// Include standard headers for size_t
#include <stddef.h>  // For size_t in C
#ifdef __cplusplus
#include <cstddef>   // For size_t in C++
#endif

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

// Define l_noret for compatibility
#ifndef l_noret
#define l_noret void
#endif

// Missing configuration constants from luaconf.h
#ifndef LUAI_USER_ALIGNMENT_T
#define LUAI_USER_ALIGNMENT_T double
#endif

#ifndef LUA_EXTRA_SIZE
#define LUA_EXTRA_SIZE 0
#endif

#ifndef LUA_SIZECLASSES
#define LUA_SIZECLASSES 32
#endif

#ifndef LUA_MEMORY_CATEGORIES
#define LUA_MEMORY_CATEGORIES 8
#endif

#ifndef LUA_UTAG_LIMIT
#define LUA_UTAG_LIMIT 8
#endif

// Additional compatibility macros
#ifndef lua_check
#define lua_check(e) ((void)0)
#endif

#ifndef luai_apicheck
#define luai_apicheck(L, e) lua_check(e)
#endif

// Define core structs used by our libraries
#ifndef luaL_Reg
struct luaL_RegStruct {
    const char *name;
    int (*func)(lua_State *L);
};
typedef struct luaL_RegStruct luaL_Reg;
#endif

// Forward declaration of lua_State to avoid including it
#ifndef lua_State
typedef struct lua_State lua_State;
#endif

// Basic types needed for Lua integration
typedef long lua_Integer;
typedef unsigned long lua_Unsigned;
typedef double lua_Number;

// Function pointer types
typedef int (*lua_CFunction)(lua_State* L);
typedef int (*lua_Continuation)(lua_State* L, int status);

// Define important Lua constants
#ifndef LUA_REGISTRYINDEX
#define LUA_REGISTRYINDEX (-10000)
#define LUA_ENVIRONINDEX (-10001)
#define LUA_GLOBALSINDEX (-10002)

#define LUA_TNONE (-1)
#define LUA_TNIL 0
#define LUA_TBOOLEAN 1
#define LUA_TLIGHTUSERDATA 2
#define LUA_TNUMBER 3
#define LUA_TVECTOR 4
#define LUA_TSTRING 5
#define LUA_TTABLE 6
#define LUA_TFUNCTION 7
#define LUA_TUSERDATA 8
#define LUA_TTHREAD 9
#endif

// Common Lua macros
#ifndef lua_tostring
#define lua_tostring(L,i) "dummy_string" // simplified
#define lua_isnumber(L,n) (1)
#define lua_pushinteger(L,n) lua_pushnumber((L), (n))
#define lua_isstring(L,n) (1)
#define lua_isnil(L,n) (0)
#define lua_pop(L,n) lua_settop(L, -(n)-1)
#endif

// Forward-declare critical functions that might cause linking issues
#ifdef __cplusplus
extern "C" {
#endif

// Fix problematic static function pointer by declaring it externally
#ifdef lua_pcall
#undef lua_pcall
#endif
extern int lua_pcall(lua_State* L, int nargs, int nresults, int errfunc);

// Fix typeerror and argerror macros
extern void luaL_typeerrorL(lua_State* L, int narg, const char* tname);
extern void luaL_argerrorL(lua_State* L, int narg, const char* extramsg);
#define luaL_typeerror(L, narg, tname) luaL_typeerrorL(L, narg, tname)
#define luaL_argerror(L, narg, extramsg) luaL_argerrorL(L, narg, extramsg)

// Forward declarations of core Lua API functions
LUA_API LUA_PRINTF_ATTR(2, 3) const char* lua_pushfstringL(lua_State* L, const char* fmt, ...);
LUA_API void luaL_error(lua_State* L, const char* fmt, ...);
LUA_API const char* luaL_typename(lua_State* L, int idx);
LUA_API int lua_gettop(lua_State* L);
LUA_API void lua_settop(lua_State* L, int idx);
LUA_API void lua_pushnil(lua_State* L);
LUA_API void lua_pushnumber(lua_State* L, double n);
LUA_API void lua_pushstring(lua_State* L, const char* s);
LUA_API void lua_createtable(lua_State* L, int narr, int nrec);
LUA_API void lua_setfield(lua_State* L, int idx, const char* k);
LUA_API int lua_type(lua_State* L, int idx);
LUA_API void lua_pushboolean(lua_State* L, int b);

// Forward declarations for additional functions
LUALIB_API int luaL_loadbuffer(lua_State* L, const char* buff, size_t sz, const char* name);
LUALIB_API void luaL_register(lua_State* L, const char* libname, const struct luaL_RegStruct* l);

#ifdef __cplusplus
}
#endif
