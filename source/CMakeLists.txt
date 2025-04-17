# CMakeLists.txt for the source directory
cmake_minimum_required(VERSION 3.16)

# Print debug information in CI
if(DEFINED ENV{CI} OR DEFINED ENV{GITHUB_ACTIONS})
    message(STATUS "LFS compilation setup:")
    message(STATUS "  LUA_INCLUDE_DIR: ${LUA_INCLUDE_DIR}")
    message(STATUS "  Current source dir: ${CMAKE_CURRENT_SOURCE_DIR}")
    message(STATUS "  Using direct VM source inclusion in main library")
    set(CMAKE_VERBOSE_MAKEFILE ON)
endif()

# Check if lfs.c exists
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/lfs.c")
    message(WARNING "lfs.c not found at ${CMAKE_CURRENT_SOURCE_DIR}/lfs.c")
endif()

# Compile LFS (Lua File System)
add_library(lfs_lib OBJECT
    lfs.c
)

# Set compile flags for LFS
target_compile_definitions(lfs_lib PRIVATE 
    LFS_API=  # Leave blank for static linking
    LUA_LIB=1
    USE_LUAU=1 # Ensure it knows we're using Luau
)

# Include directories - direct path to VM include folder
target_include_directories(lfs_lib PRIVATE
    ${LUA_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/VM/include
)

# Set visibility
set_target_properties(lfs_lib PROPERTIES
    C_VISIBILITY_PRESET hidden
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
    POSITION_INDEPENDENT_CODE ON
)

# iOS-specific compile options if applicable
if(APPLE AND CMAKE_SYSTEM_NAME MATCHES "iOS")
    target_compile_options(lfs_lib PRIVATE
        -fembed-bitcode
        -mios-version-min=13.0
    )
endif()

# Expose headers to parent scope
target_include_directories(lfs_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
