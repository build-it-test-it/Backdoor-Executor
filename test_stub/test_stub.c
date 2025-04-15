// Include our stub headers
#include "../source/lua_stub/lua.h"
#include "../source/lua_stub/lualib.h"

int main() {
    lua_State* L = NULL;
    lua_pushstring(L, "test");
    luaL_typename(L, 1);
    return 0;
}
