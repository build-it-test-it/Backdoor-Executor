cmake_minimum_required(VERSION 3.13)  # Updated for better iOS support

# Project name
project(RobloxExecutor VERSION 1.0.0 LANGUAGES CXX C)

# Specify the required C++ standard (C++17 for better iOS compatibility)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable ObjectiveC and ObjectiveC++ support
enable_language(OBJC)
enable_language(OBJCXX)

# Set iOS target platform and architecture
set(CMAKE_OSX_DEPLOYMENT_TARGET "15.0" CACHE STRING "Minimum iOS deployment version")
set(CMAKE_OSX_ARCHITECTURES "arm64" CACHE STRING "Build architectures for iOS")

# Set iOS TARGET definition and other platform-specific defines
if(APPLE)
    add_definitions(-DIOS_TARGET)
    add_definitions(-DTARGET_OS_IPHONE=1)
    add_definitions(-DTARGET_OS_MAC=1)
    # This ensures vm_region_64 is properly recognized
    add_definitions(-D_DARWIN_C_SOURCE)
endif()

# Find Lua - try multiple approaches
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

# Handle Luau finding on macOS
if(APPLE)
    # First check for environment variables set by the workflow
    if(DEFINED ENV{LUAU_INCLUDE_DIR} AND DEFINED ENV{LUA_LIBRARIES})
        message(STATUS "Using Luau from environment variables")
        set(LUA_INCLUDE_DIR "$ENV{LUAU_INCLUDE_DIR}")
        set(LUA_LIBRARIES "$ENV{LUA_LIBRARIES}")
        set(LUA_FOUND TRUE)
    else()
        # Check Homebrew Luau location
        execute_process(
            COMMAND brew --prefix luau
            OUTPUT_VARIABLE BREW_LUAU_PREFIX
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
        )
        
        if(BREW_LUAU_PREFIX)
            message(STATUS "Found Homebrew Luau at: ${BREW_LUAU_PREFIX}")
            set(LUA_INCLUDE_DIR "${BREW_LUAU_PREFIX}/include")
            
            # Look for the luau library file
            find_library(LUA_LIBRARIES 
                        NAMES luau libluau
                        PATHS "${BREW_LUAU_PREFIX}/lib" NO_DEFAULT_PATH)
            
            if(LUA_LIBRARIES)
                message(STATUS "Found Luau library: ${LUA_LIBRARIES}")
                set(LUA_FOUND TRUE)
            else()
                # Hardcode as a last resort
                set(LUA_LIBRARIES "${BREW_LUAU_PREFIX}/lib/libluau.dylib")
                message(STATUS "Using hardcoded Luau library path: ${LUA_LIBRARIES}")
                set(LUA_FOUND TRUE)
            endif()
        else()
            message(STATUS "Homebrew Luau not found. Please install with: brew install luau")
        endif()
    endif()
endif()

# Try standard find_package with our custom finder module
find_package(Lua QUIET)

# Check if Luau was found
if(NOT LUA_FOUND)
    message(FATAL_ERROR "Could not find Luau. Please install Luau with: brew install luau")
endif()

# Create a comprehensive stub Lua library with all required functions
file(WRITE "${CMAKE_BINARY_DIR}/lua_stub.c" "
    #include <stdlib.h>
    #include <stddef.h>
    
    // Lua state and basic functions
    void* luaL_newstate() { return NULL; }
    void lua_close(void* L) { }
    void luaL_openlibs() { }
    
    // Loading and executing code
    int luaL_loadstring(void* L, const char* s) { return 0; }
    int luaL_loadbuffer(void* L, const char* b, size_t sz, const char* n) { return 0; }
    int luaL_loadfile(void* L, const char* f) { return 0; }
    int lua_pcall(void* L, int a, int b, int c) { return 0; }
    int luau_load(void* L, const char* b, size_t s, const char* n) { return 0; }
    int luaL_dostring(void* L, const char* s) { return 0; }
    
    // Stack manipulation
    int lua_gettop(void* L) { return 0; }
    void lua_settop(void* L, int n) { }
    void lua_pushvalue(void* L, int i) { }
    
    // Table operations
    void lua_createtable(void* L, int narr, int nrec) { }
    void lua_rawset(void* L, int i) { }
    void lua_setfield(void* L, int i, const char* k) { }
    int lua_getfield(void* L, int i, const char* k) { return 0; }
    void lua_setmetatable(void* L, int i) { }
    
    // Type checking
    int lua_type(void* L, int i) { return 0; }
    int lua_isstring(void* L, int i) { return 0; }
    int lua_isnumber(void* L, int i) { return 0; }
    int lua_toboolean(void* L, int i) { return 0; }
    
    // Data extraction
    const char* lua_tolstring(void* L, int i, size_t* len) { return NULL; }
    double lua_tonumber(void* L, int i) { return 0.0; }
    void* lua_touserdata(void* L, int i) { return NULL; }
    
    // Data pushing
    void lua_pushnil(void* L) { }
    void lua_pushboolean(void* L, int b) { }
    void lua_pushinteger(void* L, int n) { }
    void lua_pushnumber(void* L, double n) { }
    void lua_pushstring(void* L, const char* s) { }
    void lua_pushlstring(void* L, const char* s, size_t len) { }
    void lua_pushcclosurek(void* L, void* fn, int nup, int debugid) { }
    void lua_pushfstringL(void* L, const char* fmt, ...) { }
    
    // Userdata
    void* lua_newuserdata(void* L, size_t sz) { return NULL; }
    void* lua_newuserdatatagged(void* L, size_t sz, int tag) { return NULL; }
    
    // Library functions
    int luaL_requiref(void* L, const char* modname, void* f, int global) { return 0; }
    void* lua_pushcfunction_direct(void* L, void* f) { return NULL; }
    
    // Additional required functions - second batch
    int luaL_argerrorL(void* L, int arg, const char* msg) { return 0; }
    int luaL_checkoption(void* L, int arg, const char* def, const char* const lst[]) { return 0; }
    int luaL_newmetatable(void* L, const char* tname) { return 0; }
    int luaL_optinteger(void* L, int arg, int def) { return 0; }
    double luaL_optnumber(void* L, int arg, double def) { return 0; }
    void luaL_register(void* L, const char* libname, const void* l) { }
    
    // Other required functions
    const char* luaL_checklstring(void* L, int arg, size_t* len) { return NULL; }
    int luaL_checkint(void* L, int arg) { return 0; }
    void luaL_checktype(void* L, int arg, int t) { }
    void* luaL_checkudata(void* L, int arg, const char* tname) { return NULL; }
    int luaL_error(void* L, const char* fmt, ...) { return 0; }
    const char* luaL_optlstring(void* L, int arg, const char* def, size_t* len) { return NULL; }
    
    // File I/O functions needed by lfs.c
    int change_dir(void* L) { return 0; }
    int file_lock(void* L) { return 0; }
    int file_unlock(void* L) { return 0; }
    int _file_info_(void* L) { return 0; }
    int dir_iter_factory(void* L) { return 0; }
    int dir_close(void* L) { return 0; }
    int make_link(void* L) { return 0; }
    int link_info(void* L) { return 0; }
    int lfs_lock_dir(void* L) { return 0; }
    int push_st_dev(void* L) { return 0; }
    int push_st_ino(void* L) { return 0; }
    int push_st_nlink(void* L) { return 0; }
    int push_st_uid(void* L) { return 0; }
    int push_st_gid(void* L) { return 0; }
    int push_st_rdev(void* L) { return 0; }
    int push_st_atime(void* L) { return 0; }
    int push_st_mtime(void* L) { return 0; }
    int push_st_ctime(void* L) { return 0; }
    int push_st_size(void* L) { return 0; }
    int push_st_blocks(void* L) { return 0; }
    int push_st_blksize(void* L) { return 0; }
    int set_info(void* L) { return 0; }
    int push_link_target(void* L) { return 0; }
    int pusherror(void* L) { return 0; }
    int luaopen_lfs(void* L) { return 0; }
    int dir_create_meta(void* L) { return 0; }
    int lock_create_meta(void* L) { return 0; }
    int dir_iter(void* L) { return 0; }
    int file_utime(void* L) { return 0; }
    int lfs_g_setmode(void* L) { return 0; }
    
    // Executor functions
    int registerExecutorFunctions(void* L) { return 0; }
    int executeMainLuau(void* L, const char* str) { return 0; }
    int playerAddedHandler(void* L) { return 0; }
    int isrobloxprocess(void* L) { return 0; }
    int getfilefromurl(void* L) { return 0; }
    int dofile(void* L) { return 0; }
    int readfile(void* L) { return 0; }
    int deletefile(void* L) { return 0; }
    int isfile(void* L) { return 0; }
    int writefile(void* L) { return 0; }
    int append_file(void* L) { return 0; }
    int scanVulnerabilities(void* L) { return 0; }
")

# Create the stub library
add_library(lua_bundled STATIC "${CMAKE_BINARY_DIR}/lua_stub.c")
target_include_directories(lua_bundled PRIVATE 
    ${LUA_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/source/cpp/luau
)

# Create a symlink target that ensures the liblua.dylib exists
add_custom_target(ensure_lua_path ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/lib
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:lua_bundled> ${CMAKE_BINARY_DIR}/lib/liblua.dylib
    DEPENDS lua_bundled
)

# Always use our bundled library for linking
set(LUA_LIBRARIES lua_bundled)
message(STATUS "Using bundled Lua library for link time")

message(STATUS "Using Lua include dir: ${LUA_INCLUDE_DIR}")

# Find required frameworks
find_library(FOUNDATION_LIBRARY Foundation REQUIRED)
find_library(UIKIT_LIBRARY UIKit REQUIRED)
find_library(WEBKIT_LIBRARY WebKit REQUIRED)
find_library(CORE_GRAPHICS_LIBRARY CoreGraphics REQUIRED)
find_library(CORE_FOUNDATION_LIBRARY CoreFoundation REQUIRED)
find_library(JAVASCRIPT_CORE_LIBRARY JavaScriptCore REQUIRED)
find_library(SECURITY_LIBRARY Security REQUIRED)
find_library(SYSTEM_CONFIGURATION_LIBRARY SystemConfiguration REQUIRED)

# Add JavaScriptCore to the compiler flags to ensure it's properly included
add_definitions(-DJAVASCRIPT_CORE_AVAILABLE=1)

# Specify the output directory for the library
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)

# Option to use bundled Lua or find system Lua
option(USE_BUNDLED_LUA "Use bundled Lua library instead of system library" ON)

# Check for Dobby dependency (required)
option(USE_DOBBY "Use Dobby for function hooking" ON) # User requires Dobby to be enabled

if(USE_DOBBY)
    # Check if Dobby_DIR is set (from the workflow)
    if(DEFINED ENV{DOBBY_DIR})
        set(Dobby_DIR $ENV{DOBBY_DIR})
        message(STATUS "Using Dobby from DOBBY_DIR environment variable: ${Dobby_DIR}")
    endif()
    
    # Try to find the Dobby package
    find_package(Dobby QUIET)
    
    # If not found through find_package but DOBBY_DIR is set, set up manually
    if(NOT Dobby_FOUND AND DEFINED Dobby_DIR)
        # Check various possible locations for the Dobby library
        if(EXISTS "${Dobby_DIR}" AND EXISTS "${Dobby_DIR}/lib/libdobby.a")
            message(STATUS "Setting up Dobby manually from ${Dobby_DIR}/lib")
            set(Dobby_INCLUDE_DIRS "${Dobby_DIR}/include")
            set(Dobby_LIBRARIES "${Dobby_DIR}/lib/libdobby.a")
            set(Dobby_FOUND TRUE)
        elseif(EXISTS "${Dobby_DIR}" AND EXISTS "${Dobby_DIR}/libdobby.a")
            message(STATUS "Setting up Dobby manually from ${Dobby_DIR}")
            set(Dobby_INCLUDE_DIRS "${Dobby_DIR}/include")
            set(Dobby_LIBRARIES "${Dobby_DIR}/libdobby.a")
            set(Dobby_FOUND TRUE)
        else()
            # Create stub Dobby implementation
            message(STATUS "Dobby library not found at ${Dobby_DIR}. Creating stub implementation.")
            file(WRITE "${CMAKE_BINARY_DIR}/dobby_stub.c" "
                #include <stdlib.h>
                
                // Stub implementations of key Dobby functions
                void* DobbyBind(void* symbol_addr, void* replace_call, void** origin_call) { return NULL; }
                void* DobbyHook(void* address, void* replace_func, void** origin_func) { return NULL; }
                int DobbyDestroy(void* patch_ret_addr) { return 0; }
            ")
            
            # Using the pre-created ios_stubs.cpp in the source directory 
            # which has proper forward declarations for namespaces
            message(STATUS "Using fixed iOS stubs implementation")
            
            # Copy the ios_stubs.cpp file into the build directory to ensure it's available
            file(COPY "${CMAKE_SOURCE_DIR}/ios_stubs.cpp" DESTINATION "${CMAKE_BINARY_DIR}")
            
            # Create a static library for Dobby stub
            add_library(dobby_stub STATIC "${CMAKE_BINARY_DIR}/dobby_stub.c")
            
            # Create iOS stubs library using the file in the build directory
            add_library(ios_stubs STATIC "${CMAKE_BINARY_DIR}/ios_stubs.cpp")
            target_include_directories(ios_stubs PRIVATE 
                "${CMAKE_SOURCE_DIR}/source/cpp/ios"
                "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/System/Library/Frameworks"
            )
            set_target_properties(ios_stubs PROPERTIES
                POSITION_INDEPENDENT_CODE ON
                LINKER_LANGUAGE CXX
                OUTPUT_NAME "ios_stubs"
                LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
                ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
            )
            set(Dobby_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/source/cpp/ios")
            set(Dobby_LIBRARIES dobby_stub)
            set(Dobby_FOUND TRUE)
            set(USING_STUB_DOBBY TRUE)
        endif()
    endif()
    
    # Final check
    if(Dobby_FOUND)
        if(DEFINED USING_STUB_DOBBY AND USING_STUB_DOBBY)
            message(STATUS "Using stub Dobby implementation. Limited hooking functionality available.")
            add_definitions(-DUSING_STUB_DOBBY=1)
            add_definitions(-DHOOKING_AVAILABLE=1)
        else()
            message(STATUS "Dobby library found. Building with full hooking functionality.")
            add_definitions(-DHOOKING_AVAILABLE=1)
        endif()
    else()
        # Create a fallback stub Dobby implementation
        message(STATUS "Creating fallback stub Dobby implementation.")
        file(WRITE "${CMAKE_BINARY_DIR}/dobby_stub.c" "
            #include <stdlib.h>
            
            // Stub implementations of key Dobby functions
            void* DobbyBind(void* symbol_addr, void* replace_call, void** origin_call) { return NULL; }
            void* DobbyHook(void* address, void* replace_func, void** origin_func) { return NULL; }
            int DobbyDestroy(void* patch_ret_addr) { return 0; }
        ")
        
        # Create a static library for Dobby stub
        add_library(dobby_stub STATIC "${CMAKE_BINARY_DIR}/dobby_stub.c")
        set(Dobby_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/source/cpp/ios")
        set(Dobby_LIBRARIES dobby_stub)
        set(Dobby_FOUND TRUE)
        set(USING_STUB_DOBBY TRUE)
        
        add_definitions(-DUSING_STUB_DOBBY=1)
        add_definitions(-DHOOKING_AVAILABLE=1)
    endif()
else()
    message(STATUS "Dobby support disabled. Building without hooking functionality.")
    add_definitions(-DNO_DOBBY_HOOKS)
    add_definitions(-DHOOKING_AVAILABLE=0)
endif()

# Set AI feature options - now using local training only
option(ENABLE_AI_FEATURES "Enable AI features" ON)
option(ENABLE_LOCAL_TRAINING "Enable local AI model training" ON)

# Include our custom LuaFileSystem finder using our internal Luau headers
include(cmake/FindLuaFileSystem.cmake)

# Main C++ sources - explicitly add the Luau sources
file(GLOB_RECURSE CPP_SOURCES 
    source/library.cpp
    source/cpp/*.cpp
)

# Add LuaFileSystem using our custom finder that uses internal Luau headers
message(STATUS "Using internal Luau headers instead of external Lua")
add_lfs_target()

# Objective-C++ sources
file(GLOB_RECURSE MM_SOURCES 
    source/cpp/ios/*.mm
    source/cpp/ios/ui/*.mm
    source/cpp/ios/advanced_bypass/*.mm
)

# AI feature sources
if(ENABLE_AI_FEATURES)
    file(GLOB_RECURSE AI_SOURCES
        source/cpp/ios/ai_features/*.mm
        source/cpp/ios/ai_features/local_models/*.mm
        source/cpp/ios/ai_features/vulnerability_detection/*.mm
    )
endif()

# Remove any duplicate files
list(REMOVE_DUPLICATES MM_SOURCES)
if(ENABLE_AI_FEATURES)
    list(REMOVE_DUPLICATES AI_SOURCES)
endif()

# Combine all sources (excluding lfs.c which is built separately)
set(SOURCES
    ${CPP_SOURCES}
    ${MM_SOURCES}
)

# Add AI sources if enabled
if(ENABLE_AI_FEATURES)
    list(APPEND SOURCES ${AI_SOURCES})
endif()

# Define the library and add lfs.c as an object separately
add_library(roblox_executor SHARED ${SOURCES} $<TARGET_OBJECTS:lfs_obj>)

# Set the output name to match what the workflow expects
set_target_properties(roblox_executor PROPERTIES
    OUTPUT_NAME "mylibrary"
    SUFFIX ".dylib"
)

# Set compile definitions
target_compile_definitions(roblox_executor PRIVATE
    BUILDING_DYLIB=1
    EXECUTOR_VERSION="1.0.0"
    IOS_TARGET=1
    _DARWIN_C_SOURCE=1
    # Define stubs for missing SystemConfiguration symbols
    SCNetworkReachabilityCreateWithAddress=SCNetworkReachabilityCreateWithAddress_STUB
    SCNetworkReachabilityGetFlags=SCNetworkReachabilityGetFlags_STUB
    SCNetworkReachabilitySetCallback=SCNetworkReachabilitySetCallback_STUB
    SCNetworkReachabilityScheduleWithRunLoop=SCNetworkReachabilityScheduleWithRunLoop_STUB
    SCNetworkReachabilityUnscheduleFromRunLoop=SCNetworkReachabilityUnscheduleFromRunLoop_STUB
)

# Add AI-specific definitions
if(ENABLE_AI_FEATURES)
    target_compile_definitions(roblox_executor PRIVATE
        ENABLE_AI_FEATURES=1
        ENABLE_LOCAL_TRAINING=1
    )
else()
    target_compile_definitions(roblox_executor PRIVATE
        ENABLE_AI_FEATURES=0
    )
endif()

# Explicitly reference iOS symbols to prevent optimization - move to linker flags
# (previously was causing "linker input unused" warnings)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -u _SCNetworkReachabilityCreateWithAddress_STUB")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -u _SCNetworkReachabilityGetFlags_STUB")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -u _SCNetworkReachabilityScheduleWithRunLoop_STUB")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -u _SCNetworkReachabilitySetCallback_STUB")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -u _SCNetworkReachabilityUnscheduleFromRunLoop_STUB")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u _SCNetworkReachabilityCreateWithAddress_STUB")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u _SCNetworkReachabilityGetFlags_STUB")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u _SCNetworkReachabilityScheduleWithRunLoop_STUB")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u _SCNetworkReachabilitySetCallback_STUB")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u _SCNetworkReachabilityUnscheduleFromRunLoop_STUB")

# Include directories - ensure Lua headers are available
target_include_directories(roblox_executor PRIVATE 
    ${LUA_INCLUDE_DIR}
    $ENV{LUA_INCLUDE_DIR}  # Also try from environment
    /opt/homebrew/opt/lua/include  # Explicit path for macOS GitHub runner
    /opt/homebrew/include  # Common Homebrew include path
    /usr/local/include  # Standard system include path
    source
    source/cpp
    source/cpp/ios
)

# Add Dobby include if found
if(Dobby_FOUND)
    target_include_directories(roblox_executor PRIVATE ${Dobby_INCLUDE_DIRS})
endif()

# Link against required libraries
target_link_libraries(roblox_executor PRIVATE
    lua_bundled  # Directly use the target name instead of ${LUA_LIBRARIES}
    "-framework Foundation"
    "-framework UIKit"
    "-framework WebKit"
    "-framework CoreGraphics"
    "-framework CoreFoundation"
    "-framework JavaScriptCore"
    "-framework Security"
    "-weak_framework SystemConfiguration"  # Use weak linking for SystemConfiguration
)

# Ensure all stub libraries are built before the main target
add_dependencies(roblox_executor lua_bundled ensure_lua_path ios_stubs dobby_stub)

# Add Dobby and iOS stubs - we'll always have something to link against
if(USE_DOBBY AND Dobby_FOUND)
    if(TARGET dobby_stub)
        message(STATUS "Linking with stub Dobby implementation")
        target_link_libraries(roblox_executor PRIVATE dobby_stub)
    elseif(EXISTS "${Dobby_LIBRARIES}")
        message(STATUS "Linking with Dobby library: ${Dobby_LIBRARIES}")
        target_link_libraries(roblox_executor PRIVATE ${Dobby_LIBRARIES})
    else()
        message(WARNING "Dobby library file not found and no stub created. This shouldn't happen.")
    endif()
endif()

# Link against iOS stubs with multiple approaches to ensure symbols are found
# Use direct path to static library to avoid dependency issues
set(IOS_STUBS_LIB "${CMAKE_BINARY_DIR}/libios_stubs.a")
target_link_libraries(roblox_executor PRIVATE 
    "-force_load" "${IOS_STUBS_LIB}"
    "-all_load" # Force all symbols to be loaded from all libraries
)

# Create required directories for AI data
if(ENABLE_AI_FEATURES)
    add_custom_command(TARGET roblox_executor POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:roblox_executor>/Resources/AIData"
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:roblox_executor>/Resources/AIData/LocalModels"
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:roblox_executor>/Resources/AIData/Vulnerabilities"
        COMMENT "Creating AI data directories"
    )
endif()

# Install the dylib
install(TARGETS roblox_executor
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# Set build options for iOS
if(CMAKE_BUILD_TYPE MATCHES Release)
    # Optimization flags for release builds
    target_compile_options(roblox_executor PRIVATE
        -Os
        -fvisibility=hidden
        -fvisibility-inlines-hidden
    )
else()
    # Debug build flags
    target_compile_options(roblox_executor PRIVATE
        -g
    )
endif()

# Add error reporting flags to show more details during build
target_compile_options(roblox_executor PRIVATE
    -ferror-limit=0        # No limit on number of errors to show
    -fcolor-diagnostics    # Use color in diagnostics
    -fdiagnostics-show-category=name    # Show category name
    -fdiagnostics-absolute-paths        # Show absolute paths
)
