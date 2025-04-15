#!/bin/bash
# Find all .c and .cpp files that include lua.h or lualib.h
echo "Finding files that include Lua headers..."
FILES=$(grep -l "#include.*luau/lua" --include="*.c" --include="*.cpp" --include="*.mm" -r source/)

# Add our wrapper at the top of each file
for file in $FILES; do
  echo "Patching $file..."
  sed -i '1i\
// Include our wrapper first to fix Lua compatibility issues\
#include "lua_wrapper.h"\
' "$file"
done

echo "Done! Patched $(echo "$FILES" | wc -w) files."
