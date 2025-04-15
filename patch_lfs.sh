#!/bin/bash
# Insert our lua_wrapper.h at the top of the file
sed -i '1i\
// Include our wrapper first to fix Lua compatibility issues\
#include "lua_wrapper.h"\
' source/lfs.c
