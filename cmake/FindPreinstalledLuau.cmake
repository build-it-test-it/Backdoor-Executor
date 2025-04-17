# FindPreinstalledLuau.cmake - Find pre-installed Luau libraries
# This module looks for pre-installed Luau libraries and sets up targets to use them
#
# Variables defined by this module:
# LUAU_FOUND          - True if Luau was found
# LUAU_INCLUDE_DIR    - Directory containing Luau headers
# LUAU_VM_LIBRARY     - Path to the Luau VM library (libLuau.VM.a)
# LUAU_COMPILER_LIBRARY - Path to the Luau Compiler library (libLuau.Compiler.a)
# LUAU_LIBRARIES      - All libraries needed to use Luau
#
# Targets created:
# Luau::VM            - Target for the Luau VM library
# Luau::Compiler      - Target for the Luau Compiler library (optional)
# Luau::lua           - Combined target for both libraries (compatible with FindLua)

# Configure paths to pre-installed Luau
# User can override these before including this file
if(NOT DEFINED LUAU_ROOT)
    set(LUAU_ROOT "${CMAKE_SOURCE_DIR}/external/luau" CACHE PATH "Root directory of the Luau installation")
endif()

# Set default paths based on standard Luau build layout
if(NOT DEFINED LUAU_INCLUDE_DIR)
    set(LUAU_INCLUDE_DIR "${LUAU_ROOT}/VM/include" CACHE PATH "Directory containing Luau headers")
endif()

if(NOT DEFINED LUAU_VM_LIBRARY)
    # Try common paths for the VM library
    if(EXISTS "${LUAU_ROOT}/build/libLuau.VM.a")
        set(LUAU_VM_LIBRARY "${LUAU_ROOT}/build/libLuau.VM.a" CACHE FILEPATH "Path to Luau VM library")
    elseif(EXISTS "${LUAU_ROOT}/build/Luau.VM.a")
        set(LUAU_VM_LIBRARY "${LUAU_ROOT}/build/Luau.VM.a" CACHE FILEPATH "Path to Luau VM library")
    endif()
endif()

if(NOT DEFINED LUAU_COMPILER_LIBRARY)
    # Try common paths for the Compiler library
    if(EXISTS "${LUAU_ROOT}/build/libLuau.Compiler.a")
        set(LUAU_COMPILER_LIBRARY "${LUAU_ROOT}/build/libLuau.Compiler.a" CACHE FILEPATH "Path to Luau Compiler library")
    elseif(EXISTS "${LUAU_ROOT}/build/Luau.Compiler.a")
        set(LUAU_COMPILER_LIBRARY "${LUAU_ROOT}/build/Luau.Compiler.a" CACHE FILEPATH "Path to Luau Compiler library")
    endif()
endif()

# Log paths for debugging
message(STATUS "Looking for Luau in the following locations:")
message(STATUS "  LUAU_ROOT          : ${LUAU_ROOT}")
message(STATUS "  LUAU_INCLUDE_DIR   : ${LUAU_INCLUDE_DIR}")
message(STATUS "  LUAU_VM_LIBRARY    : ${LUAU_VM_LIBRARY}")
message(STATUS "  LUAU_COMPILER_LIBRARY : ${LUAU_COMPILER_LIBRARY}")

# Verify files exist
set(LUAU_FOUND FALSE)

# Check for the include directory and key header files
if(EXISTS "${LUAU_INCLUDE_DIR}")
    if(EXISTS "${LUAU_INCLUDE_DIR}/lua.h" AND 
       EXISTS "${LUAU_INCLUDE_DIR}/luaconf.h" AND 
       EXISTS "${LUAU_INCLUDE_DIR}/lualib.h" AND 
       EXISTS "${LUAU_INCLUDE_DIR}/lauxlib.h")
        message(STATUS "Found Luau headers in ${LUAU_INCLUDE_DIR}")
    else()
        message(WARNING "Luau include directory exists but missing some headers")
        set(LUAU_INCLUDE_MISSING TRUE)
    endif()
else()
    message(WARNING "Luau include directory not found: ${LUAU_INCLUDE_DIR}")
    set(LUAU_INCLUDE_MISSING TRUE)
endif()

# Check for the VM library (required)
if(EXISTS "${LUAU_VM_LIBRARY}")
    message(STATUS "Found Luau VM library: ${LUAU_VM_LIBRARY}")
else()
    message(WARNING "Luau VM library not found: ${LUAU_VM_LIBRARY}")
    set(LUAU_VM_MISSING TRUE)
endif()

# Check for the Compiler library (optional)
if(EXISTS "${LUAU_COMPILER_LIBRARY}")
    message(STATUS "Found Luau Compiler library: ${LUAU_COMPILER_LIBRARY}")
    set(LUAU_HAVE_COMPILER TRUE)
else()
    message(STATUS "Luau Compiler library not found, will only use VM library")
    set(LUAU_HAVE_COMPILER FALSE)
endif()

# Set result variables
if(NOT LUAU_INCLUDE_MISSING AND NOT LUAU_VM_MISSING)
    set(LUAU_FOUND TRUE)
    
    # Set up the libraries list with the VM library (required)
    set(LUAU_LIBRARIES "${LUAU_VM_LIBRARY}")
    
    # Add Compiler library if available
    if(LUAU_HAVE_COMPILER)
        list(APPEND LUAU_LIBRARIES "${LUAU_COMPILER_LIBRARY}")
    endif()
    
    message(STATUS "Luau found: ${LUAU_FOUND}")
else()
    message(FATAL_ERROR "Could not find required Luau files. Please check paths.")
endif()

# Create imported targets for Luau libraries
if(NOT TARGET Luau::VM AND LUAU_FOUND)
    add_library(Luau::VM STATIC IMPORTED)
    set_target_properties(Luau::VM PROPERTIES
        IMPORTED_LOCATION "${LUAU_VM_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${LUAU_INCLUDE_DIR}"
    )
    message(STATUS "Created Luau::VM imported target")
    
    if(LUAU_HAVE_COMPILER)
        add_library(Luau::Compiler STATIC IMPORTED)
        set_target_properties(Luau::Compiler PROPERTIES
            IMPORTED_LOCATION "${LUAU_COMPILER_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${LUAU_INCLUDE_DIR}"
        )
        message(STATUS "Created Luau::Compiler imported target")
    endif()
    
    # Create a combined target for compatibility with the FindLua module
    add_library(Luau::lua INTERFACE)
    target_link_libraries(Luau::lua INTERFACE Luau::VM)
    if(LUAU_HAVE_COMPILER)
        target_link_libraries(Luau::lua INTERFACE Luau::Compiler)
    endif()
    
    # Also create Lua::lua alias for backwards compatibility with standard FindLua
    if(NOT TARGET Lua::lua)
        add_library(Lua::lua ALIAS Luau::lua)
    endif()
    
    message(STATUS "Created Luau::lua interface target")
endif()

# For FindLua.cmake compatibility
set(LUA_FOUND ${LUAU_FOUND})
set(LUA_INCLUDE_DIR ${LUAU_INCLUDE_DIR})
set(LUA_LIBRARIES ${LUAU_LIBRARIES})
set(LUA_VERSION_STRING "Luau (Roblox Lua)")

# Handle the QUIETLY and REQUIRED arguments and set LUAU_FOUND to TRUE
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Luau
    REQUIRED_VARS LUAU_VM_LIBRARY LUAU_INCLUDE_DIR
    VERSION_VAR LUA_VERSION_STRING
)

mark_as_advanced(
    LUAU_INCLUDE_DIR
    LUAU_VM_LIBRARY
    LUAU_COMPILER_LIBRARY
    LUAU_LIBRARIES
)
