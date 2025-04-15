#!/bin/bash
# Fix the malformed CMakeLists.txt target_include_directories

# First, create a backup
cp CMakeLists.txt CMakeLists.txt.bak

# Replace the malformed section with a clean version
sed -i '/# Add LuaFileSystem if not found/,/target_compile_definitions(lfs_obj/c\
# Add LuaFileSystem if not found
if(NOT TARGET lfs_obj AND EXISTS ${CMAKE_SOURCE_DIR}/source/lfs.c)
    message(STATUS "Using bundled LuaFileSystem implementation")
    add_library(lfs_obj OBJECT ${CMAKE_SOURCE_DIR}/source/lfs.c)
    target_include_directories(lfs_obj PRIVATE
        ${CMAKE_SOURCE_DIR}/source
        ${CMAKE_SOURCE_DIR}/source/lua_stub
    )
    target_compile_definitions(lfs_obj PRIVATE LUA_COMPAT_5_1=1)' CMakeLists.txt

# Check if the fix worked
grep -n -A 10 "Add LuaFileSystem if not found" CMakeLists.txt
