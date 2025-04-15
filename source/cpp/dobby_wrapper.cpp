// Real implementation of Dobby hook functionality
#include "../hooks/hooks.hpp"
#include <iostream>
#include <memory>
#include <unordered_map>
#include <mutex>

// Include Dobby API
#include "dobby.h"

// Track hooked functions
namespace {
    std::mutex g_hookMutex;
    std::unordered_map<void*, void*> g_hookedFunctions;
}

namespace Hooks {

    // Implementation of HookEngine using Dobby
    bool HookEngine::Initialize() {
        std::cout << "Initializing Dobby hook engine..." << std::endl;
        
        // Dobby doesn't need explicit initialization
        return true;
    }
    
    bool HookEngine::RegisterHook(void* targetAddr, void* hookAddr, void** originalAddr) {
        std::lock_guard<std::mutex> lock(g_hookMutex);
        
        // Check if already hooked
        if (g_hookedFunctions.find(targetAddr) != g_hookedFunctions.end()) {
            std::cout << "Function at " << targetAddr << " is already hooked" << std::endl;
            if (originalAddr) {
                *originalAddr = g_hookedFunctions[targetAddr];
            }
            return true;
        }
        
        // Apply the hook using Dobby
        int result = DobbyHook(targetAddr, hookAddr, originalAddr);
        if (result == 0) {
            // Successful hook
            std::cout << "Successfully hooked function at " << targetAddr << std::endl;
            
            // Store the original function pointer
            if (originalAddr) {
                g_hookedFunctions[targetAddr] = *originalAddr;
            }
            return true;
        } else {
            std::cerr << "Failed to hook function at " << targetAddr << ", error code: " << result << std::endl;
            return false;
        }
    }
    
    bool HookEngine::UnregisterHook(void* targetAddr) {
        std::lock_guard<std::mutex> lock(g_hookMutex);
        
        // Check if the function is hooked
        if (g_hookedFunctions.find(targetAddr) == g_hookedFunctions.end()) {
            std::cout << "Function at " << targetAddr << " is not hooked" << std::endl;
            return false;
        }
        
        // Unhook using Dobby
        int result = DobbyUnHook(targetAddr);
        if (result == 0) {
            // Successful unhook
            std::cout << "Successfully unhooked function at " << targetAddr << std::endl;
            g_hookedFunctions.erase(targetAddr);
            return true;
        } else {
            std::cerr << "Failed to unhook function at " << targetAddr << ", error code: " << result << std::endl;
            return false;
        }
    }
    
    void HookEngine::ClearAllHooks() {
        std::lock_guard<std::mutex> lock(g_hookMutex);
        
        std::cout << "Clearing all hooks..." << std::endl;
        
        // Unhook all functions
        for (const auto& pair : g_hookedFunctions) {
            DobbyUnHook(pair.first);
        }
        
        // Clear the map
        g_hookedFunctions.clear();
        
        std::cout << "All hooks cleared" << std::endl;
    }

    namespace Implementation {
        // Direct implementation for hooks
        bool HookFunction(void* target, void* replacement, void** original) {
            return HookEngine::RegisterHook(target, replacement, original);
        }
        
        bool UnhookFunction(void* target) {
            return HookEngine::UnregisterHook(target);
        }
    }
}
