#pragma once

#include <string>
#include <functional>
#include <unordered_map>
#include <vector>
#include <map>
#include <iostream>
#include <mutex>

// Forward declarations for Objective-C runtime types
#ifdef __APPLE__
typedef void* Class;
typedef void* Method;
typedef void* SEL;
typedef void* IMP;
typedef void* id;
#else
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
        static bool Initialize();
        
        // Register hooks
        static bool RegisterHook(void* targetAddr, void* hookAddr, void** originalAddr);
        static bool UnregisterHook(void* targetAddr);
        
        // Hook management
        static void ClearAllHooks();
        
    private:
        // Track registered hooks
        static std::unordered_map<void*, void*> s_hookedFunctions;
        static std::mutex s_hookMutex;
    };
    
    // Platform-specific hook implementations
    namespace Implementation {
        // Hook function implementation
        bool HookFunction(void* target, void* replacement, void** original);
        
        // Unhook function implementation
        bool UnhookFunction(void* target);
    }
    
    // Objective-C Method hooking
    class ObjcMethodHook {
    public:
        static bool HookMethod(const std::string& className, const std::string& selectorName, 
                             void* replacementFn, void** originalFn);
        
        static bool UnhookMethod(const std::string& className, const std::string& selectorName);
        
        static void ClearAllHooks();
        
    private:
        // Keep track of hooked methods
        static std::map<std::string, std::pair<Class, SEL>> s_hookedMethods;
        static std::mutex s_methodMutex;
    };
}

// Initialize static members
std::unordered_map<void*, void*> Hooks::HookEngine::s_hookedFunctions;
std::mutex Hooks::HookEngine::s_hookMutex;
std::map<std::string, std::pair<void*, void*>> Hooks::ObjcMethodHook::s_hookedMethods;
std::mutex Hooks::ObjcMethodHook::s_methodMutex;
