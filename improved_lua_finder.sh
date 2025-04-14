#!/bin/bash

# This script will be added to the GitHub workflow to better find Lua

# Check Homebrew installation and location
echo "Checking Homebrew location and installation..."
BREW_PREFIX=$(brew --prefix)
echo "Homebrew prefix: $BREW_PREFIX"

# Install lua explicitly with a specific version
echo "Installing Lua with full development headers..."
brew install lua@5.4 || brew install lua

# Check both lua and lua@5.4 installations
echo "Checking lua installation locations..."
LUA_PREFIX=$(brew --prefix lua 2>/dev/null)
LUA54_PREFIX=$(brew --prefix lua@5.4 2>/dev/null)

echo "Standard Lua prefix: $LUA_PREFIX"
echo "Lua 5.4 prefix: $LUA54_PREFIX"

# Decide which prefix to use
if [ -n "$LUA54_PREFIX" ] && [ -d "$LUA54_PREFIX/include" ]; then
    echo "Using Lua 5.4 installation"
    ACTIVE_LUA_PREFIX=$LUA54_PREFIX
elif [ -n "$LUA_PREFIX" ] && [ -d "$LUA_PREFIX/include" ]; then
    echo "Using standard Lua installation"
    ACTIVE_LUA_PREFIX=$LUA_PREFIX
else
    echo "No valid Lua installation found"
    ACTIVE_LUA_PREFIX=$BREW_PREFIX
fi

# Create all possible directories we might need
mkdir -p lua_headers
mkdir -p lua_lib

# Recursively find lua.h in Homebrew directories
echo "Searching for lua.h in Homebrew directories..."
find $BREW_PREFIX -name "lua.h" | while read -r file; do 
    echo "Found: $file"
    cp "$file" lua_headers/
done

# Check all possible include directories
echo "Searching for Lua headers in standard locations..."
LUA_INCLUDE_DIRS=(
    "$ACTIVE_LUA_PREFIX/include"
    "$ACTIVE_LUA_PREFIX/include/lua"
    "$ACTIVE_LUA_PREFIX/include/lua5.4"
    "$BREW_PREFIX/include"
    "$BREW_PREFIX/include/lua"
    "$BREW_PREFIX/include/lua5.4"
    "/usr/local/include"
    "/usr/local/include/lua"
    "/usr/local/include/lua5.4"
    "/usr/include"
    "/usr/include/lua"
    "/usr/include/lua5.4"
)

# Check all possible lib directories
echo "Searching for Lua libraries in standard locations..."
LUA_LIB_DIRS=(
    "$ACTIVE_LUA_PREFIX/lib"
    "$BREW_PREFIX/lib"
    "/usr/local/lib"
    "/usr/lib"
)

# Copy headers and libraries
echo "Copying all Lua headers and libraries found..."

# Search and copy headers
for dir in "${LUA_INCLUDE_DIRS[@]}"; do
    if [ -d "$dir" ]; then
        echo "Checking directory: $dir"
        find "$dir" -name "lua*.h" -exec cp {} lua_headers/ \; 2>/dev/null
    fi
done

# Search and copy libraries
for dir in "${LUA_LIB_DIRS[@]}"; do
    if [ -d "$dir" ]; then
        echo "Checking directory: $dir"
        find "$dir" -name "liblua*.dylib" -o -name "liblua*.a" -exec cp {} lua_lib/ \; 2>/dev/null
    fi
done

# Check what we found
echo "Copied headers:"
ls -la lua_headers/

echo "Copied libraries:"
ls -la lua_lib/

# Create simpler header includes for C code
echo "Creating simplified lua.h for direct inclusion..."
cat > lua_headers/simplified_lua.h << 'EOFILE'
/* Simplified lua.h header for direct inclusion */
#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>

typedef struct lua_State lua_State;

typedef int (*lua_CFunction) (lua_State *L);

#define LUA_REGISTRYINDEX(-10000)
#define LUA_ENVIRONINDEX(-10001)
#define LUA_GLOBALSINDEX(-10002)

/* thread status */
#define LUA_OK0
#define LUA_YIELD1
#define LUA_ERRRUN2
#define LUA_ERRSYNTAX3
#define LUA_ERRMEM4
#define LUA_ERRERR5

typedef double lua_Number;
typedef ptrdiff_t lua_Integer;

/* Basic API */
lua_State *lua_newstate(void *(*f)(void *ud, void *ptr, size_t osize, size_t nsize), void *ud);
void lua_close(lua_State *L);
lua_State *lua_newthread(lua_State *L);

lua_CFunction lua_atpanic(lua_State *L, lua_CFunction panicf);

int lua_gettop(lua_State *L);
void lua_settop(lua_State *L, int idx);
void lua_pushvalue(lua_State *L, int idx);
void lua_remove(lua_State *L, int idx);
void lua_insert(lua_State *L, int idx);
void lua_replace(lua_State *L, int idx);
int lua_checkstack(lua_State *L, int sz);

void lua_xmove(lua_State *from, lua_State *to, int n);

/* Access functions */
int lua_isnumber(lua_State *L, int idx);
int lua_isstring(lua_State *L, int idx);
int lua_iscfunction(lua_State *L, int idx);
int lua_isuserdata(lua_State *L, int idx);
int lua_type(lua_State *L, int idx);
const char *lua_typename(lua_State *L, int tp);

int lua_equal(lua_State *L, int idx1, int idx2);
int lua_rawequal(lua_State *L, int idx1, int idx2);
int lua_lessthan(lua_State *L, int idx1, int idx2);

lua_Number lua_tonumber(lua_State *L, int idx);
lua_Integer lua_tointeger(lua_State *L, int idx);
int lua_toboolean(lua_State *L, int idx);
const char *lua_tostring(lua_State *L, int idx);
size_t lua_strlen(lua_State *L, int idx);
lua_CFunction lua_tocfunction(lua_State *L, int idx);
void *lua_touserdata(lua_State *L, int idx);
lua_State *lua_tothread(lua_State *L, int idx);
const void *lua_topointer(lua_State *L, int idx);

/* Push functions */
void lua_pushnil(lua_State *L);
void lua_pushnumber(lua_State *L, lua_Number n);
void lua_pushinteger(lua_State *L, lua_Integer n);
void lua_pushlstring(lua_State *L, const char *s, size_t l);
void lua_pushstring(lua_State *L, const char *s);
const char *lua_pushvfstring(lua_State *L, const char *fmt, va_list argp);
const char *lua_pushfstring(lua_State *L, const char *fmt, ...);
void lua_pushcclosure(lua_State *L, lua_CFunction fn, int n);
void lua_pushboolean(lua_State *L, int b);
void lua_pushlightuserdata(lua_State *L, void *p);
int lua_pushthread(lua_State *L);

/* Get functions */
void lua_gettable(lua_State *L, int idx);
void lua_getfield(lua_State *L, int idx, const char *k);
void lua_rawget(lua_State *L, int idx);
void lua_rawgeti(lua_State *L, int idx, int n);
void lua_createtable(lua_State *L, int narr, int nrec);
void *lua_newuserdata(lua_State *L, size_t sz);
int lua_getmetatable(lua_State *L, int objindex);
void lua_getfenv(lua_State *L, int idx);

/* Set functions */
void lua_settable(lua_State *L, int idx);
void lua_setfield(lua_State *L, int idx, const char *k);
void lua_rawset(lua_State *L, int idx);
void lua_rawseti(lua_State *L, int idx, int n);
int lua_setmetatable(lua_State *L, int objindex);
int lua_setfenv(lua_State *L, int idx);

/* 'load' and 'call' functions */
void lua_call(lua_State *L, int nargs, int nresults);
int lua_pcall(lua_State *L, int nargs, int nresults, int errfunc);
int lua_cpcall(lua_State *L, lua_CFunction func, void *ud);
int lua_load(lua_State *L, const char *(*reader)(lua_State *, void *, size_t *), void *dt, const char *chunkname);

int lua_dump(lua_State *L, int (*writer)(lua_State *, const void *, size_t, void *), void *data);

/* Coroutine functions */
int lua_yield(lua_State *L, int nresults);
int lua_resume(lua_State *L, int narg);
int lua_status(lua_State *L);

#ifdef __cplusplus
}
#endif
EOFILE

# Create simplified lauxlib.h
cat > lua_headers/simplified_lauxlib.h << 'EOFILE'
/* Simplified lauxlib.h header for direct inclusion */
#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdio.h>

#include "simplified_lua.h"

/* Extra error code for 'luaL_load' */
#define LUA_ERRFILE     (LUA_ERRERR+1)

typedef struct luaL_Reg {
  const char *name;
  lua_CFunction func;
} luaL_Reg;

void luaL_openlib(lua_State *L, const char *libname, const luaL_Reg *l, int nup);
void luaL_register(lua_State *L, const char *libname, const luaL_Reg *l);
int luaL_getmetafield(lua_State *L, int obj, const char *e);
int luaL_callmeta(lua_State *L, int obj, const char *e);
int luaL_typerror(lua_State *L, int narg, const char *tname);
int luaL_argerror(lua_State *L, int numarg, const char *extramsg);
const char *luaL_checklstring(lua_State *L, int numArg, size_t *l);
const char *luaL_optlstring(lua_State *L, int numArg, const char *def, size_t *l);
lua_Number luaL_checknumber(lua_State *L, int numArg);
lua_Number luaL_optnumber(lua_State *L, int nArg, lua_Number def);

lua_Integer luaL_checkinteger(lua_State *L, int numArg);
lua_Integer luaL_optinteger(lua_State *L, int nArg, lua_Integer def);

void luaL_checkstack(lua_State *L, int sz, const char *msg);
void luaL_checktype(lua_State *L, int narg, int t);
void luaL_checkany(lua_State *L, int narg);

int luaL_newmetatable(lua_State *L, const char *tname);
void *luaL_checkudata(lua_State *L, int ud, const char *tname);

void luaL_where(lua_State *L, int lvl);
int luaL_error(lua_State *L, const char *fmt, ...);

int luaL_checkoption(lua_State *L, int narg, const char *def, const char *const lst[]);

int luaL_ref(lua_State *L, int t);
void luaL_unref(lua_State *L, int t, int ref);

int luaL_loadfile(lua_State *L, const char *filename);
int luaL_loadbuffer(lua_State *L, const char *buff, size_t sz, const char *name);
int luaL_loadstring(lua_State *L, const char *s);

lua_State *luaL_newstate(void);

const char *luaL_gsub(lua_State *L, const char *s, const char *p, const char *r);

const char *luaL_findtable(lua_State *L, int idx, const char *fname, int szhint);

#ifdef __cplusplus
}
#endif
EOFILE

# Create simplified lualib.h
cat > lua_headers/simplified_lualib.h << 'EOFILE'
/* Simplified lualib.h header for direct inclusion */
#ifdef __cplusplus
extern "C" {
#endif

#include "simplified_lua.h"

/* Key to file-handle type */
#define LUA_FILEHANDLE "FILE*"

#define LUA_COLIBNAME "coroutine"
LUALIB_API int luaopen_base(lua_State *L);

#define LUA_TABLIBNAME "table"
LUALIB_API int luaopen_table(lua_State *L);

#define LUA_IOLIBNAME "io"
LUALIB_API int luaopen_io(lua_State *L);

#define LUA_OSLIBNAME "os"
LUALIB_API int luaopen_os(lua_State *L);

#define LUA_STRLIBNAME "string"
LUALIB_API int luaopen_string(lua_State *L);

#define LUA_MATHLIBNAME "math"
LUALIB_API int luaopen_math(lua_State *L);

#define LUA_DBLIBNAME "debug"
LUALIB_API int luaopen_debug(lua_State *L);

#define LUA_LOADLIBNAME "package"
LUALIB_API int luaopen_package(lua_State *L);

/* open all previous libraries */
LUALIB_API void luaL_openlibs(lua_State *L);

#ifdef __cplusplus
}
#endif
EOFILE

echo "Script completed"
