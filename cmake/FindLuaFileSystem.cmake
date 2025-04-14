# FindLuaFileSystem.cmake
# This module allows compilation of lfs.c specifically by finding external Lua

# Create a target for lfs.c with external Lua
function(add_lfs_target)
    # Don't add it twice
    if(TARGET lfs_obj)
        return()
    endif()
    
    message(STATUS "Setting up LuaFileSystem with external Lua headers")
    
    # Create an object library for lfs.c
    add_library(lfs_obj OBJECT ${CMAKE_SOURCE_DIR}/source/lfs.c)
    
    # Look for lua in standard paths
    find_path(LUA_INCLUDE_DIR lua.h
        PATHS
        /opt/homebrew/opt/lua/include
        /opt/homebrew/include
        /usr/local/include
        /usr/include
        PATH_SUFFIXES lua lua5.4 lua5.3 lua5.2 lua5.1
    )
    
    # Add include directories
    target_include_directories(lfs_obj PRIVATE
        ${LUA_INCLUDE_DIR}
    )
    
    # Enable standard paths with quotes if needed
    if(LUA_INCLUDE_DIR)
        message(STATUS "Found Lua include directory: ${LUA_INCLUDE_DIR}")
        target_compile_definitions(lfs_obj PRIVATE LFS_USE_INCLUDE_QUOTES)
    endif()
    
    # Ensure the compiler knows this is C
    set_target_properties(lfs_obj PROPERTIES
        C_STANDARD 99
        POSITION_INDEPENDENT_CODE ON
    )
    
    message(STATUS "LFS using external Lua headers from: ${LUA_INCLUDE_DIR}")
endfunction()
