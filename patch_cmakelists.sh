#!/bin/bash
# Add lua_wrapper.c to the CORE_SOURCES in CMakeLists.txt
echo "Patching source/cpp/CMakeLists.txt..."
sed -i '/set(CORE_SOURCES/a\    source/lua_wrapper.c' source/cpp/CMakeLists.txt
sed -i '/include_directories/a\    ${CMAKE_SOURCE_DIR}/source' source/cpp/CMakeLists.txt
echo "Done!"
