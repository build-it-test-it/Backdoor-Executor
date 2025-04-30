/**
 * @file lua_defs.h
 * @brief Minimal stub definitions for Lua compatibility
 */
#pragma once

// Include standard headers for types
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// Forward declarations for Lua types
typedef struct lua_State lua_State;

// Lua type definitions
typedef int lua_CFunction(lua_State* L);
typedef void* (*lua_Alloc)(void* ud, void* ptr, size_t osize, size_t nsize);
typedef intptr_t lua_Integer;
typedef double lua_Number;

// Define some common constants
#define LUA_MULTRET    (-1)
#define LUA_REGISTRYINDEX   (-10000)
#define LUA_ENVIRONINDEX    (-10001)
#define LUA_GLOBALSINDEX    (-10002)

// Define Lua memory allocation tags
#define LUA_TNIL       0
#define LUA_TBOOLEAN   1
#define LUA_TLIGHTUSERDATA 2
#define LUA_TNUMBER    3
#define LUA_TSTRING    4
#define LUA_TTABLE     5
#define LUA_TFUNCTION  6
#define LUA_TUSERDATA  7
#define LUA_TTHREAD    8
#define LUA_NUMTAGS    9

#ifdef __cplusplus
}
#endif
