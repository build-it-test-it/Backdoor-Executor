#!/bin/bash
# Apply isolation fixes to the codebase

echo "==== Applying clean isolation approach ===="

# 1. First make sure we have a backup of the original library.cpp
if [ ! -f "source/library.cpp.bak" ]; then
  cp source/library.cpp source/library.cpp.bak
fi

# 2. Make the updated library.cpp the active one
cp source/library.cpp.new source/library.cpp

# 3. Update CMakeLists.txt to include our bridge code
if ! grep -q "lua_objc_bridge.cpp" CMakeLists.txt; then
  echo "Updating CMakeLists.txt to include bridge code..."
  
  # Find where the roblox_executor target is defined
  EXECUTOR_LINE=$(grep -n "add_library(roblox_executor" CMakeLists.txt | cut -d: -f1)
  
  if [ ! -z "$EXECUTOR_LINE" ]; then
    # Add our bridge file to the sources
    sed -i "$EXECUTOR_LINE a\    source/cpp/bridge/lua_objc_bridge.cpp" CMakeLists.txt
    
    echo "Added bridge file to roblox_executor target"
  else
    echo "Warning: Could not find roblox_executor target in CMakeLists.txt"
    echo "Appending bridge code to the end of CMakeLists.txt"
    
    echo "" >> CMakeLists.txt
    echo "# Bridge for Lua and Objective-C isolation" >> CMakeLists.txt
    echo "target_sources(roblox_executor PRIVATE" >> CMakeLists.txt
    echo "    source/cpp/bridge/lua_objc_bridge.cpp" >> CMakeLists.txt
    echo ")" >> CMakeLists.txt
  fi
fi

# 4. Update key files that need isolation
echo "Updating key files that need isolation..."

# Find files that include both Lua and iOS headers
FILES_TO_UPDATE=$(grep -l "include.*Foundation\|UIKit" $(grep -l "include.*lua.h" $(find source/cpp -name "*.cpp" -o -name "*.h")))

if [ ! -z "$FILES_TO_UPDATE" ]; then
  echo "Found files that need isolation:"
  echo "$FILES_TO_UPDATE"
  
  for FILE in $FILES_TO_UPDATE; do
    echo "Updating $FILE..."
    
    # Create a backup
    cp "$FILE" "$FILE.bak"
    
    # Replace Lua includes with our isolation header
    sed -i 's|#include [<"].*lua.h[>"]|#include "../bridge/lua_isolation.h"|g' "$FILE"
    sed -i 's|#include [<"].*lualib.h[>"]|// Included via lua_isolation.h|g' "$FILE"
    sed -i 's|#include [<"].*lauxlib.h[>"]|// Included via lua_isolation.h|g' "$FILE"
    
    # Replace # Let's examine the issue in more detail
echo "Examining the key files causing issues..."

# Check the current ios_compat.h
cat source/cpp/ios_compat.h

# Check the iOS files with errors
grep -n "m_modTime" source/cpp/ios/FileSystem.h || echo "FileSystem.h not found"
ls -la source/cpp/ios/

# Let's create a more comprehensive fix
cat > fix_objc_mm_files.sh << 'EOF'
#!/bin/bash
# Comprehensive fix for Objective-C and C++ separation

echo "==== Implementing comprehensive Objective-C/C++ separation ===="

# 1. First, create a proper ios_compat.h file
mkdir -p source/cpp/ios
cat > source/cpp/ios/ios_compat.h << 'EOL'
// iOS compatibility header - provides clean separation between Objective-C and C++
#pragma once

// The key to proper isolation is to use preprocessor directives to separate code
// that is Objective-C specific from code that is C++ specific

// For Objective-C++ files (.mm extension)
#ifdef __OBJC__
    // Include full Objective-C frameworks
    #import <Foundation/Foundation.h>
    #import <UIKit/UIKit.h>
#else
    // For pure C++ files, provide simple type definitions without the Objective-C syntax
    #ifdef __cplusplus
    extern "C" {
    #endif
    
    // Basic types
    typedef void* id;
    typedef void* Class;
    typedef void* SEL;
    
    // Forward declarations of common iOS types as void pointers
    typedef id NSString;
    typedef id NSArray;
    typedef id NSMutableArray;
    typedef id NSDictionary;
    typedef id NSMutableDictionary;
    typedef id NSData;
    typedef id NSError;
    
    // UIKit types
    typedef id UIView;
    typedef id UIViewController;
    typedef id UIColor;
    typedef id UIFont;
    typedef id UIImage;
    typedef id UIWindow;
    typedef id UIButton;
    
    // CGRect and related structs
    typedef struct {
        double x;
        double y;
    } CGPoint;
    
    typedef struct {
        double width;
        double height;
    } CGSize;
    
    typedef struct {
        CGPoint origin;
        CGSize size;
    } CGRect;
    
    // Common typedefs
    typedef unsigned long NSUInteger;
    typedef long NSInteger;
    typedef unsigned char BOOL;
    
    #ifdef __cplusplus
    }
    #endif
#endif

// Common C/C++ includes
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string>
#include <vector>
#include <memory>

// Target platform define
#ifndef IOS_TARGET
#define IOS_TARGET
#endif
EOL

# 2. Fix the FileSystem.h issue
if [ -f "source/cpp/ios/FileSystem.h" ]; then
    cp source/cpp/ios/FileSystem.h source/cpp/ios/FileSystem.h.bak
    
    # Fix the m_modTime issue
    cat > source/cpp/ios/FileSystem.h.new << 'EOL'
// FileSystem interface for iOS
#pragma once

#include "../ios_compat.h"
#include <string>
#include <vector>
#include <ctime>

namespace iOS {
    // File types
    enum class FileType {
        Unknown,
        File,
        Directory
    };
    
    // File information structure
    class FileInfo {
    public:
        std::string m_path;
        FileType m_type;
        size_t m_size;
        time_t m_modificationTime;  // Fixed variable name
        bool m_isReadable;
        bool m_isWritable;
        
        FileInfo() : 
            m_type(FileType::Unknown), 
            m_size(0), 
            m_modificationTime(0),  // Fixed variable name
            m_isReadable(false), 
            m_isWritable(false) {}
        
        FileInfo(const std::string& path, FileType type, size_t size, time_t modTime, 
                  bool isReadable, bool isWritable) : 
            m_path(path), m_type(type), m_size(size), 
            m_modificationTime(modTime),  // Fixed variable name 
            m_isReadable(isReadable), 
            m_isWritable(isWritable) {}
    };
    
    // FileSystem class declaration
    class FileSystem {
    public:
        static bool FileExists(const std::string& path);
        static bool DirectoryExists(const std::string& path);
        static bool CreateDirectory(const std::string& path);
        static bool DeleteFile(const std::string& path);
        static bool RenameFile(const std::string& oldPath, const std::string& newPath);
        static bool CopyFile(const std::string& sourcePath, const std::string& destPath);
        
        static std::string ReadFile(const std::string& path);
        static bool WriteFile(const std::string& path, const std::string& content);
        static bool AppendToFile(const std::string& path, const std::string& content);
        
        static std::vector<FileInfo> ListDirectory(const std::string& path);
        static FileInfo GetFileInfo(const std::string& path);
        
        static std::string GetDocumentsDirectory();
        static std::string GetTempDirectory();
        static std::string GetCachesDirectory();
        
        static std::string JoinPaths(const std::string& path1, const std::string& path2);
        static std::string GetFileName(const std::string& path);
        static std::string GetFileExtension(const std::string& path);
        static std::string GetDirectoryName(const std::string& path);
    };
}
EOL
    
    mv source/cpp/ios/FileSystem.h.new source/cpp/ios/FileSystem.h
    echo "Fixed FileSystem.h"
fi

# 3. Set up script to separate Objective-C and C++ compilation
cat > source/cpp/bridge/mm_cpp_bridge.h << 'EOL'
// Bridge between Objective-C++ and C++ code
#pragma once

// This file should be included in both .mm and .cpp files
// It provides a clean API that doesn't expose incompatible types

#include <string>
#include <vector>
#include <functional>

// Objective-C bridge namespace
namespace ObjCBridge {
    // UI Functions
    bool ShowAlert(const char* title, const char* message);
    bool CreateFloatingButton(int x, int y, int width, int height, const char* title);
    void ShowViewController(const char* viewControllerName);
    
    // File System
    bool SaveFile(const char* path, const char* data, size_t length);
    char* LoadFile(const char* path, size_t* outLength);
    bool FileExists(const char* path);
    
    // Script Execution
    typedef void (*ScriptCallback)(const char* output);
    bool ExecuteScript(const char* script, ScriptCallback callback);
    
    // Memory functions (these will be implemented in C++ and called from Objective-C)
    void* AllocateMemory(size_t size);
    void FreeMemory(void* ptr);
    
    // Registration function to setup the bridge
    void RegisterBridgeFunctions();
}

// Define any pure C interfaces needed by both sides
extern "C" {
    // Simple C function that can be called from Objective-C
    bool CppExecuteScript(const char* script);
    
    // Allow Objective-C to call back into C++ with a result
    void ObjCCallbackWithResult(const char* result);
}
EOL

# 4. Create an implementation file for our bridge
cat > source/cpp/bridge/mm_cpp_bridge.mm << 'EOL'
// Implementation of the Objective-C/C++ bridge
#include "mm_cpp_bridge.h"
#include "../ios/ios_compat.h"

#ifdef __OBJC__
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

// Private Objective-C implementation
@interface BridgeImplementation : NSObject
+ (BOOL)showAlert:(NSString*)title message:(NSString*)message;
+ (BOOL)createFloatingButton:(CGRect)frame title:(NSString*)title;
+ (void)showViewController:(NSString*)viewControllerName;
@end

@implementation BridgeImplementation
+ (BOOL)showAlert:(NSString*)title message:(NSString*)message {
    UIAlertController* alert = [UIAlertController 
        alertControllerWithTitle:title 
        message:message 
        preferredStyle:UIAlertControllerStyleAlert];
    
    [alert addAction:[UIAlertAction actionWithTitle:@"OK" style:UIAlertActionStyleDefault handler:nil]];
    
    // In a real implementation, you'd present this from the current view controller
    // For this stub, we just return success
    return YES;
}

+ (BOOL)createFloatingButton:(CGRect)frame title:(NSString*)title {
    // In a real implementation, this would create a UIButton and add it to the window
    return YES;
}

+ (void)showViewController:(NSString*)viewControllerName {
    // In a real implementation, this would instantiate and present a view controller
}
@end
#endif

// Implementation of ObjCBridge functions
namespace ObjCBridge {
    bool ShowAlert(const char* title, const char* message) {
        #ifdef __OBJC__
        NSString* nsTitle = [NSString stringWithUTF8String:title];
        NSString* nsMessage = [NSString stringWithUTF8String:message];
        return [BridgeImplementation showAlert:nsTitle message:nsMessage];
        #else
        // Fallback for pure C++ builds (e.g., unit tests)
        return false;
        #endif
    }
    
    bool CreateFloatingButton(int x, int y, int width, int height, const char* title) {
        #ifdef __OBJC__
        CGRect frame = CGRectMake(x, y, width, height);
        NSString* nsTitle = [NSString stringWithUTF8String:title];
        return [BridgeImplementation createFloatingButton:frame title:nsTitle];
        #else
        return false;
        #endif
    }
    
    void ShowViewController(const char* viewControllerName) {
        #ifdef __OBJC__
        NSString* nsName = [NSString stringWithUTF8String:viewControllerName];
        [BridgeImplementation showViewController:nsName];
        #endif
    }
    
    bool SaveFile(const char* path, const char* data, size_t length) {
        #ifdef __OBJC__
        NSString* nsPath = [NSString stringWithUTF8String:path];
        NSData* nsData = [NSData dataWithBytes:data length:length];
        return [nsData writeToFile:nsPath atomically:YES];
        #else
        return false;
        #endif
    }
    
    char* LoadFile(const char* path, size_t* outLength) {
        #ifdef __OBJC__
        NSString* nsPath = [NSS# Let's try a more direct approach to fix the issues
echo "Let's directly fix the key files..."

# First, check if the ios_compat.h file actually exists and what's in it
ls -la source/cpp/ios_compat.h || echo "ios_compat.h not found in expected location"
find source -name "ios_compat.h"

# Let's examine the FileSystem.h file
find source -name "FileSystem.h"
if [ -f "source/cpp/ios/FileSystem.h" ]; then
  grep -n -A3 "m_modTime" source/cpp/ios/FileSystem.h
else
  echo "FileSystem.h not found at expected location"
fi

# Let's create all the necessary files directly
mkdir -p source/cpp/ios

# Create ios_compat.h
cat > source/cpp/ios_compat.h << 'EOF'
// iOS compatibility header
#pragma once

// Guards against including in a regular C++ file
#ifdef __OBJC__
  // Include full Objective-C frameworks when in Objective-C++ mode
  #import <Foundation/Foundation.h>
  #import <UIKit/UIKit.h>
#else
  // Forward declarations for C++ mode
  #ifdef __cplusplus
  extern "C" {
  #endif
  
  // Basic Objective-C types as opaque pointers
  typedef void* id;
  typedef struct objc_class* Class;
  typedef struct objc_selector* SEL;
  
  // Common iOS types
  typedef id NSString;
  typedef id UIView;
  typedef id UIViewController;
  typedef id NSArray;
  typedef id NSDictionary;
  
  // Basic structs
  typedef struct {
    double x, y;
  } CGPoint;
  
  typedef struct {
    double width, height;
  } CGSize;
  
  typedef struct {
    CGPoint origin;
    CGSize size;
  } CGRect;
  
  #ifdef __cplusplus
  }
  #endif
#endif

// Define iOS target if not already defined
#ifndef IOS_TARGET
#define IOS_TARGET
#endif
