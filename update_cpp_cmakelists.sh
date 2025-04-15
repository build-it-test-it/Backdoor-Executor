#!/bin/bash

# Backup the original file
cp source/cpp/CMakeLists.txt source/cpp/CMakeLists.txt.bak

# Create a new file with proper implementation
cat > source/cpp/CMakeLists.txt << 'EOL'
# CMakeLists.txt for source/cpp - Production version

# Set include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/source
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/source/cpp
    ${CMAKE_SOURCE_DIR}/source/cpp/luau
)

# Gather all source files
file(GLOB_RECURSE CPP_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
)

# Exclude iOS and Objective-C files if needed
if(NOT APPLE)
    list(FILTER CPP_SOURCES EXCLUDE REGEX ".*\\.mm$")
endif()

# Create the library with real implementation
add_library(roblox_execution STATIC ${CPP_SOURCES})

# Set include directories
target_include_directories(roblox_execution PUBLIC
    ${CMAKE_SOURCE_DIR}/source
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/source/cpp
    ${CMAKE_SOURCE_DIR}/source/cpp/luau
)

# Find Dobby and link
find_package(Dobby QUIET)
if(Dobby_FOUND)
    target_link_libraries(roblox_execution Dobby::dobby)
else()
    # Use our local Dobby files
    target_include_directories(roblox_execution PUBLIC
        ${CMAKE_SOURCE_DIR}/external/dobby/include
    )
    target_link_libraries(roblox_execution ${CMAKE_SOURCE_DIR}/external/dobby/lib/libdobby.a)
endif()

# Link against Lua libraries
target_link_libraries(roblox_execution lua_bundled)
EOL

echo "Updated source/cpp/CMakeLists.txt with real implementation"
