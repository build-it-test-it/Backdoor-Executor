# Define CI_BUILD for all compiler instances
add_definitions(-DCI_BUILD)

cmake_minimum_required(VERSION 3.10)
project(RobloxExecutor VERSION 1.0.0 LANGUAGES CXX C)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Create output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/source
    ${CMAKE_SOURCE_DIR}/source/cpp
    ${CMAKE_SOURCE_DIR}/source/cpp/luau
    ${CMAKE_BINARY_DIR}
)

# Build Luau from source as a static library
file(GLOB LUAU_SOURCES 
    "source/cpp/luau/*.cpp"
)

# Create Lua bundled library
add_library(lua_bundled STATIC ${LUAU_SOURCES})
target_include_directories(lua_bundled PUBLIC
    ${CMAKE_SOURCE_DIR}/source/cpp/luau
)

# Create a symlink target to ensure the library is available
add_custom_target(ensure_lua_path ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/lib
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:lua_bundled> ${CMAKE_BINARY_DIR}/lib/liblua.dylib
    DEPENDS lua_bundled
)

# Build lfs.c as a separate object
add_library(lfs_obj OBJECT source/lfs.c)
target_include_directories(lfs_obj PRIVATE
    ${CMAKE_SOURCE_DIR}/source/cpp/luau
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/source
)
target_compile_definitions(lfs_obj PRIVATE LUA_COMPAT_5_1=1)
set_target_properties(lfs_obj PROPERTIES C_STANDARD 99 POSITION_INDEPENDENT_CODE ON)

# Add subdirectories
add_subdirectory(source/cpp)

# Create the dynamic library
add_library(roblox_executor SHARED
    source/library.cpp
    $<TARGET_OBJECTS:lfs_obj>
)

# Link with libraries
target_link_libraries(roblox_executor 
    lua_bundled
    roblox_execution
)

# Add dependencies
add_dependencies(roblox_executor lua_bundled ensure_lua_path)

# Set output name
set_target_properties(roblox_executor PROPERTIES
    OUTPUT_NAME "roblox_executor"
    PREFIX ""
)

# Print build information
message(STATUS "Building with CI stub implementations")
message(STATUS "Using bundled Lua library for link time")
