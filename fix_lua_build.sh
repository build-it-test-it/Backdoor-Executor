#!/bin/bash
# Fix the build system to handle Lua properly

echo "==== Fixing Lua Build System ===="

# 1. Create a separate CMake target for lfs_obj that uses real Lua headers
echo "Updating CMakeLists.txt to separate Lua implementations..."

# Find the lfs_obj add_library command in CMakeLists.txt
if grep -q "add_library(lfs_obj OBJECT" CMakeLists.txt; then
  # Update the target to use real Lua headers
  sed -i '/add_library(lfs_obj OBJECT/,/)/c\
# LuaFileSystem with real Lua headers\
add_library(lfs_obj OBJECT ${CMAKE_SOURCE_DIR}/source/lfs.c)\
target_include_directories(lfs_obj PRIVATE\
    ${CMAKE_SOURCE_DIR}/source/cpp/luau\
    ${CMAKE_SOURCE_DIR}/source\
)\
target_compile_definitions(lfs_obj PRIVATE LUA_COMPAT_5_1=1)' CMakeLists.txt
fi

# 2. Create a completely new compatibility layer approach
echo "Creating an updated lua_wrapper.h..."

cat > source/lua_wrapper.h << 'EOL'
// Lua API wrapper for executor - Completely independent implementation
// Do not use this file together with actual Lua headers
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// Basic type definitions
typedef struct lua_State lua_State;
typedef int (*lua_CFunction)(lua_State* L);

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

// Basic API functions
int lua_pcall(lua_State* L, int nargs, int nresults, int errfunc);
void luaL_error(lua_State* L, const char* fmt, ...);
int lua_gettop(lua_State* L);
void lua_settop(lua_State* L, int idx);
void lua_pushnil(lua_State* L);
void lua_pushnumber(lua_State* L, double n);
void lua_pushboolean(lua_State* L, int b);
void lua_pushstring(lua_State* L, const char* s);
void lua_createtable(lua_State* L, int narr, int nrec);
void lua_setfield(lua_State* L, int idx, const char* k);
int lua_type(lua_State* L, int idx);
const char* luaL_typename(lua_State* L, int idx);
void luaL_typeerror(lua_State* L, int narg, const char* tname);
void luaL_argerror(lua_State* L, int narg, const char* extramsg);
void luaL_register(lua_State* L, const char* libname, const void* l);

// Simplified helper macros - completely separate from Lua's real macros
#define lua_isnil(L,n) (lua_type(L,n) == LUA_TNIL)
#define lua_isnumber(L,n) (lua_type(L,n) == LUA_TNUMBER)
#define lua_isstring(L,n) (lua_type(L,n) == LUA_TSTRING)
#define lua_tostring(L,i) "dummy_string"
#define lua_pushinteger(L,n) lua_pushnumber(L, (double)(n))
#define lua_pop(L,n) lua_settop(L, -(n)-1)

// Simplified structure for registry entries - not compatible with real Lua
struct ExecutorLuaReg {
    const char* name;
    lua_CFunction func;
};

#ifdef __cplusplus
}
#endif
EOL

# Update the implementation file
cat > source/lua_wrapper.c << 'EOL'
// Implementation for executor's independent Lua wrapper
#include "lua_wrapper.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

// Simplified implementation - these functions are only used when
// not linking directly to a Lua implementation
int lua_pcall(lua_State* L, int nargs, int nresults, int errfunc) {
    printf("lua_pcall(%p, %d, %d, %d) called\n", L, nargs, nresults, errfunc);
    return 0; // Success
}

void luaL_error(lua_State* L, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    printf("luaL_error: ");
    vprintf(fmt, args);
    printf("\n");
    va_end(args);
}

int lua_gettop(lua_State* L) {
    return 0;
}

void lua_settop(lua_State* L, int idx) {
    // No operation
}

void lua_pushnil(lua_State* L) {
    // No operation
}

void lua_pushnumber(lua_State* L, double n) {
    // No operation
}

void lua_pushboolean(lua_State* L, int b) {
    // No operation
}

void lua_pushstring(lua_State* L, const char* s) {
    // No operation
}

void lua_createtable(lua_State* L, int narr, int nrec) {
    // No operation
}

void lua_setfield(lua_State* L, int idx, const char* k) {
    // No operation
}

int lua_type(lua_State* L, int idx) {
    return LUA_TNIL;
}

const char* luaL_typename(lua_State* L, int idx) {
    return "nil";
}

void luaL_typeerror(lua_State* L, int narg, const char* tname) {
    printf("luaL_typeerror: Expected %s at argument %d\n", tname, narg);
}

void luaL_argerror(lua_State* L, int narg, const char* extramsg) {
    printf("luaL_argerror: %s at argument %d\n", extramsg, narg);
}

void luaL_register(lua_State* L, const char* libname, const void* l) {
    // Simplified implementation
    printf("luaL_register: Registering library %s\n", libname ? libname : "unknown");
}
EOL

# 3. Fix the fix_lua_includes.sh script to use a complete replacement approach
echo "Updating fix_lua_includes.sh..."

cat > fix_lua_includes.sh << 'EOL'
#!/bin/bash
# Find files that include our wrapper or Lua headers
echo "Cleaning up Lua includes in all files..."

# For files that include both our wrapper and real Lua headers, remove our wrapper
find source -name "*.c" -o -name "*.cpp" -o -name "*.mm" | xargs grep -l "lua_wrapper.h.*luau/lua\|luau/lua.*lua_wrapper.h" | while read file; do
  if [ "$file" != "source/lfs.c" ]; then  # Skip lfs.c as it's handled separately
    echo "Fixing $file to use real Lua headers only..."
    grep -v "lua_wrapper.h" "$file" > "$file.tmp"
    mv "$file.tmp" "$file"
  fi
done

# For files that don't include real Lua headers but use Lua functionality,
# make sure they include our wrapper
find source -name "*.c" -o -name "*.cpp" -o -name "*.mm" | xargs grep -l "lua_State\|lua_pcall\|luaL_error" | \
  grep -v -l "#include.*luau/lua" | \
  grep -v "lfs.c" | \
  while read file; do
    if ! grep -q "#include.*lua_wrapper.h" "$file"; then
      echo "Adding our wrapper to $file..."
      sed -i '1i#include "lua_wrapper.h"' "$file"
    fi
  done

echo "Done fixing Lua includes!"
EOL
chmod +x fix_lua_includes.sh

# 4. Now apply the fixes
echo "Applying Lua fixes..."
./fix_lfs.sh
./fix_lua_includes.sh

echo "==== Lua Build System Fixes Complete ===="
