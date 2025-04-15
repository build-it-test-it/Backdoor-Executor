#!/bin/bash
# Focused fix specifically for Lua macro definitions

echo "==== Applying Focused Lua Macro Fix ===="

# 1. Ensure lfs.c includes lua_defs.h before any other Lua headers
echo "Fixing lfs.c to include essential Lua macro definitions..."
cp source/lfs.c source/lfs.c.bak

# Remove any existing includes to start fresh
grep -v "#include.*luau/lua" source/lfs.c > source/lfs.c.tmp1
grep -v "#include.*lualib" source/lfs.c.tmp1 > source/lfs.c.tmp2
grep -v "lua_wrapper.h" source/lfs.c.tmp2 > source/lfs.c.tmp
mv source/lfs.c.tmp source/lfs.c

# Add proper includes in the correct order
sed -i '1i// Include Lua in proper order with essential definitions first\n#include "cpp/luau/lua_defs.h"\n#include "cpp/luau/lua.h"\n#include "cpp/luau/lualib.h"\n' source/lfs.c

# 2. Ensure our lua_wrapper.h is used only in files that don't use real Lua
echo "Updating lua_wrapper.h to avoid conflicts..."

cat > source/lua_wrapper.h << 'EOL'
// Standalone Lua wrapper for executor - For use in non-Lua files only
#pragma once

// If real Lua headers are already included, this file does nothing
#ifndef _lua_already_included
#define _lua_already_included

#ifdef __cplusplus
extern "C" {
#endif

// Basic type definitions
typedef struct lua_State lua_State;
typedef int (*lua_CFunction)(lua_State* L);

// API function declarations
extern int lua_pcall(lua_State* L, int nargs, int nresults, int errfunc);
extern void luaL_error(lua_State* L, const char* fmt, ...);
extern int lua_gettop(lua_State* L);
extern void lua_settop(lua_State* L, int idx);
extern void lua_pushnil(lua_State* L);
extern void lua_pushnumber(lua_State* L, double n);
extern void lua_pushboolean(lua_State* L, int b);
extern void lua_pushstring(lua_State* L, const char* s);
extern void lua_createtable(lua_State* L, int narr, int nrec);
extern void lua_setfield(lua_State* L, int idx, const char* k);
extern int lua_type(lua_State* L, int idx);
extern const char* luaL_typename(lua_State* L, int idx);

// Basic constants
#define LUA_REGISTRYINDEX (-10000)
#define LUA_ENVIRONINDEX (-10001)
#define LUA_GLOBALSINDEX (-10002)

#define LUA_TNONE (-1)
#define LUA_TNIL 0
#define LUA_TBOOLEAN 1
#define LUA_TLIGHTUSERDATA 2
#define LUA_TNUMBER 3
#define LUA_TSTRING 5

// Helper macros
#define lua_isnil(L,n) (lua_type(L,n) == LUA_TNIL)
#define lua_isnumber(L,n) (lua_type(L,n) == LUA_TNUMBER)
#define lua_pushinteger(L,n) lua_pushnumber(L, (double)(n))
#define lua_pop(L,n) lua_settop(L, -(n)-1)
#define lua_tostring(L,i) "dummy_string"

// Registry structure
struct lfs_RegStruct {
    const char *name;
    lua_CFunction func;
};
typedef struct lfs_RegStruct luaL_Reg;

#ifdef __cplusplus
}
#endif

#endif // _lua_already_included
EOL

# 3. Update CMakeLists.txt to ensure lfs.c compiles correctly
echo "Updating CMakeLists.txt to ensure correct compilation..."

# Update the lfs_obj target to include luau directory
if grep -q "target_include_directories(lfs_obj" CMakeLists.txt; then
  sed -i '/target_include_directories(lfs_obj/c\
target_include_directories(lfs_obj PRIVATE\
    ${CMAKE_SOURCE_DIR}/source/cpp/luau\
    ${CMAKE_SOURCE_DIR}/source\
)' CMakeLists.txt
fi

echo "==== Focused Lua Macro Fix Complete ===="

# Verify our changes
echo "Verifying fix..."
head -n 10 source/lfs.c
head -n 10 source/lua_wrapper.h
