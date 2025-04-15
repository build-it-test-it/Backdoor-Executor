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
        return fs::path(getenv("HOME")) / "Documents";
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
    
    // Directory listing
    struct FileInfo {
        std::string path;
        bool isDirectory;
        std::uintmax_t size;
        std::time_t modificationTime;
        bool isReadable;
        bool isWritable;
        
        FileInfo() : 
            isDirectory(false),
            size(0),
            modificationTime(0),
            isReadable(false),
            isWritable(false) {}
    };
    
    inline FileInfo GetFileInfo(const std::string& path) {
        FileInfo info;
        info.path = path;
        
        std::error_code ec;
        if (!fs::exists(path, ec)) {
            return info;
        }
        
        info.isDirectory = fs::is_directory(path, ec);
        info.size = fs::file_size(path, ec);
        
        auto time = fs::last_write_time(path, ec);
        auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
            time - fs::file_time_type::clock::now() + std::chrono::system_clock::now());
        info.modificationTime = std::chrono::system_clock::to_time_t(sctp);
        
        // Check permissions (this is platform specific, simplified here)
        std::ifstream readTest(path);
        info.isReadable = readTest.good();
        readTest.close();
        
        std::ofstream writeTest(path, std::ios::app);
        info.isWritable = writeTest.good();
        writeTest.close();
        
        return info;
    }
    
    inline std::vector<FileInfo> ListDirectory(const std::string& path) {
        std::vector<FileInfo> results;
        
        std::error_code ec;
        if (!fs::is_directory(path, ec)) {
            std::cerr << "Cannot list directory, it does not exist: " << path << std::endl;
            return results;
        }
        
        try {
            for(const auto& entry : fs::directory_iterator(path, ec)) {
                results.push_back(GetFileInfo(entry.path().string()));
            }
        } catch (const std::exception& e) {
            std::cerr << "Exception listing directory: " << e.what() << std::endl;
        }
        
        return results;
    }
    
    // Initialize filesystem
    inline bool Initialize(const std::string& appName = "RobloxExecutor") {
        try {
            // Create workspace directory
            std::string workspacePath = GetWorkspacePath(appName);
            if (!CreateDirectory(workspacePath)) {
                std::cerr << "Failed to create workspace directory" << std::endl;
                return false;
            }
            
            // Create scripts directory
            std::string scriptsPath = GetScriptsPath(appName);
            if (!CreateDirectory(scriptsPath)) {
                std::cerr << "Failed to create scripts directory" << std::endl;
                return false;
            }
            
            // Create logs directory
            std::string logsPath = GetLogPath(appName);
            if (!CreateDirectory(logsPath)) {
                std::cerr << "Failed to create logs directory" << std::endl;
                return false;
            }
            
            // Create config directory
            std::string configPath = GetConfigPath(appName);
            if (!CreateDirectory(configPath)) {
                std::cerr << "Failed to create config directory" << std::endl;
                return false;
            }
            
            // Create default script
            std::string scriptPath = JoinPaths(scriptsPath, "WelcomeScript.lua");
            if (!FileExists(scriptPath)) {
                std::string content = R"(
-- Welcome to the Roblox Executor
-- This is an example script to get you started

print("Hello from the Roblox Executor!")

-- Example function to change player speed
local function setSpeed(speed)
    local player = game.Players.LocalPlayer
    if player and player.Character then
        local humanoid = player.Character:FindFirstChildOfClass("Humanoid")
        if humanoid then
            humanoid.WalkSpeed = speed
        end
    end
end

-- Example usage: Uncomment the line below to set speed to 50
-- setSpeed(50)

-- Enjoy using the executor!
)";
                
                if (!WriteFile(scriptPath, content)) {
                    std::cerr << "Failed to create default script" << std::endl;
                    return false;
                }
            }
            
            // Create default config
            std::string configFilePath = JoinPaths(configPath, "settings.json");
            if (!FileExists(configFilePath)) {
                std::string content = R"({
    "version": "1.0.0",
    "settings": {
        "autoExecute": false,
        "darkMode": true,
        "fontSize": 14,
        "logExecution": true,
        "maxRecentScripts": 10
    },
    "execution": {
        "timeoutMs": 5000,
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
                
                if (!WriteFile(configFilePath, content)) {
                    std::cerr <<# Let's get more aggressive with making changes
echo "Checking current state..."

# First, let's make sure our filesystem_utils.h was created
if [ ! -f "source/cpp/filesystem_utils.h" ]; then
  echo "Filesystem utils is missing! Creating it now..."
  # We need to create it again, but we've already defined it in the previous shell
  # Let's create a minimal version for testing
  mkdir -p source/cpp
  cat > source/cpp/filesystem_utils.h << 'EOF'
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
        return fs::path(getenv("HOME")) / "Documents";
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
    
    // File operations
    inline bool FileExists(const std::string& path) {
        std::error_code ec;
        return fs::exists(path, ec) && fs::is_regular_file(path, ec);
    }
    
    inline bool WriteFile(const std::string& path, const std::string& content) {
        try {
            std::ofstream file(path);
            if (!file.is_open()) return false;
            file << content;
            return true;
        } catch (...) {
            return false;
        }
    }
    
    inline std::string ReadFile(const std::string& path) {
        try {
            std::ifstream file(path);
            if (!file.is_open()) return "";
            return std::string(std::istreambuf_iterator<char>(file), {});
        } catch (...) {
            return "";
        }
    }
}
