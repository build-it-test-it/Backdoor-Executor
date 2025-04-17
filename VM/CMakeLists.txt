# CMakeLists.txt for the VM directory
cmake_minimum_required(VERSION 3.16)

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
