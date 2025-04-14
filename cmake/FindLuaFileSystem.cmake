# FindLuaFileSystem.cmake
# This module allows compilation of lfs.c specifically using our own Lua headers

# Create a target for lfs.c that ensures it can find the Luau headers
function(add_lfs_target)
    # Don't add it twice
    if(TARGET lfs_obj)
        return()
    endif()
    
    message(STATUS "Setting up LuaFileSystem with native Luau headers")
    
    # Create an object library for lfs.c
    add_library(lfs_obj OBJECT ${CMAKE_SOURCE_DIR}/source/lfs.c)
    
    # Set include directories for just this file
    # The source directory is needed for relative includes like "cpp/luau/lua.h"
    target_include_directories(lfs_obj PRIVATE
        ${CMAKE_SOURCE_DIR}/source         # Main source directory for relative includes
        ${CMAKE_SOURCE_DIR}/source/cpp     # For cpp/luau/lua.h path style
        ${CMAKE_SOURCE_DIR}/source/cpp/luau # For direct lua.h access
        ${CMAKE_SOURCE_DIR}                # For absolute paths
    )
    
    # Add a define to use the internal Luau headers
    target_compile_definitions(lfs_obj PRIVATE 
        LFS_USE_INTERNAL_LUAU=1
        LUAU_FASTFLAG_LUAERROR=1          # Handle missing dependencies
    )
    
    # Ensure the compiler knows this is C
    set_target_properties(lfs_obj PROPERTIES
        C_STANDARD 99
        POSITION_INDEPENDENT_CODE ON
    )
    
    message(STATUS "LFS using internal Luau headers from ${CMAKE_SOURCE_DIR}/source/cpp/luau")
endfunction()
