# FindLua.cmake
# This module finds Lua libraries and includes needed for our project
# This is used if the standard FindLua provided by CMake doesn't work

# Variables this module defines:
# LUA_FOUND - True if Lua was found
# LUA_INCLUDE_DIR - Directory containing Lua headers
# LUA_LIBRARIES - Libraries needed to use Lua

# Try to find Lua in standard locations
find_path(LUA_INCLUDE_DIR lua.h
  PATHS
  /usr/include
  /usr/local/include
  /opt/local/include
  /opt/homebrew/include
  /usr/local/opt/lua/include
  /usr/local/Cellar/lua/*/include
  /opt/homebrew/Cellar/lua/*/include
  PATH_SUFFIXES
  lua
  lua5.4
  lua5.3
  lua5.2
  lua5.1
  include/lua
  include/lua5.4
  include/lua5.3
  include/lua5.2
  include/lua5.1
)

# Try to find the lua library
find_library(LUA_LIBRARIES
  NAMES
  lua
  liblua
  lua5.4
  liblua5.4
  lua-5.4
  lua5.3
  liblua5.3
  lua-5.3
  lua5.2
  liblua5.2
  lua-5.2
  lua5.1
  liblua5.1
  lua-5.1
  PATHS
  /usr/lib
  /usr/local/lib
  /opt/local/lib
  /opt/homebrew/lib
  /usr/local/opt/lua/lib
  /usr/local/Cellar/lua/*/lib
  /opt/homebrew/Cellar/lua/*/lib
)

# Handle Homebrew on macOS
if(APPLE)
  execute_process(
    COMMAND brew --prefix lua
    OUTPUT_VARIABLE BREW_LUA_PREFIX
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
  )
  
  if(BREW_LUA_PREFIX)
    message(STATUS "Found Homebrew Lua at: ${BREW_LUA_PREFIX}")
    
    if(NOT LUA_INCLUDE_DIR)
      set(LUA_INCLUDE_DIR "${BREW_LUA_PREFIX}/include")
    endif()
    
    if(NOT LUA_LIBRARIES)
      find_library(LUA_LIBRARIES NAMES lua lua54 lua5.4 lua-5.4 liblua
                  PATHS "${BREW_LUA_PREFIX}/lib" NO_DEFAULT_PATH)
    endif()
  endif()
endif()

# For iOS cross-compilation
if(CMAKE_SYSTEM_NAME STREQUAL "iOS")
  # Check if lua was built for iOS (custom build is usually needed)
  # Try the standard Homebrew prefix as lua might be there
  execute_process(
    COMMAND brew --prefix
    OUTPUT_VARIABLE BREW_PREFIX
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
  )
  
  if(BREW_PREFIX)
    set(IOS_LUA_INCLUDE_DIR "${BREW_PREFIX}/include")
    set(IOS_LUA_LIBRARY "${BREW_PREFIX}/lib/liblua.a")
    
    if(EXISTS "${IOS_LUA_INCLUDE_DIR}/lua.h" AND EXISTS "${IOS_LUA_LIBRARY}")
      set(LUA_INCLUDE_DIR "${IOS_LUA_INCLUDE_DIR}")
      set(LUA_LIBRARIES "${IOS_LUA_LIBRARY}")
    endif()
  endif()
endif()

# Handle the QUIETLY and REQUIRED arguments
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Lua DEFAULT_MSG LUA_LIBRARIES LUA_INCLUDE_DIR)

mark_as_advanced(LUA_INCLUDE_DIR LUA_LIBRARIES)
