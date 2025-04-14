#pragma once

#include <string>
#include <vector>
#include <functional>
#include <unordered_map>
#include <unordered_set>

// Include platform-specific headers
#if defined(__APPLE__) || defined(IOS_TARGET)
#include "MethodSwizzling.h"
#endif

namespace iOS {
    /**
     * @class JailbreakBypass
     * @brief Bypasses jailbreak detection mechanisms in Roblox iOS
     * 
     * This class implements various techniques to prevent Roblox from detecting
     * that it's running on a jailbroken device. It hooks file access, process
     * listing, and other APIs that could be used for jailbreak detection.
     */
    class JailbreakBypass {
    private:
        // Member variables with consistent m_ prefix
        static bool m_initialized;
        static std::unordered_set<std::string> m_jailbreakPaths;
        static std::unordered_set<std::string> m_jailbreakProcesses;
        static std::unordered_map<std::string, std::string> m_fileRedirects;
        
        // Private methods
        static void InitializeTables();
        static void InstallHooks();
        static void PatchMemoryChecks();
        
        // Hook handler declarations
        static int HookStatHandler(const char* path, struct stat* buf);
        static int HookAccessHandler(const char* path, int mode);
        static FILE* HookFopenHandler(const char* path, const char* mode);
        static char* HookGetenvHandler(const char* name);
        static int HookSystemHandler(const char* command);
        static int HookForkHandler(void);
        static int HookExecveHandler(const char* path, char* const argv[], char* const envp[]);
        
    public:
        /**
         * @brief Initialize the jailbreak bypass system
         * @return True if initialization succeeded, false otherwise
         */
        static bool Initialize();
        
        /**
         * @brief Add a path to be hidden from jailbreak detection
         * @param path The path to hide
         */
        static void AddJailbreakPath(const std::string& path);
        
        /**
         * @brief Add a process name to be hidden from jailbreak detection
         * @param processName The process name to hide
         */
        static void AddJailbreakProcess(const std::string& processName);
        
        /**
         * @brief Add a file path redirect
         * @param originalPath The original path that would be accessed
         * @param redirectPath The path to redirect to
         */
        static void AddFileRedirect(const std::string& originalPath, const std::string& redirectPath);
        
        /**
         * @brief Check if a path is a known jailbreak-related path
         * @param path The path to check
         * @return True if the path is jailbreak-related, false otherwise
         */
        static bool IsJailbreakPath(const std::string& path);
        
        /**
         * @brief Check if a process name is a known jailbreak-related process
         * @param processName The process name to check
         * @return True if the process is jailbreak-related, false otherwise
         */
        static bool IsJailbreakProcess(const std::string& processName);
        
        /**
         * @brief Get the redirected path for a given path
         * @param originalPath The original path
         * @return The redirected path, or the original if no redirect exists
         */
        static std::string GetRedirectedPath(const std::string& originalPath);
        
        /**
         * @brief Disable jailbreak detection bypass
         */
        static void Cleanup();
    };
}
