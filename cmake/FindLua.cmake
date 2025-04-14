# FindLua.cmake for Homebrew Luau
# This module finds Luau libraries and includes installed via Homebrew

# Variables this module defines:
# LUA_FOUND - True if Luau was found
# LUA_INCLUDE_DIR - Directory containing Luau headers
# LUA_LIBRARIES - Libraries needed to use Luau

# Try to get from environment variables first
if(DEFINED ENV{LUAU_INCLUDE_DIR} AND DEFINED ENV{LUA_LIBRARIES})
  set(LUA_INCLUDE_DIR $ENV{LUAU_INCLUDE_DIR})
  set(LUA_LIBRARIES $ENV{LUA_LIBRARIES})
  set(LUA_FOUND TRUE)
  message(STATUS "Using Luau from environment variables")
  message(STATUS "Luau include dir: ${LUA_INCLUDE_DIR}")
  message(STATUS "Luau libraries: ${LUA_LIBRARIES}")
else()
  # Try to find using Homebrew
  message(STATUS "Looking for Homebrew Luau installation")
  
  execute_process(
    COMMAND brew --prefix luau
    OUTPUT_VARIABLE LUAU_PREFIX
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
  )
  
  if(LUAU_PREFIX)
    message(STATUS "Found Homebrew Luau at: ${LUAU_PREFIX}")
    
    # Set include directory
    set(LUA_INCLUDE_DIR "${LUAU_PREFIX}/include")
    
    # Find the library - try multiple variations of names
    find_library(LUA_LIBRARIES
      NAMES luau lua liblua libluau
      PATHS "${LUAU_PREFIX}/lib"
      NO_DEFAULT_PATH
    )
    
    # If library not found directly, try with the default name or search for any .dylib
    if(NOT LUA_LIBRARIES)
      file(GLOB LUAU_LIBS "${LUAU_PREFIX}/lib/*.dylib")
      if(LUAU_LIBS)
        list(GET LUAU_LIBS 0 FIRST_LIB)
        set(LUA_LIBRARIES "${FIRST_LIB}")
        message(STATUS "Using found Luau library: ${LUA_LIBRARIES}")
      else()
        file(GLOB LUAU_STATIC_LIBS "${LUAU_PREFIX}/lib/*.a")
        if(LUAU_STATIC_LIBS)
          list(GET LUAU_STATIC_LIBS 0 FIRST_STATIC_LIB)
          set(LUA_LIBRARIES "${FIRST_STATIC_LIB}")
          message(STATUS "Using found Luau static library: ${LUA_LIBRARIES}")
        else()
          # Default fallback options
          if(EXISTS "${LUAU_PREFIX}/lib/liblua.dylib")
            set(LUA_LIBRARIES "${LUAU_PREFIX}/lib/liblua.dylib")
          elseif(EXISTS "${LUAU_PREFIX}/lib/libluau.dylib")
            set(LUA_LIBRARIES "${LUAU_PREFIX}/lib/libluau.dylib")
          else()
            message(WARNING "Could not find any Luau library in ${LUAU_PREFIX}/lib")
          endif()
          message(STATUS "Using hardcoded Luau library path: ${LUA_LIBRARIES}")
        endif()
      endif()
    endif()
    
    set(LUA_FOUND TRUE)
  else()
    message(WARNING "Homebrew Luau not found. Please install with: brew install luau")
    set(LUA_FOUND FALSE)
  endif()
endif()

# Handle the QUIETLY and REQUIRED arguments
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Lua DEFAULT_MSG LUA_LIBRARIES LUA_INCLUDE_DIR)

mark_as_advanced(LUA_INCLUDE_DIR LUA_LIBRARIES)
