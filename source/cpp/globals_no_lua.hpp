// globals_no_lua.hpp
// A version of globals.hpp that doesn't require Lua headers
// Used to break circular dependencies

#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <mutex>
#include <unordered_map>
#include <atomic>
#include <thread>
#include <chrono>
#include <mach-o/dyld.h>
#include <dlfcn.h>
#include <sys/sysctl.h>
#include <sys/stat.h>

// Forward declarations for Lua types
struct lua_State;

// Configuration for the executor - optimized for iOS
namespace ExecutorConfig {
    // Whether to enable advanced anti-detection features
    static bool EnableAntiDetection = true;
    
    // Whether to enable script obfuscation for outgoing scripts
    static bool EnableScriptObfuscation = true;
    
    // Whether to enable VM detection countermeasures
    static bool EnableVMDetection = true;
    
    // Whether to encrypt stored scripts
    static bool EncryptSavedScripts = true;
    
    // Script execution timeout in milliseconds (0 = no timeout)
    static int ScriptExecutionTimeout = 5000;
    
    // Auto-retry on failed execution
    static bool AutoRetryFailedExecution = true;
    static int MaxAutoRetries = 3;
    
    // iOS-specific options
    namespace iOS {
        // Memory usage settings
        static int MemoryLimitMB = 256;  // Limit memory usage to avoid watchdog termination
        
        // UI Configuration
        static bool UseFloatingButton = true;
        static bool AutoHideUIInScreenshots = true;
        
        // Battery optimization
        static bool EnableBatteryOptimization = true;
        
        // Network settings
        static bool UseSecureConnections = true;
        static bool BlockTeleportRequests = false;  // Will be user-configurable
        
        // Stability settings
        static bool CrashRecoveryEnabled = true;
        static int BackgroundTimeout = 30;  // Seconds before cleaning up when app goes to background
    }
    
    // Advanced execution options
    namespace Advanced {
        // Cache compiled scripts to improve performance
        static bool EnableScriptCaching = true;
        
        // Enable self-modification capabilities for anti-detection
        static bool EnableSelfModification = true;
        
        // Bypass specific checks
        static bool BypassJailbreakDetection = true;
        static bool BypassIntegrityChecks = true;
        
        // Security options
        static bool ObfuscateInternalFunctions = true;
        static bool RandomizeMemoryLayout = true;
        
        // Debug options - disabled in production
        static bool EnableDebugLogs = false;
    }
}

// Function to get addresses - replace direct access with this
inline uintptr_t GetFunctionAddress(const std::string& name) {
    return 0; // Stub implementation - real one in globals.hpp
}

// Convenience definitions for commonly used addresses
#define startscript_addy GetFunctionAddress("startscript")
#define getstate_addy GetFunctionAddress("getstate")
#define newthread_addy GetFunctionAddress("newthread")
#define luauload_addy GetFunctionAddress("luauload")
#define spawn_addy GetFunctionAddress("spawn")
