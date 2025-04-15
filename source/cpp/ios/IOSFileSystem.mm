// IOSFileSystem implementation
#include "IOSFileSystem.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <ctime>
#include <cstring>

namespace iOS {
    // Initialize static members
    std::string IOSFileSystem::m_documentsPath = "";
    std::string IOSFileSystem::m_workspacePath = "";
    std::string IOSFileSystem::m_scriptsPath = "";
    std::string IOSFileSystem::m_logPath = "";
    std::string IOSFileSystem::m_configPath = "";
    bool IOSFileSystem::m_initialized = false;
    
    // Initialize the file system
    bool IOSFileSystem::Initialize(const std::string& appName) {
        if (m_initialized) {
            return true;
        }
        
        try {
            // Get the documents directory
            #ifdef __OBJC__
            NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
            if ([paths count] > 0) {
                NSString *documentsDirectory = [paths objectAtIndex:0];
                m_documentsPath = [documentsDirectory UTF8String];
            } else {
                std::cerr << "FileSystem: Failed to get documents directory" << std::endl;
                return false;
            }
            #else
            // For non-Objective-C builds, use a default path
            m_documentsPath = "/var/mobile/Documents";
            #endif
            
            // Create the workspace directory structure
            m_workspacePath = JoinPaths(m_documentsPath, appName);
            if (!EnsureDirectoryExists(m_workspacePath)) {
                std::cerr << "FileSystem: Failed to create workspace directory" << std::endl;
                return false;
            }
            
            m_scriptsPath = JoinPaths(m_workspacePath, "Scripts");
            if (!EnsureDirectoryExists(m_scriptsPath)) {
                std::cerr << "FileSystem: Failed to create scripts directory" << std::endl;
                return false;
            }
            
            m_logPath = JoinPaths(m_workspacePath, "Logs");
            if (!EnsureDirectoryExists(m_logPath)) {
                std::cerr << "FileSystem: Failed to create logs directory" << std::endl;
                return false;
            }
            
            m_configPath = JoinPaths(m_workspacePath, "Config");
            if (!EnsureDirectoryExists(m_configPath)) {
                std::cerr << "FileSystem: Failed to create config directory" << std::endl;
                return false;
            }
            
            // Create default files
            if (!CreateDefaultScript()) {
                std::cerr << "FileSystem: Failed to create default script" << std::endl;
                return false;
            }
            
            if (!CreateDefaultConfig()) {
                std::cerr << "FileSystem: Failed to create default config" << std::endl;
                return false;
            }
            
            m_initialized = true;
            std::cout << "FileSystem: Initialized successfully" << std::endl;
            return true;
        } catch (const std::exception& e) {
            std::cerr << "FileSystem: Exception during initialization: " << e.what() << std::endl;
            return false;
        }
    }
    
    // Path getters
    std::string IOSFileSystem::GetDocumentsPath() {
        return m_documentsPath;
    }
    
    std::string IOSFileSystem::GetWorkspacePath() {
        return m_workspacePath;
    }
    
    std::string IOSFileSystem::GetScriptsPath() {
        return m_scriptsPath;
    }
    
    std::string IOSFileSystem::GetLogPath() {
        return m_logPath;
    }
    
    std::string IOSFileSystem::GetConfigPath() {
        return m_configPath;
    }
    
    // Create a directory
    bool IOSFileSystem::CreateDirectory(const std::string& path) {
        std::string safePath = SanitizePath(path);
        return CreateDirectoryInternal(safePath);
    }
    
    // Internal implementation of directory creation
    bool IOSFileSystem::CreateDirectoryInternal(const std::string& path) {
        #ifdef __OBJC__
        NSFileManager *fileManager = [NSFileManager defaultManager];
        NSString *nsPath = [NSString stringWithUTF8String:path.c_str()];
        
        NSError *error = nil;
        BOOL success = [fileManager createDirectoryAtPath:nsPath
                              withIntermediateDirectories:YES
                                               attributes:nil
                                                    error:&error];
        
        if (!success) {
            std::cerr << "FileSystem: Failed to create directory: " 
               # Let's check if our IOSFileSystem.mm file was created and make sure update_references.sh runs
echo "Checking if IOSFileSystem.mm was created..."
ls -la source/cpp/ios/IOSFileSystem*

# Make sure update_references.sh was created and is executable
echo "Checking update_references.sh script..."
ls -la update_references.sh
chmod +x update_references.sh

# Run the update_references.sh script
echo "Running update_references.sh..."
./update_references.sh

# Now let's copy the implementation from FileSystem.mm to IOSFileSystem.mm if needed
if [ ! -f "source/cpp/ios/IOSFileSystem.mm" ]; then
  echo "Creating IOSFileSystem.mm from FileSystem.mm..."
  cp source/cpp/ios/FileSystem.mm source/cpp/ios/IOSFileSystem.mm
  
  # Update all references in IOSFileSystem.mm
  sed -i 's/FileSystem::/IOSFileSystem::/g' source/cpp/ios/IOSFileSystem.mm
  sed -i 's/std::string FileSystem::/std::string IOSFileSystem::/g' source/cpp/ios/IOSFileSystem.mm
  sed -i 's/bool FileSystem::/bool IOSFileSystem::/g' source/cpp/ios/IOSFileSystem.mm
  sed -i 's/FileInfo FileSystem::/FileInfo IOSFileSystem::/g' source/cpp/ios/IOSFileSystem.mm
  sed -i 's/FileType FileSystem::/FileType IOSFileSystem::/g' source/cpp/ios/IOSFileSystem.mm
  sed -i 's/std::vector<FileInfo> FileSystem::/std::vector<FileInfo> IOSFileSystem::/g' source/cpp/ios/IOSFileSystem.mm
fi

# Make sure FloatingButtonController.mm includes the proper headers
echo "Checking FloatingButtonController.mm..."
head -5 source/cpp/ios/FloatingButtonController.mm

# Add proper includes to FloatingButtonController.mm if they're missing
if ! grep -q "#import <UIKit/UIKit.h>" source/cpp/ios/FloatingButtonController.mm; then
  echo "Fixing FloatingButtonController.mm imports..."
  sed -i '1s/
^
/#import <UIKit\/UIKit.h>\n#import <QuartzCore\/QuartzCore.h>\n/' source/cpp/ios/FloatingButtonController.mm
fi

# Fix ExecutionEngine.mm to use IOSFileSystem
echo "Fixing ExecutionEngine.mm..."
sed -i 's/iOS::FileSystem::/iOS::IOSFileSystem::/g' source/cpp/ios/ExecutionEngine.mm

# Update AIIntegration.mm and ScriptManager.mm to use IOSFileSystem
echo "Fixing other files..."
if [ -f source/cpp/ios/ai_features/AIIntegration.mm ]; then
  sed -i 's/FileSystem::/IOSFileSystem::/g' source/cpp/ios/ai_features/AIIntegration.mm
fi

if [ -f source/cpp/ios/ScriptManager.mm ]; then
  sed -i 's/FileSystem::/IOSFileSystem::/g' source/cpp/ios/ScriptManager.mm
fi

# Let's check which files were modified
git status

echo "All files have been updated to use IOSFileSystem instead of FileSystem."
# We have an issue - FileSystem.mm has been corrupted with shell script
# Let's create a proper FileSystem.mm file that just has compatibility code

echo "Creating a clean FileSystem.mm compatibility file..."
cat > source/cpp/ios/FileSystem.mm << 'EOF'
// FileSystem compatibility implementation
// This file exists to forward calls to IOSFileSystem implementation

#include "FileSystem.h"
#include "IOSFileSystem.h"

// All functionality is handled by IOSFileSystem now
// This file exists just for compatibility with existing code
