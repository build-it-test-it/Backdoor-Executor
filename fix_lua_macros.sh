#!/bin/bash
# Very specific fix for Lua macro definitions

echo "==== Fixing Lua macro definitions ===="

# Create even more precise lua_defs.h
cat > source/cpp/luau/lua_defs.h << 'EOL'
// Essential Lua macro definitions needed by lua.h and lualib.h
#pragma once

// API macros
#ifndef LUA_API
#define LUA_API extern
#endif

#ifndef LUALIB_API
#define LUALIB_API extern
#endif

// Only define LUA_NORETURN if it's not already defined
#ifndef LUA_NORETURN
#define LUA_NORETURN
#endif

// Important: DO NOT define l_noret here, let lua.h define it properly

// Format attributes
#ifndef LUA_PRINTF_ATTR
#define LUA_PRINTF_ATTR(fmt, args)
#endif
EOL

# Fix lfs.c to use the correct headers in the right order
cp source/lfs.c source/lfs.c.bak

# Remove any existing includes to start fresh
grep -v "#include.*luau/lua" source/lfs.c > source/lfs.c.tmp1
grep -v "#include.*lualib" source/lfs.c.tmp1 > source/lfs.c.tmp2
grep -v "lua_wrapper.h" source/lfs.c.tmp2 > source/lfs.c.tmp
mv source/lfs.c.tmp source/lfs.c

# Add proper includes in the correct order
sed -i '1i// Include Lua in proper order with essential definitions first\n#include "cpp/luau/lua_defs.h"\n#include "cpp/luau/lua.h"\n#include "cpp/luau/lualib.h"\n' source/lfs.c

# Let's test compile with a more accurate test
mkdir -p test_build2
cd test_build2
cat > test.c << 'EOL'
// Include in the same order as lfs.c
#include "../source/cpp/luau/lua_defs.h"
#include "../source/cpp/luau/lua.h"
#include "../source/cpp/luau/lualib.h"

int main() {
    lua_State* L = NULL;
    lua_pushstring(L, "test");
    return 0;
}
EOL

# Compile with -fsyntax-only to check for compilation errors without linking
echo "Testing compilation with gcc..."
gcc -fsyntax-only -I.. test.c

# Check if compilation worked
if [ $? -eq 0 ]; then
    echo "✅ Test compilation successful!"
else
    echo "❌ Test compilation failed!"
fi

cd ..

echo "==== Lua macro fix complete ===="
