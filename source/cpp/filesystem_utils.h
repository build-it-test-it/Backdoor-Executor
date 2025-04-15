// Standard filesystem utilities - using std::filesystem
#pragma once

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <system_error>

namespace fs = std::filesystem;

// Simple filesystem utility functions
namespace FileUtils {
    // Path operations
    inline std::string GetDocumentsPath() {
        #ifdef __APPLE__
        // Get user home directory and append Documents
        return (fs::path(getenv("HOME")) / "Documents").string();
        #else
        return fs::current_path().string();
        #endif
    }
    
    inline std::string GetWorkspacePath(const std::string& appName = "RobloxExecutor") {
        return (fs::path(GetDocumentsPath()) / appName).string();
    }
    
    inline std::string GetScriptsPath(const std::string& appName = "RobloxExecutor") {
        return (fs::path(GetWorkspacePath(appName)) / "Scripts").string();
    }
    
    inline std::string GetLogPath(const std::string& appName = "RobloxExecutor") {
        return (fs::path(GetWorkspacePath(appName)) / "Logs").string();
    }
    
    inline std::string GetConfigPath(const std::string& appName = "RobloxExecutor") {
        return (fs::path(GetWorkspacePath(appName)) / "Config").string();
    }

    inline std::string GetTempDirectory() {
        return fs::temp_directory_path().string();
    }
    
    inline std::string JoinPaths(const std::string& path1, const std::string& path2) {
        return (fs::path(path1) / path2).string();
    }
    
    inline std::string GetFileName(const std::string& path) {
        return fs::path(path).filename().string();
    }
    
    inline std::string GetFileExtension(const std::string& path) {
        return fs::path(path).extension().string();
    }
    
    inline std::string GetDirectoryName(const std::string& path) {
        return fs::path(path).parent_path().string();
    }
    
    // File operations
    inline bool FileExists(const std::string& path) {
        std::error_code ec;
        return fs::exists(path, ec) && fs::is_regular_file(path, ec);
    }
    
    inline bool DirectoryExists(const std::string& path) {
        std::error_code ec;
        return fs::exists(path, ec) && fs::is_directory(path, ec);
    }
    
    inline bool CreateDirectory(const std::string& path) {
        std::error_code ec;
        return fs::create_directories(path, ec);
    }
    
    inline bool DeleteFile(const std::string& path) {
        std::error_code ec;
        return fs::remove(path, ec);
    }
    
    inline bool RenameFile(const std::string& oldPath, const std::string& newPath) {
        std::error_code ec;
        fs::rename(oldPath, newPath, ec);
        return !ec;
    }
    
    inline bool CopyFile(const std::string& sourcePath, const std::string& destPath) {
        std::error_code ec;
        fs::copy_file(sourcePath, destPath, 
                      fs::copy_options::overwrite_existing, ec);
        return !ec;
    }
    
    // File content operations
    inline std::string ReadFile(const std::string& path) {
        if (!FileExists(path)) {
            std::cerr << "File does not exist: " << path << std::endl;
            return "";
        }
        
        try {
            std::ifstream file(path, std::ios::in | std::ios::binary);
            if (!file.is_open()) {
                std::cerr << "Failed to open file for reading: " << path << std::endl;
                return "";
            }
            
            // Read the entire file
            std::string content((std::istreambuf_iterator<char>(file)),
                               std::istreambuf_iterator<char>());
            
            return content;
        } catch (const std::exception& e) {
            std::cerr << "Exception reading file: " << e.what() << std::endl;
            return "";
        }
    }
    
    inline bool WriteFile(const std::string& path, const std::string& content) {
        try {
            // Create parent directory if it doesn't exist
            fs::path filePath(path);
            fs::path parentPath = filePath.parent_path();
            if (!parentPath.empty()) {
                std::error_code ec;
                fs::create_directories(parentPath, ec);
            }
            
            std::ofstream file(path, std::ios::out | std::ios::binary);
            if (!file.is_open()) {
                std::cerr << "Failed to open file for writing: " << path << std::endl;
                return false;
            }
            
            file.write(content.c_str(), content.size());
            bool success = !file.fail();
            file.close();
            
            return success;
        } catch (const std::exception& e) {
            std::cerr << "Exception writing file: " << e.what() << std::endl;
            return false;
        }
    }
    
    inline bool AppendToFile(const std::string& path, const std::string& content) {
        try {
            // Create parent directory if it doesn't exist
            fs::path filePath(path);
            fs::path parentPath = filePath.parent_path();
            if (!parentPath.empty()) {
                std::error_code ec;
                fs::create_directories(parentPath, ec);
            }
            
            std::ofstream file(path, std::ios::out | std::ios::app | std::ios::binary);
            if (!file.is_open()) {
                std::cerr << "Failed to open file for appending: " << path << std::endl;
                return false;
            }
            
            file.write(content.c_str(), content.size());
            bool success = !file.fail();
            file.close();
            
            return success;
        } catch (const std::exception& e) {
            std::cerr << "Exception appending to file: " << e.what() << std::endl;
            return false;
        }
    }
    
    // Additional compatibility functions that existed in the original FileSystem
    inline bool Exists(const std::string& path) {
        std::error_code ec;
        return fs::exists(path, ec);
    }
    
    inline bool Delete(const std::string& path) {
        return DeleteFile(path);
    }
    
    inline bool EnsureDirectoryExists(const std::string& path) {
        return CreateDirectory(path);
    }
    
    inline std::string CombinePaths(const std::string& path1, const std::string& path2) {
        return JoinPaths(path1, path2);
    }
}
