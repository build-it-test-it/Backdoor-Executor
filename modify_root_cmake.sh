#!/bin/bash
echo "# Include our wrapper files directly" >> CMakeLists.txt
echo "add_library(lua_wrapper STATIC source/lua_wrapper.c)" >> CMakeLists.txt
echo "target_include_directories(lua_wrapper PUBLIC source)" >> CMakeLists.txt
echo "# Link the wrapper with the main library" >> CMakeLists.txt
echo "target_link_libraries(roblox_executor PRIVATE lua_wrapper)" >> CMakeLists.txt
