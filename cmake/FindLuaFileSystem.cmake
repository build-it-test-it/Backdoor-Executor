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
    
    # First try the internal Luau headers from the project
    set(INTERNAL_LUAU_DIR "${CMAKE_SOURCE_DIR}/source/cpp/luau")
    
    if(EXISTS "${INTERNAL_LUAU_DIR}/lua.h")
        set(LUAU_INCLUDE_DIR ${INTERNAL_LUAU_DIR})
        message(STATUS "Using internal Luau headers from: ${LUAU_INCLUDE_DIR}")
    else()
        # Try to get from environment variables
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
                message(WARNING "Luau include directory not found. Using internal headers.")
                set(LUAU_INCLUDE_DIR "${INTERNAL_LUAU_DIR}")
            endif()
        endif()
    endif()
    
    # Add include directories - include both the project root and source dir to help with relative includes
    target_include_directories(lfs_obj PRIVATE
        ${LUAU_INCLUDE_DIR}
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/source
        ${CMAKE_SOURCE_DIR}/source/cpp
    )
    
    # Add compile definitions to help with compatibility
    target_compile_definitions(lfs_obj PRIVATE
        LUA_COMPAT_5_1=1
    )
    
    # Ensure the compiler knows this is C
    set_target_properties(lfs_obj PROPERTIES
        C_STANDARD 99
        POSITION_INDEPENDENT_CODE ON
    )
    
    message(STATUS "LFS using Homebrew Luau headers from: ${LUAU_INCLUDE_DIR}")
endfunction()
