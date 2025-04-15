#!/bin/bash

echo "Looking for lua_bundled library:"
find build -name "liblua*" -o -name "*lua_bundled*"

echo ""
echo "Checking for symbols in lua_bundled library (if available):"
for lib in $(find build -name "liblua*" -o -name "*lua_bundled*"); do
  echo "Symbols in $lib:"
  nm -g $lib | grep -E "_lua_|_luaL_" | head -15
done
