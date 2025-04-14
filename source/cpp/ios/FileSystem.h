#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>

namespace iOS {
    /**
     * @class FileSystem
     * @brief Manages file operations in a sandbox-compliant way for iOS
     * 
     * This class provides file operations that work on both jailbroken and
     * non-jailbroken devices, respecting iOS sandbox restrictions while
     * creating the necessary workspace structure for the executor.
     */
    class FileSystem {
    public:
        // File type enumeration
        enum class FileType {
            Regular,    // Regular file
            Directory,  // Directory
            Symlink,    // Symbolic link
            Unknown     // Unknown or error
        };
        
        // File information structure
        struct FileInfo {
            std::string m_path;        // Full path
            std::string m_name;        // File name only
            FileType m_type;           // File type
            uint64_t m_size;           // File size in bytes
            uint64_t m_modTime;        // Last modification time
            bool m_isReadable;         // Is readable by app
            bool m_isWritable;         // Is writable by app
            
            FileInfo() : m_type(FileType::Unknown), m_size(0), m_modTime(0),
                        m_isReadable(false), m_isWritable(false) {}
            
            FileInfo(const std::string& path, const std::string& name, FileType type,
                    uint64_t size, uint64_t modTime, bool isReadable, bool isWritable)
                : m_path(path), m_name(name), m_type(type), m_size(size),
                  m_modTime(modTime), m_isReadable(isReadable), m_isWritable(isWritable) {}
        };
        
    private:
        // Member variables with consistent m_ prefix
        static std::string m_documentsPath;
        static std::string m_workspacePath;
        static std::string m_scriptsPath;
        static std::string m_logPath;
        static std::string m_configPath;
        static bool m_initialized;
        
        // Private methods
        static bool CreateDirectoryInternal(const std::string& path);
        // Methods moved to public
        static std::string SanitizePath(const std::string& path);
        static std::string GetFileName(const std::string& path);
        
    public:
        // Made public to allow access from other classes
        static bool EnsureDirectoryExists(const std::string& path);
        static std::string CombinePaths(const std::string& path1, const std::string& path2);
        
    public:
        /**
         * @brief Initialize the file system
         * @param appName Name of the app (used for workspace folder)
         * @return True if initialization succeeded, false otherwise
         */
        static bool Initialize(const std::string& appName = "ExecutorWorkspace");
        
        /**
         * @brief Get the path to the Documents directory
         * @return Path to Documents directory
         */
        static std::string GetDocumentsPath();
        
        /**
         * @brief Get the path to the workspace directory
         * @return Path to workspace directory
         */
        static std::string GetWorkspacePath();
        
        /**
         * @brief Get the path to the scripts directory
         * @return Path to scripts directory
         */
        static std::string GetScriptsPath();
        
        /**
         * @brief Get the path to the log directory
         * @return Path to log directory
         */
        static std::string GetLogPath();
        
        /**
         * @brief Get the path to the config directory
         * @return Path to config directory
         */
        static std::string GetConfigPath();
        
        /**
         * @brief Create a directory
         * @param path Path to the directory to create
         * @return True if creation succeeded, false otherwise
         */
        static bool CreateDirectory(const std::string& path);
        
        /**
         * @brief Create a file
         * @param path Path to the file to create
         * @param content Initial content of the file (empty by default)
         * @return True if creation succeeded, false otherwise
         */
        static bool CreateFile(const std::string& path, const std::string& content = "");
        
        /**
         * @brief Check if a file or directory exists
         * @param path Path to check
         * @return True if file or directory exists, false otherwise
         */
        static bool Exists(const std::string& path);
        
        /**
         * @brief Get information about a file or directory
         * @param path Path to the file or directory
         * @return FileInfo structure with information, or default (error) FileInfo if path doesn't exist
         */
        static FileInfo GetFileInfo(const std::string& path);
        
        /**
         * @brief Get the type of a file or directory
         * @param path Path to the file or directory
         * @return FileType enumeration value
         */
        static FileType GetFileType(const std::string& path);
        
        /**
         * @brief Read a file
         * @param path Path to the file to read
         * @return File content as string, or empty string if read failed
         */
        static std::string ReadFile(const std::string& path);
        
        /**
         * @brief Write to a file
         * @param path Path to the file to write
         * @param content Content to write
         * @param append True to append to file, false to overwrite
         * @return True if write succeeded, false otherwise
         */
        static bool WriteFile(const std::string& path, const std::string& content, bool append = false);
        
        /**
         * @brief Delete a file or directory
         * @param path Path to the file or directory to delete
         * @return True if deletion succeeded, false otherwise
         */
        static bool Delete(const std::string& path);
        
        /**
         * @brief Rename a file or directory
         * @param oldPath Current path
         * @param newPath New path
         * @return True if rename succeeded, false otherwise
         */
        static bool Rename(const std::string& oldPath, const std::string& newPath);
        
        /**
         * @brief Copy a file
         * @param sourcePath Source file path
         * @param destPath Destination file path
         * @return True if copy succeeded, false otherwise
         */
        static bool CopyFile(const std::string& sourcePath, const std::string& destPath);
        
        /**
         * @brief List files in a directory
         * @param path Path to the directory
         * @return Vector of FileInfo structures, or empty vector if directory doesn't exist
         */
        static std::vector<FileInfo> ListDirectory(const std::string& path);
        
        /**
         * @brief Get a unique file name for a path by appending a number if needed
         * @param basePath Base file path
         * @return Unique file path that doesn't exist yet
         */
        static std::string GetUniqueFilePath(const std::string& basePath);
        
        /**
         * @brief Get a safe path within the app's sandbox
         * @param relativePath Relative path from workspace directory
         * @return Full path constrained to app sandbox
         */
        static std::string GetSafePath(const std::string& relativePath);
        
        /**
         * @brief Check if the app has permission to access a path
         * @param path Path to check
         * @param requireWrite True to require write permission, false for read-only
         * @return True if app has permission, false otherwise
         */
        static bool HasPermission(const std::string& path, bool requireWrite = false);
        
        /**
         * @brief Create a default script in the scripts directory
         * @return True if creation succeeded, false otherwise
         */
        static bool CreateDefaultScript();
        
        /**
         * @brief Create a default configuration file
         * @return True if creation succeeded, false otherwise
         */
        static bool CreateDefaultConfig();
    };
}
