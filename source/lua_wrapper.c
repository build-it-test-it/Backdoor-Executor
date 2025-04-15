// Implementation of our non-conflicting Lua wrapper
#include "lua_wrapper.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

// Implementation of our functions
int executor_lua_pcall(lua_State* L, int nargs, int nresults, int errfunc) {
    printf("executor_lua_pcall(%p, %d, %d, %d) called\n", L, nargs, nresults, errfunc);
    return 0; // Success
}

void executor_luaL_error(lua_State* L, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    printf("executor_luaL_error: ");
    vprintf(fmt, args);
    printf("\n");
    va_end(args);
}

const char* executor_luaL_typename(lua_State* L, int idx) {
    return "nil";
}

int executor_lua_gettop(lua_State* L) {
    return 0;
}

void executor_lua_settop(lua_State* L, int idx) {
    // No-op
}

void executor_lua_pushnil(lua_State* L) {
    // No-op
}

void executor_lua_pushnumber(lua_State* L, double n) {
    // No-op
}

void executor_lua_pushstring(lua_State* L, const char* s) {
    // No-op
}

void executor_lua_createtable(lua_State* L, int narr, int nrec) {
    // No-op
}

void executor_lua_setfield(lua_State* L, int idx, const char* k) {
    // No-op
}

int executor_lua_type(lua_State* L, int idx) {
    return LUA_TNIL;
}
