# CMakeLists.txt for source/cpp

# Define CI_BUILD for all files
add_definitions(-DCI_BUILD)

# Include Lua headers
include_directories(
    ${CMAKE_SOURCE_DIR}/source/cpp/luau
)

# Only include a minimal set of source files for CI build
file(GLOB_RECURSE COMMON_SOURCES
    "memory/*.cpp"
)

# Create the static library
add_library(roblox_execution STATIC ${COMMON_SOURCES})

# Link with Lua
target_link_libraries(roblox_execution
    lua_bundled
)

# Print what we're building
message(STATUS "Building with CI stub implementations")
