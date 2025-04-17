# FindLua.cmake for iOS Roblox Executor
# This module finds the Lua/Luau libraries or builds them if not found
# Inspired by the FindDobby.cmake approach

# Variables this module defines:
# LUA_FOUND - True if Lua was found
# LUA_INCLUDE_DIR - Directory containing Lua headers
# LUA_LIBRARIES - Libraries needed to use Lua

# Try to find Lua in standard locations
find_path(LUA_INCLUDE_DIR
    NAMES lua.h luaconf.h lualib.h
    PATHS
        ${CMAKE_SOURCE_DIR}/external/lua/include
        ${CMAKE_SOURCE_DIR}/external/include
        ${CMAKE_SOURCE_DIR}/lua/include
        /usr/local/include/lua
        /usr/local/include
        /usr/include/lua
        /usr/include
        /opt/homebrew/include/lua
        /opt/homebrew/include
    DOC "Lua include directory"
)

find_library(LUA_LIBRARY
    NAMES lua liblua lua54 liblua54 lua5.4 liblua5.4 luau libluau
    PATHS
        ${CMAKE_SOURCE_DIR}/external/lua/lib
        ${CMAKE_SOURCE_DIR}/external/lib
        ${CMAKE_SOURCE_DIR}/lua/lib
        ${CMAKE_SOURCE_DIR}/lib
        /usr/local/lib
        /usr/lib
        /opt/homebrew/lib
    DOC "Lua library"
)

# If Lua wasn't found, we'll build it from source (no stubs)
if(NOT LUA_INCLUDE_DIR OR NOT LUA_LIBRARY)
    message(STATUS "Lua not found, building from source...")
    
    # Ensure the external directory exists
    if(NOT EXISTS ${CMAKE_SOURCE_DIR}/external/lua)
        file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/external/lua)
    endif()
    if(NOT EXISTS ${CMAKE_SOURCE_DIR}/external/lua/include)
        file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/external/lua/include)
    endif()
    if(NOT EXISTS ${CMAKE_SOURCE_DIR}/external/lua/lib)
        file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/external/lua/lib)
    endif()

    # Clone and build Lua from the repository
    include(ExternalProject)
    
    set(LUA_BUILD_DIR ${CMAKE_BINARY_DIR}/lua-build)
    set(LUA_SOURCE_DIR ${LUA_BUILD_DIR}/src/lua_external)
    
    # We'll use standard Lua 5.4 for better compatibility
    ExternalProject_Add(
        lua_external
        URL https://www.lua.org/ftp/lua-5.4.6.tar.gz
        URL_HASH SHA256=7d5ea1b9cb6aa0b59ca3dde1c6adcb57ef83a1ba8e5432c0ecd06bf439b3ad88
        PREFIX ${LUA_BUILD_DIR}
        CONFIGURE_COMMAND ""
        # Build Lua as a static library
        BUILD_COMMAND ${CMAKE_COMMAND} -E env CC=${CMAKE_C_COMPILER} "MYCFLAGS=-fPIC" make -C <SOURCE_DIR> generic
        BUILD_IN_SOURCE 1
        # Custom command to copy the built library and headers
        INSTALL_COMMAND ${CMAKE_COMMAND} -E copy_directory
            <SOURCE_DIR>/src
            ${CMAKE_SOURCE_DIR}/external/lua/include
        COMMAND ${CMAKE_COMMAND} -E copy
            <SOURCE_DIR>/src/liblua.a
            ${CMAKE_SOURCE_DIR}/external/lua/lib/liblua.a
    )
    
    # Set locations after build
    set(LUA_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/external/lua/include)
    set(LUA_LIBRARY ${CMAKE_SOURCE_DIR}/external/lua/lib/liblua.a)
    
    # Make directory for include files
    file(MAKE_DIRECTORY ${LUA_INCLUDE_DIR})
    
    # Set found flag after build
    set(LUA_FOUND TRUE)
    
    # Create imported target for Lua
    add_library(lua_imported STATIC IMPORTED GLOBAL)
    add_dependencies(lua_imported lua_external)
    set_target_properties(lua_imported PROPERTIES
        IMPORTED_LOCATION ${LUA_LIBRARY}
        INTERFACE_INCLUDE_DIRECTORIES ${LUA_INCLUDE_DIR}
    )

    # Create an alias for the imported target
    add_library(Lua::lua ALIAS lua_imported)
    
    message(STATUS "Lua will be built from source at: ${LUA_BUILD_DIR}")
    message(STATUS "Lua headers will be installed to: ${LUA_INCLUDE_DIR}")
    message(STATUS "Lua library will be installed to: ${LUA_LIBRARY}")
else()
    # If Lua was found, set the found flag
    set(LUA_FOUND TRUE)
    message(STATUS "Found existing Lua installation")
    message(STATUS "Lua include directory: ${LUA_INCLUDE_DIR}")
    message(STATUS "Lua library: ${LUA_LIBRARY}")
    
    # Create imported target for existing Lua
    if(NOT TARGET Lua::lua)
        add_library(Lua::lua UNKNOWN IMPORTED GLOBAL)
        set_target_properties(Lua::lua PROPERTIES
            IMPORTED_LOCATION "${LUA_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${LUA_INCLUDE_DIR}"
        )
    endif()
endif()

# Set libraries variable for backward compatibility
set(LUA_LIBRARIES ${LUA_LIBRARY})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Lua DEFAULT_MSG LUA_INCLUDE_DIR LUA_LIBRARIES)

mark_as_advanced(LUA_INCLUDE_DIR LUA_LIBRARIES)
