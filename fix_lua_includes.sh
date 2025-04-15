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
