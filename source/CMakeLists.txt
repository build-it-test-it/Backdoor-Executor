# CMakeLists.txt for the source directory
cmake_minimum_required(VERSION 3.16)

# Compile LFS (Lua File System)
add_library(lfs_lib OBJECT
    lfs.c
)

# Set compile flags for LFS
target_compile_definitions(lfs_lib PRIVATE 
    LFS_API=  # Leave blank for static linking
    LUA_LIB=1
)

# Include directories
target_include_directories(lfs_lib PRIVATE
    ${LUA_INCLUDE_DIR}
)

# Set visibility
set_target_properties(lfs_lib PROPERTIES
    C_VISIBILITY_PRESET hidden
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
)

# Expose headers to parent scope
target_include_directories(lfs_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
