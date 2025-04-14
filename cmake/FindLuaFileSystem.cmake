# FindLuaFileSystem.cmake for Homebrew Luau
# This module allows compilation of lfs.c using Homebrew Luau

# Create a target for lfs.c with Homebrew Luau
function(add_lfs_target)
    # Don't add it twice
    if(TARGET lfs_obj)
        return()
    endif()
    
    message(STATUS "Setting up LuaFileSystem with Homebrew Luau headers")
    
    # Create an object library for lfs.c
    add_library(lfs_obj OBJECT ${CMAKE_SOURCE_DIR}/source/lfs.c)
    
    # Try to get from environment variables first
    if(DEFINED ENV{LUAU_INCLUDE_DIR})
        set(LUAU_INCLUDE_DIR $ENV{LUAU_INCLUDE_DIR})
        message(STATUS "Using Luau include dir from environment: ${LUAU_INCLUDE_DIR}")
    else()
        # Try to find it using Homebrew
        execute_process(
            COMMAND brew --prefix luau
            OUTPUT_VARIABLE LUAU_PREFIX
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
        )
        
        if(LUAU_PREFIX)
            set(LUAU_INCLUDE_DIR "${LUAU_PREFIX}/include")
            message(STATUS "Found Homebrew Luau include directory: ${LUAU_INCLUDE_DIR}")
        else()
            message(FATAL_ERROR "Luau include directory not found. Please install Luau with Homebrew.")
        endif()
    endif()
    
    # Add include directories
    target_include_directories(lfs_obj PRIVATE
        ${LUAU_INCLUDE_DIR}
        ${CMAKE_SOURCE_DIR}/source
    )
    
    # Ensure the compiler knows this is C
    set_target_properties(lfs_obj PROPERTIES
        C_STANDARD 99
        POSITION_INDEPENDENT_CODE ON
    )
    
    message(STATUS "LFS using Homebrew Luau headers from: ${LUAU_INCLUDE_DIR}")
endfunction()
