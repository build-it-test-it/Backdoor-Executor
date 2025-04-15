#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <mutex>
#include <unordered_map>
#include "luau/lua_defs.h"
#include "luau/lua.h"
#include "luau/lstate.h"
#include "memory/signature.hpp"

// Global variables for Roblox context
static std::uintptr_t ScriptContext{};  // Roblox's scriptcontext
static lua_State* rL{};                 // Roblox's lstate
static lua_State* eL{};                 // Exploit's lstate

// Address cache with versioning to handle Roblox updates
class AddressCache {
private:
    static std::mutex cacheMutex;
    static std::string currentRobloxVersion;
    static std::unordered_map<std::string, uintptr_t> addressCache;
    
    // Pattern signatures for dynamic scanning
    // These patterns need to be updated based on actual Roblox binaries
    static const char* PATTERN_STARTSCRIPT;
    static const char* PATTERN_GETSTATE;
    static const char* PATTERN_NEWTHREAD;
    static const char* PATTERN_LUAULOAD;
    static const char* PATTERN_SPAWN;
    
    // Fallback addresses if pattern scanning fails
    // These should be updated regularly as Roblox updates
    static const int FALLBACK_STARTSCRIPT;
    static const int FALLBACK_GETSTATE;
    static const int FALLBACK_NEWTHREAD;
    static const int FALLBACK_LUAULOAD;
    static const int FALLBACK_SPAWN;
    
public:
    // Detect current Roblox version
    static std::string GetRobloxVersion() {
        // This is a placeholder. In a real implementation, you'd extract the version from:
        // 1. Roblox binary metadata
        // 2. Version files within the Roblox directory
        // 3. Memory scanning for version strings
        
        // For now, we'll use a hardcoded version
        return "0.599.0";
    }
    
    // Reset the cache when Roblox updates
    static void ResetCache() {
        std::lock_guard<std::mutex> lock(cacheMutex);
        addressCache.clear();
        currentRobloxVersion = GetRobloxVersion();
    }
    
    // Get an address either from cache or by scanning
    static uintptr_t GetAddress(const std::string& name) {
        // Check if Roblox has updated
        std::string version = GetRobloxVersion();
        if (version != currentRobloxVersion) {
            ResetCache();
        }
        
        // Check if address is in cache
        {
            std::lock_guard<std::mutex> lock(cacheMutex);
            auto it = addressCache.find(name);
            if (it != addressCache.end()) {
                return it->second;
            }
        }
        
        // Not in cache, need to scan for it
        uintptr_t address = 0;
        
        // Use pattern scanning here
        if (name == "startscript") {
            address = Memory::PatternScanner::GetAddressByPattern(PATTERN_STARTSCRIPT);
            if (address == 0) address = FALLBACK_STARTSCRIPT;
        } else if (name == "getstate") {
            address = Memory::PatternScanner::GetAddressByPattern(PATTERN_GETSTATE);
            if (address == 0) address = FALLBACK_GETSTATE;
        } else if (name == "newthread") {
            address = Memory::PatternScanner::GetAddressByPattern(PATTERN_NEWTHREAD);
            if (address == 0) address = FALLBACK_NEWTHREAD;
        } else if (name == "luauload") {
            address = Memory::PatternScanner::GetAddressByPattern(PATTERN_LUAULOAD);
            if (address == 0) address = FALLBACK_LUAULOAD;
        } else if (name == "spawn") {
            address = Memory::PatternScanner::GetAddressByPattern(PATTERN_SPAWN);
            if (address == 0) address = FALLBACK_SPAWN;
        }
        
        // Cache the result
        if (address != 0) {
            std::lock_guard<std::mutex> lock(cacheMutex);
            addressCache[name] = address;
        }
        
        return address;
    }
};

// Initialize static members
std::mutex AddressCache::cacheMutex;
std::string AddressCache::currentRobloxVersion = "";
std::unordered_map<std::string, uintptr_t> AddressCache::addressCache;

// Define pattern signatures
const char* AddressCache::PATTERN_STARTSCRIPT = "55 8B EC 83 E4 F8 83 EC 18 56 8B 75 ?? 85 F6 74 ?? 57";
const char* AddressCache::PATTERN_GETSTATE = "55 8B EC 56 8B 75 ?? 83 FE 08 77 ?? 8B 45 ??";
const char* AddressCache::PATTERN_NEWTHREAD = "55 8B EC 56 8B 75 ?? 8B 46 ?? 83 F8 ?? 0F 8C";
const char* AddressCache::PATTERN_LUAULOAD = "55 8B EC 83 EC ?? 53 56 8B 75 ?? 8B 46 ?? 83 F8 ?? 0F 8C";
const char* AddressCache::PATTERN_SPAWN = "55 8B EC 83 EC ?? 56 8B 75 ?? 8B 46 ?? 83 F8 ?? 0F 8C";

// Fallback addresses (due to a stack issue related to thumb in 32 bits roblox you need to add a 1)
const int AddressCache::FALLBACK_STARTSCRIPT = 0x12C993D;
const int AddressCache::FALLBACK_GETSTATE = 0x12B495D;
const int AddressCache::FALLBACK_NEWTHREAD = 0x27A68F1;
const int AddressCache::FALLBACK_LUAULOAD = 0x27BEBB1;
const int AddressCache::FALLBACK_SPAWN = 0x12B66E9;

// Function to get addresses - replace direct access with this
inline uintptr_t GetFunctionAddress(const std::string& name) {
    return AddressCache::GetAddress(name);
}

// Convenience definitions for commonly used addresses
#define startscript_addy GetFunctionAddress("startscript")
#define getstate_addy GetFunctionAddress("getstate")
#define newthread_addy GetFunctionAddress("newthread")
#define luauload_addy GetFunctionAddress("luauload")
#define spawn_addy GetFunctionAddress("spawn")

// Configuration for the executor
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
}