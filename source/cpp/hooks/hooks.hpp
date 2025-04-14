#pragma once

// Define CI_BUILD for CI environments
#define CI_BUILD

#include <string>
#include <functional>
#include <unordered_map>
#include <vector>
#include <map>
#include <iostream>

// Stub definitions for Objective-C runtime types
#ifdef CI_BUILD
typedef void* Class;
typedef void* Method;
typedef void* SEL;
typedef void* IMP;
typedef void* id;
#endif

namespace Hooks {
    // Function hook types
    using HookFunction = std::function<void*(void*)>;
    using UnhookFunction = std::function<bool(void*)>;
    
    // Main hooking engine
    class HookEngine {
    public:
        // Initialize the hook engine
        static bool Initialize() {
            std::cout << "HookEngine::Initialize - CI stub" << std::endl;
            return true;
        }
        
        // Register hooks
        static bool RegisterHook(void* targetAddr, void* hookAddr, void** originalAddr) {
            if (originalAddr) *originalAddr = targetAddr;
            std::cout << "HookEngine::RegisterHook - CI stub - " << targetAddr << " -> " << hookAddr << std::endl;
            s_hookedFunctions[targetAddr] = hookAddr;
            return true;
        }

        static bool UnregisterHook(void* targetAddr) {
            std::cout << "HookEngine::UnregisterHook - CI stub - " << targetAddr << std::endl;
            s_hookedFunctions.erase(targetAddr);
            return true;
        }
        
        // Hook management
        static void ClearAllHooks() {
            std::cout << "HookEngine::ClearAllHooks - CI stub" << std::endl;
            s_hookedFunctions.clear();
        }
        
    private:
        // Track registered hooks
        static std::unordered_map<void*, void*> s_hookedFunctions;
    };
    
    // Platform-specific hook implementations
    namespace Implementation {
        // CI build or other platforms - use stub implementations
        inline bool HookFunction(void* target, void* replacement, void** original) {
            // Just store the original function pointer
            if (original) *original = target;
            return true;
        }
        
        inline bool UnhookFunction(void* target) {
            return true;
        }
    }
    
    // Objective-C Method hooking (stub for CI)
    class ObjcMethodHook {
    public:
        static bool HookMethod(const std::string& className, const std::string& selectorName, 
                             void* replacementFn, void** originalFn) {
            std::cout << "ObjcMethodHook::HookMethod - CI stub - " << className << ":" << selectorName << std::endl;
            if (originalFn) *originalFn = nullptr;
            return true;
        }
        
        static bool UnhookMethod(const std::string& className, const std::string& selectorName) {
            std::cout << "ObjcMethodHook::UnhookMethod - CI stub - " << className << ":" << selectorName << std::endl;
            return true;
        }
        
        static void ClearAllHooks() {
            std::cout << "ObjcMethodHook::ClearAllHooks - CI stub" << std::endl;
            s_hookedMethods.clear();
        }
        
    private:
        // Keep track of hooked methods
        static std::map<std::string, std::pair<Class, SEL>> s_hookedMethods;
    };
}

// Initialize static members
std::unordered_map<void*, void*> Hooks::HookEngine::s_hookedFunctions;
std::map<std::string, std::pair<void*, void*>> Hooks::ObjcMethodHook::s_hookedMethods;
