# CMakeLists.txt for source/cpp

# Determine if this is a CI build
if(DEFINED ENV{CI} OR DEFINED BUILD_CI OR DEFINED CI_BUILD)
  set(CI_BUILD TRUE)
  add_definitions(-DCI_BUILD)
  message(STATUS "source/cpp: CI Build detected - using conditional compilation")
else()
  set(CI_BUILD FALSE)
  message(STATUS "source/cpp: Normal build detected")
endif()

# Include Lua headers
include_directories(
    ${CMAKE_SOURCE_DIR}/source/cpp/luau
)

# Collect all source files
file(GLOB_RECURSE CPP_SOURCES
    "memory/*.cpp"
    "memory/*.c"
    "hooks/*.cpp"
    "hooks/*.c"
)

# For iOS-specific code, we use conditional compilation
file(GLOB_RECURSE IOS_SOURCES
    "ios/*.cpp"
    "ios/*.mm"
    "ios/*/*.cpp"
    "ios/*/*.mm"
)

if(NOT CI_BUILD)
    # On real builds, include all iOS files
    list(APPEND CPP_SOURCES ${IOS_SOURCES})
    message(STATUS "Including all iOS source files for full build")
else()
    # On CI builds, only include iOS files with CI_BUILD guards
    message(STATUS "CI build: Some iOS files may be excluded")
    
    # Create an empty stub.cpp if needed for empty library
    if(NOT CPP_SOURCES)
        file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/stub.cpp 
        "#include <iostream>\nextern \"C\" void roblox_execution_stub() { std::cout << \"Stub function called\" << std::endl; }\n")
        list(APPEND CPP_SOURCES ${CMAKE_CURRENT_BINARY_DIR}/stub.cpp)
    endif()
endif()

# Debug output
message(STATUS "Source files: ${CPP_SOURCES}")

# Create the static library
add_library(roblox_execution STATIC ${CPP_SOURCES})

# Link with Lua
target_link_libraries(roblox_execution
    lua_bundled
)

# Find Dobby and link if available
find_package(Dobby QUIET)
if(Dobby_FOUND)
    target_link_libraries(roblox_execution Dobby::dobby)
endif()

# Set include directories
target_include_directories(roblox_execution PUBLIC
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/source
    ${CMAKE_SOURCE_DIR}/source/cpp
    ${CMAKE_SOURCE_DIR}/source/cpp/luau
)

# Also define CI_BUILD at compile time if needed
if(CI_BUILD)
    target_compile_definitions(roblox_execution PRIVATE -DCI_BUILD)
endif()
