#include "FileSystem.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <ctime>
#include <cstring>
#import <Foundation/Foundation.h>

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
            return true; // Already initialized
        }
        
        try {
            // Get the documents directory path
            NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
            if ([paths count] == 0) {
                std::cerr << "FileSystem: Failed to get documents directory path" << std::endl;
                return false;
            }
            
            NSString* documentsDir = [paths objectAtIndex:0];
            m_documentsPath = [documentsDir UTF8String];
            
            // Create workspace directory
            m_workspacePath = CombinePaths(m_documentsPath, appName);
            if (!EnsureDirectoryExists(m_workspacePath)) {
                std::cerr << "FileSystem: Failed to create workspace directory" << std::endl;
                return false;
            }
            
            // Create scripts directory
            m_scriptsPath = CombinePaths(m_workspacePath, "Scripts");
            if (!EnsureDirectoryExists(m_scriptsPath)) {
                std::cerr << "FileSystem: Failed to create scripts directory" << std::endl;
                return false;
            }
            
            // Create log directory
            m_logPath = CombinePaths(m_workspacePath, "Logs");
            if (!EnsureDirectoryExists(m_logPath)) {
                std::cerr << "FileSystem: Failed to create log directory" << std::endl;
                return false;
            }
            
            // Create config directory
            m_configPath = CombinePaths(m_workspacePath, "Config");
            if (!EnsureDirectoryExists(m_configPath)) {
                std::cerr << "FileSystem: Failed to create config directory" << std::endl;
                return false;
            }
            
            // Create a default script
            if (!CreateDefaultScript()) {
                std::cerr << "FileSystem: Warning - Failed to create default script" << std::endl;
                // Continue anyway, not critical
            }
            
            // Create a default configuration file
            if (!CreateDefaultConfig()) {
                std::cerr << "FileSystem: Warning - Failed to create default config" << std::endl;
                // Continue anyway, not critical
            }
            
            m_initialized = true;
            std::cout << "FileSystem: Successfully initialized" << std::endl;
            std::cout << "FileSystem: Documents path: " << m_documentsPath << std::endl;
            std::cout << "FileSystem: Workspace path: " << m_workspacePath << std::endl;
            
            return true;
        } catch (const std::exception& e) {
            std::cerr << "FileSystem: Exception during initialization: " << e.what() << std::endl;
            return false;
        }
    }
    
    // Get the path to the Documents directory
    std::string FileSystem::GetDocumentsPath() {
        return m_documentsPath;
    }
    
    // Get the path to the workspace directory
    std::string FileSystem::GetWorkspacePath() {
        return m_workspacePath;
    }
    
    // Get the path to the scripts directory
    std::string FileSystem::GetScriptsPath() {
        return m_scriptsPath;
    }
    
    // Get the path to the log directory
    std::string FileSystem::GetLogPath() {
        return m_logPath;
    }
    
    // Get the path to the config directory
    std::string FileSystem::GetConfigPath() {
        return m_configPath;
    }
    
    // Create a directory
    bool FileSystem::CreateDirectory(const std::string& path) {
        // Sanitize the path to ensure it's within our sandbox
        std::string safePath = SanitizePath(path);
        
        return CreateDirectoryInternal(safePath);
    }
    
    // Internal method to create a directory
    bool FileSystem::CreateDirectoryInternal(const std::string& path) {
        // Use NSFileManager to create directory
        NSString* nsPath = [NSString stringWithUTF8String:path.c_str()];
        NSFileManager* fileManager = [NSFileManager defaultManager];
        
        NSError* error = nil;
        BOOL success = [fileManager createDirectoryAtPath:nsPath 
                             withIntermediateDirectories:YES 
                                              attributes:nil 
                                                   error:&error];
        
        if (!success) {
            std::cerr << "FileSystem: Failed to create directory: " << path;
            if (error) {
                std::cerr << " - " << [[error localizedDescription] UTF8String];
            }
            std::cerr << std::endl;
        }
        
        return success;
    }
    
    // Ensure a directory exists, creating it if necessary
    bool FileSystem::EnsureDirectoryExists(const std::string& path) {
        if (Exists(path)) {
            // Check if it's a directory
            if (GetFileType(path) == FileType::Directory) {
                return true;
            }
            
            // It exists but is not a directory
            std::cerr << "FileSystem: Path exists but is not a directory: " << path << std::endl;
            return false;
        }
        
        // Doesn't exist, create it
        return CreateDirectoryInternal(path);
    }
    
    // Create a file
    bool FileSystem::CreateFile(const std::string& path, const std::string& content) {
        // Sanitize the path to ensure it's within our sandbox
        std::string safePath = SanitizePath(path);
        
        // Ensure parent directory exists
        std::string parentDir = safePath.substr(0, safePath.find_last_of('/'));
        if (!EnsureDirectoryExists(parentDir)) {
            std::cerr << "FileSystem: Failed to ensure parent directory exists: " << parentDir << std::endl;
            return false;
        }
        
        // Create the file
        return WriteFile(safePath, content, false);
    }
    
    // Check if a file or directory exists
    bool FileSystem::Exists(const std::string& path) {
        // Sanitize the path to ensure it's within our sandbox
        std::string safePath = SanitizePath(path);
        
        // Use NSFileManager to check existence
        NSString* nsPath = [NSString stringWithUTF8String:safePath.c_str()];
        NSFileManager* fileManager = [NSFileManager defaultManager];
        
        return [fileManager fileExistsAtPath:nsPath];
    }
    
    // Get information about a file or directory
    FileSystem::FileInfo FileSystem::GetFileInfo(const std::string& path) {
        // Sanitize the path to ensure it's within our sandbox
        std::string safePath = SanitizePath(path);
        
        // Use NSFileManager to get file attributes
        NSString* nsPath = [NSString stringWithUTF8String:safePath.c_str()];
        NSFileManager* fileManager = [NSFileManager defaultManager];
        
        NSError* error = nil;
        NSDictionary* attributes = [fileManager attributesOfItemAtPath:nsPath error:&error];
        
        if (error) {
            std::cerr << "FileSystem: Failed to get file attributes: " << [[error localizedDescription] UTF8String] << std::endl;
            return FileInfo(); // Return default (error) FileInfo
        }
        
        // Extract file information
        NSString* fileType = [attributes fileType];
        FileType type = FileType::Regular;
        
        if ([fileType isEqualToString:NSFileTypeDirectory]) {
            type = FileType::Directory;
        } else if ([fileType isEqualToString:NSFileTypeSymbolicLink]) {
            type = FileType::Symlink;
        } else if (![fileType isEqualToString:NSFileTypeRegular]) {
            type = FileType::Unknown;
        }
        
        // Get file size
        uint64_t size = [[attributes objectForKey:NSFileSize] unsignedLongLongValue];
        
        // Get modification time
        NSDate* modDate = [attributes objectForKey:NSFileModificationDate];
        uint64_t modTime = static_cast<uint64_t>([modDate timeIntervalSince1970]);
        
        // Check permissions
        bool isReadable = [fileManager isReadableFileAtPath:nsPath];
        bool isWritable = [fileManager isWritableFileAtPath:nsPath];
        
        // Get file name
        std::string name = GetFileName(safePath);
        
        return FileInfo(safePath, name, type, size, modTime, isReadable, isWritable);
    }
    
    // Get the type of a file or directory
    FileSystem::FileType FileSystem::GetFileType(const std::string& path) {
        return GetFileInfo(path).m_type;
    }
    
    // Read a file
    std::string FileSystem::ReadFile(const std::string& path) {
        // Sanitize the path to ensure it's within our sandbox
        std::string safePath = SanitizePath(path);
        
        // Check if file exists
        if (!Exists(safePath)) {
            std::cerr << "FileSystem: File does not exist: " << safePath << std::endl;
            return "";
        }
        
        // Use NSFileManager to read file
        NSString* nsPath = [NSString stringWithUTF8String:safePath.c_str()];
        NSError* error = nil;
        NSString* content = [NSString stringWithContentsOfFile:nsPath 
                                                      encoding:NSUTF8StringEncoding 
                                                         error:&error];
        
        if (error) {
            std::cerr << "FileSystem: Failed to read file: " << [[error localizedDescription] UTF8String] << std::endl;
            return "";
        }
        
        return [content UTF8String];
    }
    
    // Write to a file
    bool FileSystem::WriteFile(const std::string& path, const std::string& content, bool append) {
        // Sanitize the path to ensure it's within our sandbox
        std::string safePath = SanitizePath(path);
        
        // Use NSFileManager to write file
        NSString* nsPath = [NSString stringWithUTF8String:safePath.c_str()];
        NSString* nsContent = [NSString stringWithUTF8String:content.c_str()];
        
        NSError* error = nil;
        
        if (append && Exists(safePath)) {
            // Read existing content
            NSString* existingContent = [NSString stringWithContentsOfFile:nsPath 
                                                                  encoding:NSUTF8StringEncoding 
                                                                     error:&error];
            
            if (error) {
                std::cerr << "FileSystem: Failed to read file for append: " << [[error localizedDescription] UTF8String] << std::endl;
                return false;
            }
            
            // Append new content
            nsContent = [existingContent stringByAppendingString:nsContent];
        }
        
        // Write content to file
        BOOL success = [nsContent writeToFile:nsPath 
                                   atomically:YES 
                                     encoding:NSUTF8StringEncoding 
                                        error:&error];
        
        if (!success) {
            std::cerr << "FileSystem: Failed to write file: ";
            if (error) {
                std::cerr << [[error localizedDescription] UTF8String];
            }
            std::cerr << std::endl;
        }
        
        return success;
    }
    
    // Delete a file or directory
    bool FileSystem::Delete(const std::string& path) {
        // Sanitize the path to ensure it's within our sandbox
        std::string safePath = SanitizePath(path);
        
        // Check if file exists
        if (!Exists(safePath)) {
            std::cerr << "FileSystem: File does not exist: " << safePath << std::endl;
            return false;
        }
        
        // Use NSFileManager to delete file
        NSString* nsPath = [NSString stringWithUTF8String:safePath.c_str()];
        NSFileManager* fileManager = [NSFileManager defaultManager];
        
        NSError* error = nil;
        BOOL success = [fileManager removeItemAtPath:nsPath error:&error];
        
        if (!success) {
            std::cerr << "FileSystem: Failed to delete file: " << [[error localizedDescription] UTF8String] << std::endl;
        }
        
        return success;
    }
    
    // Rename a file or directory
    bool FileSystem::Rename(const std::string& oldPath, const std::string& newPath) {
        // Sanitize the paths to ensure they're within our sandbox
        std::string safeOldPath = SanitizePath(oldPath);
        std::string safeNewPath = SanitizePath(newPath);
        
        // Check if source file exists
        if (!Exists(safeOldPath)) {
            std::cerr << "FileSystem: Source file does not exist: " << safeOldPath << std::endl;
            return false;
        }
        
        // Use NSFileManager to move file
        NSString* nsOldPath = [NSString stringWithUTF8String:safeOldPath.c_str()];
        NSString* nsNewPath = [NSString stringWithUTF8String:safeNewPath.c_str()];
        NSFileManager* fileManager = [NSFileManager defaultManager];
        
        NSError* error = nil;
        BOOL success = [fileManager moveItemAtPath:nsOldPath toPath:nsNewPath error:&error];
        
        if (!success) {
            std::cerr << "FileSystem: Failed to rename file: " << [[error localizedDescription] UTF8String] << std::endl;
        }
        
        return success;
    }
    
    // Copy a file
    bool FileSystem::CopyFile(const std::string& sourcePath, const std::string& destPath) {
        // Sanitize the paths to ensure they're within our sandbox
        std::string safeSourcePath = SanitizePath(sourcePath);
        std::string safeDestPath = SanitizePath(destPath);
        
        // Check if source file exists
        if (!Exists(safeSourcePath)) {
            std::cerr << "FileSystem: Source file does not exist: " << safeSourcePath << std::endl;
            return false;
        }
        
        // Use NSFileManager to copy file
        NSString* nsSourcePath = [NSString stringWithUTF8String:safeSourcePath.c_str()];
        NSString* nsDestPath = [NSString stringWithUTF8String:safeDestPath.c_str()];
        NSFileManager* fileManager = [NSFileManager defaultManager];
        
        NSError* error = nil;
        BOOL success = [fileManager copyItemAtPath:nsSourcePath toPath:nsDestPath error:&error];
        
        if (!success) {
            std::cerr << "FileSystem: Failed to copy file: " << [[error localizedDescription] UTF8String] << std::endl;
        }
        
        return success;
    }
    
    // List files in a directory
    std::vector<FileSystem::FileInfo> FileSystem::ListDirectory(const std::string& path) {
        // Sanitize the path to ensure it's within our sandbox
        std::string safePath = SanitizePath(path);
        
        std::vector<FileInfo> files;
        
        // Check if directory exists
        if (!Exists(safePath)) {
            std::cerr << "FileSystem: Directory does not exist: " << safePath << std::endl;
            return files;
        }
        
        // Check if it's a directory
        if (GetFileType(safePath) != FileType::Directory) {
            std::cerr << "FileSystem: Path is not a directory: " << safePath << std::endl;
            return files;
        }
        
        // Use NSFileManager to list directory
        NSString* nsPath = [NSString stringWithUTF8String:safePath.c_str()];
        NSFileManager* fileManager = [NSFileManager defaultManager];
        
        NSError* error = nil;
        NSArray* contents = [fileManager contentsOfDirectoryAtPath:nsPath error:&error];
        
        if (error) {
            std::cerr << "FileSystem: Failed to list directory: " << [[error localizedDescription] UTF8String] << std::endl;
            return files;
        }
        
        // Process each file
        for (NSString* file in contents) {
            std::string filePath = safePath + "/" + [file UTF8String];
            FileInfo info = GetFileInfo(filePath);
            files.push_back(info);
        }
        
        return files;
    }
    
    // Get a unique file name for a path by appending a number if needed
    std::string FileSystem::GetUniqueFilePath(const std::string& basePath) {
        // Sanitize the path to ensure it's within our sandbox
        std::string safePath = SanitizePath(basePath);
        
        // If the file doesn't exist, return the path as is
        if (!Exists(safePath)) {
            return safePath;
        }
        
        // File exists, find a unique name by appending a number
        std::string directory = safePath.substr(0, safePath.find_last_of('/'));
        std::string fileName = GetFileName(safePath);
        
        // Split file name into base name and extension
        std::string baseName = fileName;
        std::string extension = "";
        
        size_t dotPos = fileName.find_last_of('.');
        if (dotPos != std::string::npos) {
            baseName = fileName.substr(0, dotPos);
            extension = fileName.substr(dotPos);
        }
        
        // Try appending numbers until a unique name is found
        int counter = 1;
        std::string uniquePath;
        
        do {
            std::string uniqueName = baseName + "_" + std::to_string(counter) + extension;
            uniquePath = directory + "/" + uniqueName;
            counter++;
        } while (Exists(uniquePath));
        
        return uniquePath;
    }
    
    // Get a safe path within the app's sandbox
    std::string FileSystem::GetSafePath(const std::string& relativePath) {
        // Ensure file system is initialized
        if (!m_initialized) {
            std::cerr << "FileSystem: Not initialized" << std::endl;
            return "";
        }
        
        // Combine workspace path with relative path
        return CombinePaths(m_workspacePath, relativePath);
    }
    
    // Check if the app has permission to access a path
    bool FileSystem::HasPermission(const std::string& path, bool requireWrite) {
        // Sanitize the path to ensure it's within our sandbox
        std::string safePath = SanitizePath(path);
        
        // Use NSFileManager to check permissions
        NSString* nsPath = [NSString stringWithUTF8String:safePath.c_str()];
        NSFileManager* fileManager = [NSFileManager defaultManager];
        
        if (requireWrite) {
            return [fileManager isWritableFileAtPath:nsPath];
        } else {
            return [fileManager isReadableFileAtPath:nsPath];
        }
    }
    
    // Sanitize a path to ensure it's within our sandbox
    std::string FileSystem::SanitizePath(const std::string& path) {
        // If path is empty, return empty string
        if (path.empty()) {
            return "";
        }
        
        // If file system is not initialized, return path as is
        if (!m_initialized) {
            return path;
        }
        
        // If path is already absolute and within our documents directory, return it as is
        if (path.find(m_documentsPath) == 0) {
            return path;
        }
        
        // If path is absolute but outside our documents directory, treat it as relative
        std::string relativePath = path;
        if (path[0] == '/') {
            // Extract the file/directory name only
            size_t lastSlash = path.find_last_of('/');
            if (lastSlash != std::string::npos) {
                relativePath = path.substr(lastSlash + 1);
            }
        }
        
        // Combine with workspace path
        return CombinePaths(m_workspacePath, relativePath);
    }
    
    // Get the file name from a path
    std::string FileSystem::GetFileName(const std::string& path) {
        size_t lastSlash = path.find_last_of('/');
        if (lastSlash != std::string::npos) {
            return path.substr(lastSlash + 1);
        }
        
        return path;
    }
    
    // Combine two paths
    std::string FileSystem::CombinePaths(const std::string& path1, const std::string& path2) {
        // Remove trailing slash from path1 if present
        std::string cleanPath1 = path1;
        if (!cleanPath1.empty() && cleanPath1.back() == '/') {
            cleanPath1.pop_back();
        }
        
        // Remove leading slash from path2 if present
        std::string cleanPath2 = path2;
        if (!cleanPath2.empty() && cleanPath2.front() == '/') {
            cleanPath2.erase(0, 1);
        }
        
        // Combine paths with a slash
        return cleanPath1 + "/" + cleanPath2;
    }
    
    // Create a default script in the scripts directory
    bool FileSystem::CreateDefaultScript() {
        // Default script path
        std::string scriptPath = CombinePaths(m_scriptsPath, "WelcomeScript.lua");
        
        // Default script content
        std::string content = 
R"(-- Welcome to Executor Pro!
-- This is a default script to help you get started.

-- Print a welcome message
print("Welcome to Executor Pro!")
print("Execution successful!")

-- Basic Roblox script example
local Players = game:GetService("Players")
local LocalPlayer = Players.LocalPlayer

-- Display player info
print("Player Name: " .. LocalPlayer.Name)
print("Player ID: " .. LocalPlayer.UserId)

-- Simple ESP function example
local function createESP()
    for _, player in pairs(Players:GetPlayers()) do
        if player ~= LocalPlayer and player.Character then
            -- Create a highlight
            local highlight = Instance.new("Highlight")
            highlight.FillColor = Color3.fromRGB(255, 0, 0) -- Red
            highlight.OutlineColor = Color3.fromRGB(255, 255, 255) -- White
            highlight.FillTransparency = 0.5
            highlight.OutlineTransparency = 0
            highlight.Parent = player.Character
            
            print("Created ESP for " .. player.Name)
        end
    end
end

-- Uncomment the line below to create ESP for all players
-- createESP()
)";
        
        // Create the script
        return CreateFile(scriptPath, content);
    }
    
    // Create a default configuration file
    bool FileSystem::CreateDefaultConfig() {
        // Default config path
        std::string configPath = CombinePaths(m_configPath, "settings.json");
        
        // Default config content
        std::string content = 
R"({
    "ui": {
        "opacity": 0.85,
        "theme": "dark",
        "buttonSize": 50,
        "showButtonOnlyInGame": true,
        "autoShowOnGameJoin": true,
        "autoHideOnGameLeave": true
    },
    "execution": {
        "autoRetryOnFail": true,
        "maxRetries": 3,
        "timeout": 5000,
        "enableObfuscation": true
    },
    "scripts": {
        "autoSave": true,
        "defaultDirectory": "Scripts",
        "maxRecentScripts": 10
    },
    "security": {
        "encryptSavedScripts": true,
        "enableAntiDetection": true,
        "enableVMDetection": true
    }
})";
        
        // Create the config file
        return CreateFile(configPath, content);
    }
}
