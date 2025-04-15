// FileSystem interface for iOS
#pragma once

#include "../objc_isolation.h"
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
        time_t m_modificationTime;
        bool m_isReadable;
        bool m_isWritable;
        
        FileInfo() : 
            m_type(FileType::Unknown), 
            m_size(0), 
            m_modificationTime(0),
            m_isReadable(false), 
            m_isWritable(false) {}
        
        FileInfo(const std::string& path, FileType type, size_t size, time_t modTime, 
                 bool isReadable, bool isWritable) : 
            m_path(path),
            m_type(type),
            m_size(size), 
            m_modificationTime(modTime),
            m_isReadable(isReadable), 
            m_isWritable(isWritable) {}
    };
    
    // FileSystem class declaration - expanded to match implementation
    class FileSystem {
    private:
        // Static member variables
        static std::string m_documentsPath;
        static std::string m_workspacePath;
        static std::string m_scriptsPath;
        static std::string m_logPath;
        static std::string m_configPath;
        static bool m_initialized;
        
        // Helper functions
        static bool CreateDirectoryInternal(const std::string& path);
        static bool EnsureDirectoryExists(const std::string& path);
        static bool Exists(const std::string& path);
        static std::string SanitizePath(const std::string& path);
        static bool CreateDefaultScript();
        static bool CreateDefaultConfig();
        
    public:
        // Initialization
        static bool Initialize(const std::string& appName = "RobloxExecutor");
        
        // Path getters
        static std::string GetDocumentsPath();
        static std::string GetWorkspacePath();
        static std::string GetScriptsPath();
        static std::string GetLogPath();
        static std::string GetConfigPath();
        
        // Standard file operations
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
