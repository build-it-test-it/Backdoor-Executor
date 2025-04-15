// Include in the same order as lfs.c
#include "../source/cpp/luau/lua_defs.h"
#include "../source/cpp/luau/lua.h"
#include "../source/cpp/luau/lualib.h"

int main() {
    lua_State* L = NULL;
    lua_pushstring(L, "test");
    return 0;
}
