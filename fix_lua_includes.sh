#!/bin/bash
# Find all .c and .cpp files that include lua.h or lualib.h
echo "Finding files that include Lua headers..."
FILES=$(grep -l "#include.*luau/lua\|#include.*lualib\|#include.*lauxlib" --include="*.c" --include="*.cpp" --include="*.mm" -r source/)

# Add our wrapper at the top of each file
for file in $FILES; do
  # Skip lfs.c as it's handled by patch_lfs.sh
  if [[ "$file" == "source/lfs.c" ]]; then
    continue
  fi
  
  echo "Patching $file..."
  # Only add our wrapper if it's not already included
  if ! grep -q "#include.*lua_wrapper.h" "$file"; then
    sed -i '1i\// Include our compatibility wrapper\n#include "lua_wrapper.h"\n' "$file"
  fi
done

echo "Done! Patched files that include Lua headers."
