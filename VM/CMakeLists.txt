# CMakeLists.txt for the VM directory
cmake_minimum_required(VERSION 3.16)

# Include guard to prevent double inclusion
if(DEFINED LUAU_VM_INCLUDED)
    return()
endif()
set(LUAU_VM_INCLUDED TRUE)

# Enable verbose output for debugging in CI
if(DEFINED ENV{CI} OR DEFINED ENV{GITHUB_ACTIONS})
    set(CMAKE_VERBOSE_MAKEFILE ON)
endif()

# First check if all necessary files exist
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/include/lua.h" OR 
   NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/include/lualib.h" OR 
   NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/include/luaconf.h")
    message(FATAL_ERROR "Required Luau header files missing from VM/include directory")
endif()

# Make sure we aren't redefining the target
if(NOT TARGET luau_vm)
    # Create the Luau VM library
    set(VM_SOURCES
        src/lapi.cpp
        src/laux.cpp
        src/lbaselib.cpp
        src/lbitlib.cpp
        src/lbuffer.cpp
        src/lbuflib.cpp
        src/lbuiltins.cpp
        src/ldblib.cpp
        src/ldebug.cpp
        src/ldo.cpp
        src/lfunc.cpp
        src/lgc.cpp
        src/lgcdebug.cpp
        src/linit.cpp
        src/lmathlib.cpp
        src/lmem.cpp
        src/lnumprint.cpp
        src/lobject.cpp
        src/loslib.cpp
        src/lperf.cpp
        src/lstate.cpp
        src/lstring.cpp
        src/lstrlib.cpp
        src/ltable.cpp
        src/ltablib.cpp
        src/ltm.cpp
        src/ludata.cpp
        src/lutf8lib.cpp
        src/lveclib.cpp
        src/lvmexecute.cpp
        src/lvmload.cpp
        src/lvmutils.cpp
    )

    # Verify all source files exist
    foreach(SOURCE_FILE ${VM_SOURCES})
        if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE_FILE}")
            message(WARNING "Source file not found: ${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE_FILE}")
        endif()
    endforeach()

    # Create a static library from the VM sources
    add_library(luau_vm STATIC ${VM_SOURCES})

    # Set include directories
    target_include_directories(luau_vm PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src
    )

    # Set compiler flags based on platform
    if(APPLE)
        target_compile_options(luau_vm PRIVATE 
            -Wall 
            -fPIC
            -fno-exceptions 
            -fno-rtti
        )
        
        if(CMAKE_SYSTEM_NAME MATCHES "iOS")
            # Special flags for iOS
            target_compile_options(luau_vm PRIVATE
                -fembed-bitcode
                -mios-version-min=13.0
            )
        endif()
    else()
        target_compile_options(luau_vm PRIVATE 
            -Wall 
            -fPIC
            -fno-exceptions 
            -fno-rtti
        )
    endif()

    # Set compiler definitions
    target_compile_definitions(luau_vm PRIVATE 
        LUA_API=LUAI_FUNC
        LUALIB_API=LUAI_FUNC
        LUAU_FASTINT_SUPPORT=1
        USE_LUAU=1
    )

    # Create compatibility alias for Lua::lua
    add_library(Lua::lua ALIAS luau_vm)

    # Set properties to ensure library is built for the correct platform
    set_target_properties(luau_vm PROPERTIES
        POSITION_INDEPENDENT_CODE ON
        C_VISIBILITY_PRESET hidden
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN ON
    )

    # Print debug information
    message(STATUS "VM configuration:")
    message(STATUS "  Include dir: ${CMAKE_CURRENT_SOURCE_DIR}/include")
    message(STATUS "  Source files: ${VM_SOURCES}")
    message(STATUS "  System name: ${CMAKE_SYSTEM_NAME}")
    message(STATUS "  iOS build: ${IOS_TARGET}")
endif()
