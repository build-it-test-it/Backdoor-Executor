// FileSystem implementation for iOS
#include "FileSystem.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <ctime>
#include <cstring>

// Define our FileSystem implementation within the iOS namespace
// but avoid using any extra qualification inside the namespace
namespace iOS {
    // Initialize static members
    std::string FileSystem::m_documentsPath = "";
    std::string FileSystem::m_workspacePath = "";
    std::string FileSystem::m_scriptsPath = "";
    std::string FileSystem::m_logPath = "";
    std::string FileSystem::m_configPath = "";
    bool FileSystem::m_initialized = false;
    
    // Initialize the file system
    bool FileSystem::Initialize(const std::string& appName) {
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
    std::string FileSystem::GetDocumentsPath() {
        return m_documentsPath;
    }
    
    std::string FileSystem::GetWorkspacePath() {
        return m_workspacePath;
    }
    
    std::string FileSystem::GetScriptsPath() {
        return m_scriptsPath;
    }
    
    std::string FileSystem::GetLogPath() {
        return m_logPath;
    }
    
    std::string FileSystem::GetConfigPath() {
        return m_configPath;
    }
    
    // Create a directory
    bool FileSystem::CreateDirectory(const std::string& path) {
        std::string safePath = SanitizePath(path);
        return CreateDirectoryInternal(safePath);
    }
    
    // Internal implementation of directory creation
    bool FileSystem::CreateDirectoryInternal(const std::string& path) {
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
                      << [[error localizedDescription] UTF8String] << std::endl;
        }
        
        return success;
        #else
        // Fallback implementation for non-Objective-C builds
        return mkdir(path.c_str(), 0755) == 0 || errno == EEXIST;
        #endif
    }
    
    // Ensure a directory exists, creating it if necessary
    bool FileSystem::EnsureDirectoryExists(const std::string& path) {
        if (Exists(path)) {
            if (GetFileInfo(path).m_type == FileType::Directory) {
                return true;
            }
            std::cerr << "FileSystem: Path exists but is not a directory: " << path << std::endl;
            return false;
        }
        
        return CreateDirectory(path);
    }
    
    // Write data to a file
    bool FileSystem::WriteFile(const std::string& path, const std::string& content) {
        std::string safePath = SanitizePath(path);
        
        // Make sure the parent directory exists
        std::string dirPath = GetDirectoryName(safePath);
        if (!dirPath.empty() && !EnsureDirectoryExists(dirPath)) {
            std::cerr << "FileSystem: Failed to create parent directory: " << dirPath << std::endl;
            return false;
        }
        
        try {
            std::ofstream file(safePath, std::ios::out | std::ios::binary);
            if (!file.is_open()) {
                std::cerr << "FileSystem: Failed to open file for writing: " << safePath << std::endl;
                return false;
            }
            
            file.write(content.c_str(), content.size());
            bool success = !file.fail();
            file.close();
            
            return success;
        } catch (const std::exception& e) {
            std::cerr << "FileSystem: Exception writing file: " << e.what() << std::endl;
            return false;
        }
    }
    
    // Append data to a file
    bool FileSystem::AppendToFile(const std::string& path, const std::string& content) {
        std::string safePath = SanitizePath(path);
        
        // Make sure the parent directory exists
        std::string dirPath = GetDirectoryName(safePath);
        if (!dirPath.empty() && !EnsureDirectoryExists(dirPath)) {
            std::cerr << "FileSystem: Failed to create parent directory: " << dirPath << std::endl;
            return false;
        }
        
        try {
            std::ofstream file(safePath, std::ios::out | std::ios::app | std::ios::binary);
            if (!file.is_open()) {
                std::cerr << "FileSystem: Failed to open file for appending: " << safePath << std::endl;
                return false;
            }
            
            file.write(content.c_str(), content.size());
            bool success = !file.fail();
            file.close();
            
            return success;
        } catch (const std::exception& e) {
            std::cerr << "FileSystem: Exception appending to file: " << e.what() << std::endl;
            return false;
        }
    }
    
    // Read the contents of a file
    std::string FileSystem::ReadFile(const std::string& path) {
        std::string safePath = SanitizePath(path);
        
        if (!FileExists(safePath)) {
            std::cerr << "FileSystem: File does not exist: " << safePath << std::endl;
            return "";
        }
        
        try {
            std::ifstream file(safePath, std::ios::in | std::ios::binary);
            if (!file.is_open()) {
                std::cerr << "FileSystem: Failed to open file for reading: " << safePath << std::endl;
                return "";
            }
            
            // Get file size
            file.seekg(0, std::ios::end);
            size_t size = file.tellg();
            file.seekg(0, std::ios::beg);
            
            // Read the file
            std::string content(size, ' ');
            file.read(&content[0], size);
            file.close();
            
            return content;
        } catch (const std::exception& e) {
            std::cerr << "FileSystem: Exception reading file: " << e.what() << std::endl;
            return "";
        }
    }
    
    // Check if a file exists
    bool FileSystem::FileExists(const std::string& path) {
        std::string safePath = SanitizePath(path);
        
        if (!Exists(safePath)) {
            return false;
        }
        
        return GetFileInfo(safePath).m_type == FileType::File;
    }
    
    // Check if a directory exists
    bool FileSystem::DirectoryExists(const std::string& path) {
        std::string safePath = SanitizePath(path);
        
        if (!Exists(safePath)) {
            return false;
        }
        
        return GetFil# Let's check what files we've modified but haven't staged
git status

# Add the files we've modified
echo "Adding modified files to git..."
git add source/cpp/ios/FileSystem.mm
git add source/cpp/ios/FileSystem.h
git add source/cpp/ios/ScriptManager.mm
git add source/cpp/ios/ai_features/AIIntegration.mm

# Create a new ios_compat.h file if it doesn't exist
if [ ! -f source/cpp/ios_compat.h ]; then
  echo "Creating ios_compat.h..."
  cat > source/cpp/ios_compat.h << 'EOF'
// Special compatibility file to prevent namespace conflicts with std::filesystem
#pragma once

// Ensure we don't include std::filesystem directly
#ifndef IOS_AVOID_STD_FILESYSTEM
#define IOS_AVOID_STD_FILESYSTEM
#endif

// Include what we need
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <cstdint>
#include <ctime>
