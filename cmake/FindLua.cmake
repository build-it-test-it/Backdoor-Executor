# FindLua.cmake for iOS Roblox Executor
# This module finds Lua libraries and includes for use in the project

# Variables this module defines:
# LUA_FOUND - True if Lua was found
# LUA_INCLUDE_DIR - Directory containing Lua headers
# LUA_LIBRARIES - Libraries needed to use Lua

# For CI/CD builds, we'll use a simplified approach
if(DEFINED ENV{CI} OR DEFINED GITHUB_ACTIONS)
  message(STATUS "Running in CI environment. Using built-in Lua headers.")
  set(LUA_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/cmake")
  set(LUA_LIBRARIES "lua_not_required_for_ci")
  set(LUA_FOUND TRUE)
else
  # Try to get from environment variables first
  if(DEFINED ENV{LUA_INCLUDE_DIR} AND DEFINED ENV{LUA_LIBRARIES})
    set(LUA_INCLUDE_DIR $ENV{LUA_INCLUDE_DIR})
    set(LUA_LIBRARIES $ENV{LUA_LIBRARIES})
    set(LUA_FOUND TRUE)
    message(STATUS "Using Lua from environment variables")
    message(STATUS "Lua include dir: ${LUA_INCLUDE_DIR}")
    message(STATUS "Lua libraries: ${LUA_LIBRARIES}")
  else
    # Try to find using Homebrew
    message(STATUS "Looking for Homebrew Lua installation")
    
    execute_process(
      COMMAND brew --prefix lua
      OUTPUT_VARIABLE LUA_PREFIX
      OUTPUT_STRIP_TRAILING_WHITESPACE
      ERROR_QUIET
    )
    
    if(LUA_PREFIX)
      message(STATUS "Found Homebrew Lua at: ${LUA_PREFIX}")
      
      # Set include directory
      set(LUA_INCLUDE_DIR "${LUA_PREFIX}/include")
      
      # Find the library
      find_library(LUA_LIBRARIES
        NAMES lua liblua lua5.1 liblua5.1
        PATHS 
          "${LUA_PREFIX}/lib"
          "/usr/local/lib"
          "/opt/homebrew/lib"
          "/usr/lib"
      )
      
      # Always report what we're using
      message(STATUS "Using Lua include dir: ${LUA_INCLUDE_DIR}")
      message(STATUS "Using Lua libraries: ${LUA_LIBRARIES}")
      
      set(LUA_FOUND TRUE)
    else
      # For non-homebrew systems, try standard locations
      find_path(LUA_INCLUDE_DIR
        NAMES lua.h
        PATHS
          /usr/include
          /usr/local/include
          /opt/local/include
        PATH_SUFFIXES lua lua5.1
      )
      
      find_library(LUA_LIBRARIES
        NAMES lua liblua lua5.1 liblua5.1
        PATHS 
          /usr/lib
          /usr/local/lib
          /opt/local/lib
      )
      
      if(LUA_INCLUDE_DIR AND LUA_LIBRARIES)
        set(LUA_FOUND TRUE)
        message(STATUS "Found Lua in standard locations")
        message(STATUS "Using Lua include dir: ${LUA_INCLUDE_DIR}")
        message(STATUS "Using Lua libraries: ${LUA_LIBRARIES}")
      else
        # Use built-in as fallback
        message(STATUS "Using built-in Lua headers as fallback")
        set(LUA_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/cmake")
        set(LUA_LIBRARIES "lua_built_in")
        set(LUA_FOUND TRUE)
      endif()
    endif()
  endif()
endif()

# Handle the QUIETLY and REQUIRED arguments
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Lua DEFAULT_MSG LUA_LIBRARIES LUA_INCLUDE_DIR)

mark_as_advanced(LUA_INCLUDE_DIR LUA_LIBRARIES)
