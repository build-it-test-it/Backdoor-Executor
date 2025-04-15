#!/bin/bash
# Fix lfs.c to work without our wrapper

# Create a backup
cp source/lfs.c source/lfs.c.bak

# Remove our wrapper includes completely
grep -v "lua_wrapper.h" source/lfs.c > source/lfs.c.tmp
mv source/lfs.c.tmp source/lfs.c

# Make any other necessary modifications to ensure it works with the real headers
# (This step depends on what specific adaptations are needed)

echo "lfs.c updated to work directly with Lua headers"
