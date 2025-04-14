# FindLuaFileSystem.cmake
# This module allows compilation of lfs.c specifically

# Create a target for lfs.c that ensures it can find lua.h
function(add_lfs_target)
    # Don't add it twice
    if(TARGET lfs_obj)
        return()
    endif()
    
    # Get the Lua include directories
    get_directory_property(LUA_INCLUDES DIRECTORY ${CMAKE_SOURCE_DIR} INCLUDE_DIRECTORIES)
    
    # Create an object library for lfs.c
    add_library(lfs_obj OBJECT ${CMAKE_SOURCE_DIR}/source/lfs.c)
    
    # Set include directories for just this file
    target_include_directories(lfs_obj PRIVATE
        ${LUA_INCLUDE_DIR}
        $ENV{LUA_INCLUDE_DIR}
        /opt/homebrew/opt/lua/include
        /opt/homebrew/include
        /usr/local/include
        ${LUA_INCLUDES}
    )
    
    # Add a define to use quotes instead of brackets for includes
    target_compile_definitions(lfs_obj PRIVATE LFS_USE_INCLUDE_QUOTES)
    
    # Ensure the compiler knows this is C
    set_target_properties(lfs_obj PROPERTIES
        C_STANDARD 99
        POSITION_INDEPENDENT_CODE ON
    )
    
    # Print diagnostic info
    message(STATUS "LFS include directories: ${LUA_INCLUDE_DIR}")
    message(STATUS "LFS compile definitions: LFS_USE_INCLUDE_QUOTES")
endfunction()
