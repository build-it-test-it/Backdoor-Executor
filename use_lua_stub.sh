#!/bin/bash
# Use our custom stub Lua headers for lfs.c

echo "==== Using stub Lua headers for lfs.c ===="

# Fix lfs.c to use the stub headers
cp source/lfs.c source/lfs.c.bak2

# Remove any existing includes to start fresh
grep -v "#include.*luau/lua" source/lfs.c > source/lfs.c.tmp1
grep -v "#include.*lualib" source/lfs.c.tmp1 > source/lfs.c.tmp2
grep -v "lua_wrapper.h" source/lfs.c.tmp2 > source/lfs.c.tmp
grep -v "lua_defs.h" source/lfs.c.tmp > source/lfs.c.new
mv source/lfs.c.new source/lfs.c

# Add our stub headers
sed -i '1i// Using stub Lua headers\n#include "lua_stub/lua.h"\n#include "lua_stub/lualib.h"\n' source/lfs.c

# Update CMakeLists.txt to include our stub directory
if grep -q "target_include_directories(lfs_obj" CMakeLists.txt; then
  sed -i '/target_include_directories(lfs_obj/c\
target_include_directories(lfs_obj PRIVATE\
    ${CMAKE_SOURCE_DIR}/source\
    ${CMAKE_SOURCE_DIR}/source/lua_stub\
)' CMakeLists.txt
fi

# Let's test compile with our stub headers
mkdir -p test_stub
cd test_stub
cat > test_stub.c << 'EOL'
// Include our stub headers
#include "../source/lua_stub/lua.h"
#include "../source/lua_stub/lualib.h"

int main() {
    lua_State* L = NULL;
    lua_pushstring(L, "test");
    luaL_typename(L, 1);
    return 0;
}
EOL

# Compile with -fsyntax-only to check for compilation errors without linking
echo "Testing compilation with stub headers..."
gcc -fsyntax-only -I.. test_stub.c

# Check if compilation worked
if [ $? -eq 0 ]; then
    echo "✅ Stub headers compilation successful!"
else
    echo "❌ Stub headers compilation failed!"
fi

cd ..

echo "==== Stub headers setup complete ===="
