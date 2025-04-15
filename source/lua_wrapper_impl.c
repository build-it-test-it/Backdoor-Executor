// Implementation of additional Lua functions needed for lfs.c
#include "lua_stub/lua.h"
#include "lua_stub/lualib.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

// Required by lfs.c

// Push formatted string
const char* lua_pushfstring(lua_State* L, const char* fmt, ...) {
    static char buffer[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);
    
    // Call lua_pushstring with the formatted result
    lua_pushstring(L, buffer);
    return buffer;
}

// Implementation for lua_pushboolean
void lua_pushboolean(lua_State* L, int b) {
    // Stub implementation
    printf("lua_pushboolean(%p, %d) called\n", L, b);
}

// Implementation for luaL_checkstring
const char* luaL_checkstring(lua_State* L, int numArg) {
    // Simplified wrapper around luaL_checklstring
    return luaL_checklstring(L, numArg, NULL);
}

// Implementation for lua_newuserdata
void* lua_newuserdata(lua_State* L, size_t size) {
    // Simple stub implementation that just allocates memory
    // This won't be linked to any actual Lua state
    void* memory = malloc(size);
    memset(memory, 0, size); // Initialize to zeros
    return memory;
}

// Implementation for luaL_checkudata
void* luaL_checkudata(lua_State* L, int ud, const char* tname) {
    // Simple stub that returns a dummy pointer
    static char dummy[1024];
    return dummy;
}

// Implementation for luaL_getmetatable
void luaL_getmetatable(lua_State* L, const char* tname) {
    // Simplified implementation that does nothing
    printf("luaL_getmetatable(%p, %s) called\n", L, tname);
}

// Implementation for lua_setmetatable
void lua_setmetatable(lua_State* L, int idx) {
    // Simplified implementation that does nothing
    printf("lua_setmetatable(%p, %d) called\n", L, idx);
}

// Implementation for luaL_checkoption
int luaL_checkoption(lua_State* L, int narg, const char* def, const char* const lst[]) {
    // Simple implementation that always returns 0 (first option)
    return 0;
}

// Implementation for lua_toboolean
int lua_toboolean(lua_State* L, int idx) {
    // Simple implementation that always returns true
    return 1;
}

// Implementation for lua_touserdata
void* lua_touserdata(lua_State* L, int idx) {
    // Simple implementation that returns a dummy value
    static char dummy[1024];
    return dummy;
}

// Implementation for lua_newtable
void lua_newtable(lua_State* L) {
    // No operation in stub
    printf("lua_newtable(%p) called\n", L);
}

// Implementation for lua_pushcfunction
void lua_pushcfunction(lua_State* L, lua_CFunction f, const char* debugname) {
    // No operation in stub
    printf("lua_pushcfunction(%p, %p, %s) called\n", L, (void*)f, debugname);
}

// Implementation for luaL_argcheck
void luaL_argcheck(lua_State* L, int cond, int arg, const char* extramsg) {
    // If condition is false (0), call luaL_argerror
    if (!cond) {
        printf("luaL_argcheck failed: %s (arg %d)\n", extramsg, arg);
        luaL_argerror(L, arg, extramsg);
    }
}

// Implementation for luaL_newmetatable
int luaL_newmetatable(lua_State* L, const char* tname) {
    // Simplified implementation that always returns 1 (success)
    printf("luaL_newmetatable(%p, %s) called\n", L, tname);
    return 1;
}

// Additional implementations for newly added functions

// Implementation for lua_pushlstring
void lua_pushlstring(lua_State* L, const char* s, size_t len) {
    // Just call lua_pushstring for simplicity in our stub
    printf("lua_pushlstring(%p, %s, %zu) called\n", L, s, len);
    lua_pushstring(L, s);
}

// Implementation for lua_pushliteral
void lua_pushliteral(lua_State* L, const char* s) {
    // Just call lua_pushstring for simplicity in our stub
    printf("lua_pushliteral(%p, %s) called\n", L, s);
    lua_pushstring(L, s);
}

// Implementation for lua_pushvalue
void lua_pushvalue(lua_State* L, int idx) {
    // Simply log in our stub
    printf("lua_pushvalue(%p, %d) called\n", L, idx);
}

// Implementation for lua_setglobal
void lua_setglobal(lua_State* L, const char* name) {
    // Simply log in our stub
    printf("lua_setglobal(%p, %s) called\n", L, name);
}

// Implementation for lua_rawset
void lua_rawset(lua_State* L, int idx) {
    // Simply log in our stub
    printf("lua_rawset(%p, %d) called\n", L, idx);
}
