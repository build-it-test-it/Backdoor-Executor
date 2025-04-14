#pragma once

#include <cstdint>
#include <random>
#include <chrono>
#include <thread>
#include <functional>
#include <vector>
#include <algorithm>
#include <mutex>
#include <map>
#include <unordered_map>
#include <memory>
#include <string>
#include <atomic>

#include "../globals.hpp"
#include "../exec/funcs.hpp"
#include "../exec/impls.hpp"
#include "../enhanced_ui.hpp" 
#include "../luau/lualib.h"
#include "../anti_detection/obfuscator.hpp"
#include "../anti_detection/vm_detect.hpp"
#include "../ios/MemoryAccess.h"

// Check if Dobby is available for hooking
#if defined(HOOKING_AVAILABLE) && !defined(NO_DOBBY_HOOKS)
    #include <dobby.h>
    #define USE_DOBBY 1
#else
    // Stub implementations for when Dobby is not available
    #define USE_DOBBY 0
    inline void* DobbyBind(void* symbol_addr, void* replace_call, void** origin_call) { return nullptr; }
    inline void* DobbyHook(void* address, void* replace_func, void** origin_func) { return nullptr; }
    inline int DobbyDestroy(void* patch_ret_addr) { return 0; }
#endif

// Advanced hook system
namespace Hooks {
    // Custom hook types
    enum class HookType {
        Function,       // Standard C/C++ function hook
        ObjcMethod,     // Objective-C method hook
        VirtualMethod,  // C++ virtual method hook
        IAT,            // Import Address Table hook
        Inline,         // Inline/detour hook
        Breakpoint      // Debug register hook
    };
    
    // Hook status information
    struct HookInfo {
        void* targetAddr;        // Target function address
        void* hookAddr;          // Hook function address
        void* origAddr;          // Original function address (trampoline)
        HookType type;           // Type of hook
        std::string name;        // Name/description of hook
        bool active;             // Whether hook is currently active
        uintptr_t contextData;   // Additional context for special hooks
        
        HookInfo() : targetAddr(nullptr), hookAddr(nullptr), origAddr(nullptr), 
                     type(HookType::Function), active(false), contextData(0) {}
    };
    
    // Thread-safe hook manager
    class HookManager {
    private:
        static std::mutex s_hookMutex;
        static std::unordered_map<std::string, HookInfo> s_hooks;
        static std::atomic<bool> s_initialized;
        
    public:
        // Initialize the hook manager
        static bool Initialize() {
            std::lock_guard<std::mutex> lock(s_hookMutex);
            if (s_initialized) return true;
            
            #if USE_DOBBY
            // Initialize Dobby hooking library if available
            // No explicit initialization needed for Dobby
            s_initialized = true;
            #else
            // Initialize a fallback hooking mechanism
            // For example, prepare inline hook trampolines
            s_initialized = true;
            #endif
            
            return s_initialized;
        }
        
        // Create a hook for a function
        static bool CreateHook(const std::string& name, void* targetFunc, void* hookFunc, void** origFunc, HookType type = HookType::Function) {
            if (!s_initialized && !Initialize()) {
                return false;
            }
            
            std::lock_guard<std::mutex> lock(s_hookMutex);
            
            // Check if hook already exists
            if (s_hooks.find(name) != s_hooks.end()) {
                // Hook already exists, update it if needed
                HookInfo& info = s_hooks[name];
                if (info.active) {
                    // Remove existing hook first
                    RemoveHookInternal(name);
                }
            }
            
            bool success = false;
            
            #if USE_DOBBY
            // Use Dobby hooking library
            void* result = DobbyHook(targetFunc, hookFunc, origFunc);
            success = (result != nullptr);
            #else
            // Fallback to a simpler hooking mechanism
            // For example, direct memory patching or detour
            *origFunc = targetFunc; // Preserve original func pointer
            success = iOS::MemoryAccess::WriteMemory((mach_vm_address_t)targetFunc, &hookFunc, sizeof(void*));
            #endif
            
            if (success) {
                // Store hook information
                HookInfo info;
                info.targetAddr = targetFunc;
                info.hookAddr = hookFunc;
                info.origAddr = *origFunc;
                info.type = type;
                info.name = name;
                info.active = true;
                
                s_hooks[name] = info;
            }
            
            return success;
        }
        
        // Remove a hook by name
        static bool RemoveHook(const std::string& name) {
            std::lock_guard<std::mutex> lock(s_hookMutex);
            return RemoveHookInternal(name);
        }
        
        // Enable or disable a hook
        static bool EnableHook(const std::string& name, bool enable) {
            std::lock_guard<std::mutex> lock(s_hookMutex);
            
            auto it = s_hooks.find(name);
            if (it == s_hooks.end()) {
                return false;
            }
            
            HookInfo& info = it->second;
            
            if (enable == info.active) {
                // Already in desired state
                return true;
            }
            
            if (enable) {
                // Re-enable the hook
                #if USE_DOBBY
                void* origFunc = nullptr;
                void* result = DobbyHook(info.targetAddr, info.hookAddr, &origFunc);
                if (result != nullptr) {
                    info.origAddr = origFunc;
                    info.active = true;
                    return true;
                }
                #else
                // Fallback re-enable
                if (iOS::MemoryAccess::WriteMemory((mach_vm_address_t)info.targetAddr, &info.hookAddr, sizeof(void*))) {
                    info.active = true;
                    return true;
                }
                #endif
            } else {
                // Disable the hook
                #if USE_DOBBY
                if (DobbyDestroy(info.targetAddr) == 0) {
                    info.active = false;
                    return true;
                }
                #else
                // Fallback disable
                if (iOS::MemoryAccess::WriteMemory((mach_vm_address_t)info.targetAddr, &info.origAddr, sizeof(void*))) {
                    info.active = false;
                    return true;
                }
                #endif
            }
            
            return false;
        }
        
        // Get information about a hook
        static bool GetHookInfo(const std::string& name, HookInfo& outInfo) {
            std::lock_guard<std::mutex> lock(s_hookMutex);
            
            auto it = s_hooks.find(name);
            if (it == s_hooks.end()) {
                return false;
            }
            
            outInfo = it->second;
            return true;
        }
        
        // Check if a hook is active
        static bool IsHookActive(const std::string& name) {
            std::lock_guard<std::mutex> lock(s_hookMutex);
            
            auto it = s_hooks.find(name);
            if (it == s_hooks.end()) {
                return false;
            }
            
            return it->second.active;
        }
        
        // Remove all hooks
        static void RemoveAllHooks() {
            std::lock_guard<std::mutex> lock(s_hookMutex);
            
            for (auto& pair : s_hooks) {
                if (pair.second.active) {
                    RemoveHookInternal(pair.first);
                }
            }
            
            s_hooks.clear();
        }
        
    private:
        // Internal implementation of hook removal
        static bool RemoveHookInternal(const std::string& name) {
            auto it = s_hooks.find(name);
            if (it == s_hooks.end()) {
                return false;
            }
            
            HookInfo& info = it->second;
            if (!info.active) {
                return true; // Already removed
            }
            
            bool success = false;
            
            #if USE_DOBBY
            success = (DobbyDestroy(info.targetAddr) == 0);
            #else
            // Fallback hook removal
            success = iOS::MemoryAccess::WriteMemory((mach_vm_address_t)info.targetAddr, &info.origAddr, sizeof(void*));
            #endif
            
            if (success) {
                info.active = false;
            }
            
            return success;
        }
    };
    
    // Initialize static members of HookManager
    std::mutex HookManager::s_hookMutex;
    std::unordered_map<std::string, HookInfo> HookManager::s_hooks;
    std::atomic<bool> HookManager::s_initialized{false};
    
    // Store original function pointers
    int (*origstartscript)(std::uintptr_t thiz, std::uintptr_t script);
    
    // Thread concealment system
    class ThreadConcealer {
    private:
        static std::mutex s_threadMutex;
        static std::vector<std::uintptr_t> s_hiddenThreads;
        static std::unordered_map<std::uintptr_t, std::uintptr_t> s_threadOriginalData;
        
    public:
        // Hide a thread from Roblox's thread monitoring
        static bool HideThread(std::uintptr_t thread) {
            if (!thread) return false;
            
            std::lock_guard<std::mutex> lock(s_threadMutex);
            
            // Check if this thread is already hidden
            if (std::find(s_hiddenThreads.begin(), s_hiddenThreads.end(), thread) != s_hiddenThreads.end()) {
                return true; // Already hidden
            }
            
            // Track the thread
            s_hiddenThreads.push_back(thread);
            
            // Implement thread hiding by manipulating the Lua thread list
            // Get the thread state as a lua_State*
            lua_State* L = reinterpret_cast<lua_State*>(thread);
            
            try {
                // In Lua, the global thread list is usually stored in the global state
                // We need to modify this to hide our thread
                
                // First, save the original next thread pointer
                lua_State* L_next = L->next;
                s_threadOriginalData[thread] = reinterpret_cast<std::uintptr_t>(L_next);
                
                // Then, we need to unlink this thread from the list
                // Find the previous thread that points to this one
                lua_State* g = (lua_State*)GetGlobalLuaState();
                if (g) {
                    lua_State* prev = g;
                    while (prev->next && prev->next != L) {
                        prev = prev->next;
                    }
                    
                    if (prev->next == L) {
                        // Update the previous thread to skip this one
                        prev->next = L->next;
                        
                        // Clear our next pointer to prevent issues
                        L->next = nullptr;
                        
                        // Also modify the thread state to appear dormant
                        L->status = LUA_YIELD; // Make it look like a yielded thread
                        
                        return true;
                    }
                }
            } catch (...) {
                // If anything goes wrong, remove from hidden list
                s_hiddenThreads.erase(
                    std::remove(s_hiddenThreads.begin(), s_hiddenThreads.end(), thread),
                    s_hiddenThreads.end()
                );
                return false;
            }
            
            return false;
        }
        
        // Restore a hidden thread to normal visibility
        static bool UnhideThread(std::uintptr_t thread) {
            if (!thread) return false;
            
            std::lock_guard<std::mutex> lock(s_threadMutex);
            
            // Check if this thread is hidden
            auto it = std::find(s_hiddenThreads.begin(), s_hiddenThreads.end(), thread);
            if (it == s_hiddenThreads.end()) {
                return true; // Not hidden, nothing to do
            }
            
            // Get the original thread data
            auto dataIt = s_threadOriginalData.find(thread);
            if (dataIt == s_threadOriginalData.end()) {
                // No saved data, just remove from list
                s_hiddenThreads.erase(it);
                return false;
            }
            
            // Restore the original thread linkage
            lua_State* L = reinterpret_cast<lua_State*>(thread);
            L->next = reinterpret_cast<lua_State*>(dataIt->second);
            
            // Also restore the thread state
            L->status = LUA_OK;
            
            // Remove from hidden list and data map
            s_hiddenThreads.erase(it);
            s_threadOriginalData.erase(dataIt);
            
            return true;
        }
        
        // Check if a thread is hidden
        static bool IsThreadHidden(std::uintptr_t thread) {
            std::lock_guard<std::mutex> lock(s_threadMutex);
            return std::find(s_hiddenThreads.begin(), s_hiddenThreads.end(), thread) != s_hiddenThreads.end();
        }
        
        // Get the global Lua state
        static std::uintptr_t GetGlobalLuaState() {
            // This would typically come from scanning memory or a known offset
            // For now, use the global rL as a placeholder
            return reinterpret_cast<std::uintptr_t>(rL);
        }
        
        // Clean up all hidden threads
        static void CleanupHiddenThreads() {
            std::lock_guard<std::mutex> lock(s_threadMutex);
            
            // Attempt to restore all hidden threads
            for (auto thread : s_hiddenThreads) {
                auto dataIt = s_threadOriginalData.find(thread);
                if (dataIt != s_threadOriginalData.end()) {
                    try {
                        lua_State* L = reinterpret_cast<lua_State*>(thread);
                        L->next = reinterpret_cast<lua_State*>(dataIt->second);
                        L->status = LUA_OK;
                    } catch (...) {
                        // Ignore errors during cleanup
                    }
                }
            }
            
            s_hiddenThreads.clear();
            s_threadOriginalData.clear();
        }
    };
    
    // Initialize static members of ThreadConcealer
    std::mutex ThreadConcealer::s_threadMutex;
    std::vector<std::uintptr_t> ThreadConcealer::s_hiddenThreads;
    std::unordered_map<std::uintptr_t, std::uintptr_t> ThreadConcealer::s_threadOriginalData;
    
    // Anti-detection system for our hooks
    class HookProtection {
    private:
        static std::random_device s_rd;
        static std::mt19937 s_gen;
        static std::uniform_int_distribution<> s_delayDist;
        static std::atomic<bool> s_protectionEnabled;
        
        // Random delay generator to confuse timing analysis
        static void RandomDelay() {
            if (!s_protectionEnabled) return;
            
            int delay = s_delayDist(s_gen);
            std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        }
        
        // Generate a random pattern of delays to confuse anti-cheat timing checks
        static void RandomizedTiming() {
            if (!s_protectionEnabled) return;
            
            // Vary the number of mini-delays
            std::uniform_int_distribution<> count_dist(1, 3);
            int count = count_dist(s_gen);
            
            for (int i = 0; i < count; i++) {
                // Each mini-delay is between 0.1ms and 1ms
                std::uniform_int_distribution<> micro_delay(100, 1000);
                std::this_thread::sleep_for(std::chrono::microseconds(micro_delay(s_gen)));
                
                // Do a small amount of meaningless computation to prevent optimization
                volatile int dummy = 0;
                for (int j = 0; j < 100; j++) {
                    dummy += j;
                }
            }
        }
        
        // Obscure memory patterns that could be detected
        static void ObscureMemoryPatterns() {
            if (!s_protectionEnabled) return;
            
            // This function would implement techniques to avoid memory scanning detection
            // For example:
            // 1. Encrypt sensitive data when not in use
            // 2. Split critical data across multiple memory locations
            // 3. Use polymorphic code patterns
            
            // For this simplified implementation, just do some dummy operations to look different each time
            static std::vector<uint8_t> dummyBuffer(64);
            std::uniform_int_distribution<uint8_t> byteDist(0, 255);
            
            for (size_t i = 0; i < dummyBuffer.size(); i++) {
                dummyBuffer[i] = byteDist(s_gen);
            }
        }
        
    public:
        // Enable or disable hook protections
        static void SetProtectionEnabled(bool enabled) {
            s_protectionEnabled = enabled;
        }
        
        // Check if protections are enabled
        static bool IsProtectionEnabled() {
            return s_protectionEnabled;
        }
        
        // Apply protections to the hook
        static void ApplyHookProtections() {
            if (!s_protectionEnabled) return;
            
            // Check for debuggers or analysis tools
            if (AntiDetection::AntiDebug::IsDebuggerPresent()) {
                // Subtle countermeasure - add small random delays
                RandomDelay();
                
                // Take more aggressive countermeasures if in debug mode
                ObscureMemoryPatterns();
            }
            
            // Apply anti-VM measures if VM detected
            if (AntiDetection::VMDetection::DetectVM()) {
                // Subtly alter behavior in VMs
                RandomDelay();
            }
            
            // Apply randomized timing to confuse pattern recognition
            RandomizedTiming();
        }
        
        // Conceal a function hook from detection
        static void ConcealFunctionHook(const std::string& hookName) {
            if (!s_protectionEnabled) return;
            
            HookInfo info;
            if (!HookManager::GetHookInfo(hookName, info)) {
                return;
            }
            
            // This would implement techniques to hide hook trampoline code:
            // 1. Encrypt/decrypt hook code on demand
            // 2. Use unusual code patterns that avoid detection
            // 3. Periodically move hook code to new memory locations
            
            // For this simplified implementation, do nothing complex
        }
    };
    
    // Initialize static members of HookProtection
    std::random_device HookProtection::s_rd;
    std::mt19937 HookProtection::s_gen(HookProtection::s_rd());
    std::uniform_int_distribution<> HookProtection::s_delayDist(1, 5);
    std::atomic<bool> HookProtection::s_protectionEnabled{true};
    
    // Initialize a secure thread environment
    bool InitializeSecureThread(lua_State* thread) {
        if (!thread) return false;
        
        try {
            // Sandbox the thread for security
            luaL_sandboxthread(thread);
            
            // Set high execution privileges (identity 8)
            // Note: This approach accesses internal Lua structures which may change with updates
            // Use a more robust approach by dynamically finding offsets
            
            // Try to get identity field at different potential offsets
            // These offsets can vary based on Roblox's Lua implementation
            static const int possibleOffsets[] = {72, 80, 88, 96, 104};
            bool foundIdentityField = false;
            
            for (int offset : possibleOffsets) {
                if (offset + 24 < sizeof(lua_State)) {
                    continue; // Skip if offset would be out of bounds
                }
                
                try {
                    auto userdata = *reinterpret_cast<std::uintptr_t*>((std::uintptr_t)(thread) + offset);
                    if (userdata != 0) {
                        // Test if we can safely access userdata+24
                        if (iOS::MemoryAccess::IsAddressValid(userdata + 24, sizeof(std::uintptr_t))) {
                            // Set identity to 8 (high privileges)
                            *reinterpret_cast<std::uintptr_t*>(userdata + 24) = 8;
                            foundIdentityField = true;
                            break;
                        }
                    }
                } catch (...) {
                    // Ignore and try next offset
                    continue;
                }
            }
            
            // If we couldn't find identity field, try the default offset
            if (!foundIdentityField) {
                try {
                    auto userdata = *reinterpret_cast<std::uintptr_t*>((std::uintptr_t)(thread) + 72);
                    *reinterpret_cast<std::uintptr_t*>(userdata + 24) = 8;
                } catch (...) {
                    // Ignore if this fails too
                }
            }
            
            // Make the _G Table
            lua_createtable(thread, 0, 0);
            lua_setfield(thread, -10002, "_G");
            
            // Register our custom functions
            regImpls(thread);
            
            // Hide the thread from detection
            ThreadConcealer::HideThread(reinterpret_cast<std::uintptr_t>(thread));
            
            return true;
        } catch (const std::exception& e) {
            fprintf(stderr, "Error initializing secure thread: %s\n", e.what());
            return false;
        } catch (...) {
            fprintf(stderr, "Unknown error initializing secure thread\n");
            return false;
        }
    }
    
    // Class for Objective-C method swizzling
    class ObjcMethodHook {
    private:
        static std::mutex s_methodMutex;
        static std::map<std::string, std::pair<Class, SEL>> s_hookedMethods;
        
    public:
        // Hook an Objective-C method (swizzling)
        static bool HookMethod(const std::string& className, const std::string& selectorName, 
                              void* replacementFn, void** originalFn) {
            std::lock_guard<std::mutex> lock(s_methodMutex);
            
            // Get the Objective-C class
            Class cls = objc_getClass(className.c_str());
            if (!cls) {
                fprintf(stderr, "Class not found: %s\n", className.c_str());
                return false;
            }
            
            // Get the selector
            SEL selector = sel_registerName(selectorName.c_str());
            if (!selector) {
                fprintf(stderr, "Selector not found: %s\n", selectorName.c_str());
                return false;
            }
            
            // Get the method
            Method method = class_getInstanceMethod(cls, selector);
            if (!method) {
                fprintf(stderr, "Method not found: %s.%s\n", className.c_str(), selectorName.c_str());
                return false;
            }
            
            // Get the implementation
            IMP originalImpl = method_getImplementation(method);
            if (originalFn) {
                *originalFn = (void*)originalImpl;
            }
            
            // Replace the implementation
            IMP newImpl = (IMP)replacementFn;
            method_setImplementation(method, newImpl);
            
            // Store the hooked method
            std::string key = className + "." + selectorName;
            s_hookedMethods[key] = std::make_pair(cls, selector);
            
            return true;
        }
        
        // Restore an Objective-C method
        static bool RestoreMethod(const std::string& className, const std::string& selectorName) {
            std::lock_guard<std::mutex> lock(s_methodMutex);
            
            std::string key = className + "." + selectorName;
            auto it = s_hookedMethods.find(key);
            if (it == s_hookedMethods.end()) {
                return false;
            }
            
            Class cls = it->second.first;
            SEL selector = it->second.second;
            
            // Get the method again
            Method method = class_getInstanceMethod(cls, selector);
            if (!method) {
                return false;
            }
            
            // We need the original implementation, which we don't store
            // In a real implementation, you'd store this too
            // This is just a placeholder to show how it would work
            IMP originalImpl = nullptr;
            method_setImplementation(method, originalImpl);
            
            s_hookedMethods.erase(it);
            return true;
        }
    };
    
    // Initialize static members of ObjcMethodHook
    std::mutex ObjcMethodHook::s_methodMutex;
    std::map<std::string, std::pair<Class, SEL>> ObjcMethodHook::s_hookedMethods;
}

// Advanced hook implementation for Roblox's startscript function
int hkstartscript(std::uintptr_t thiz, std::uintptr_t rscript) {
    // Apply anti-detection measures before proceeding
    Hooks::HookProtection::ApplyHookProtections();
    
    // Check if game context changed
    bool contextChanged = false;
    
    // Use a try/catch to prevent crashes from unexpected Roblox updates
    try {
        if (ScriptContext != thiz) {
            // Store the new context
            ScriptContext = thiz;
            contextChanged = true;
        }
    } catch (...) {
        // If an exception occurs, just treat it as if context changed
        contextChanged = true;
    }
    
    // If the context has changed, we need to reinitialize our environment
    if (contextChanged) {
        try {
            // Set up identity for getting main state
            int id[2] = {8, 0};
            int script[] = {0, 0}; // Using 0 instead of NULL to avoid conversion warnings
            
            // Get the main Lua state
            rL = rlua_getmainstate(thiz, reinterpret_cast<uintptr_t>(id), reinterpret_cast<uintptr_t>(script));
            if (!rL) {
                // Handle error getting main state
                fprintf(stderr, "Failed to get main state\n");
                goto original_call;
            }
            
            // Create our thread for execution
            eL = rlua_newthread(rL);
            if (!eL) {
                // Handle error creating thread
                fprintf(stderr, "Failed to create new thread\n");
                goto original_call;
            }
            
            // Initialize our secure thread
            if (!Hooks::InitializeSecureThread(eL)) {
                fprintf(stderr, "Failed to initialize secure thread\n");
                goto original_call;
            }
            
            // Load and execute our enhanced UI
            ExecutionStatus status = executescript(eL, EnhancedUI::GetCompleteUI(), "ExecutorUI");
            
            // Check execution status
            if (!status.success) {
                fprintf(stderr, "Failed to execute UI: %s\n", status.error.c_str());
                // We continue anyway because the UI might fail but the game should still run
            }
        }
        catch (const std::exception& e) {
            // Log error but continue to original function
            fprintf(stderr, "Exception in hkstartscript: %s\n", e.what());
        }
    }
    
original_call:
    // Apply another anti-detection measure before calling the original
    Hooks::HookProtection::ApplyHookProtections();
    
    // Call the original function
    return Hooks::origstartscript(thiz, rscript);
}

// The hook initialization function
bool InitializeHooks() {
    // Initialize the hook manager
    if (!Hooks::HookManager::Initialize()) {
        fprintf(stderr, "Failed to initialize hook manager\n");
        return false;
    }
    
    // Apply anti-tampering measures
    if (ExecutorConfig::EnableAntiDetection) {
        AntiDetection::AntiDebug::ApplyAntiTamperingMeasures();
    }
    
    // Get the address of startscript
    uintptr_t startscriptAddr = getAddress(startscript_addy);
    if (startscriptAddr == 0) {
        fprintf(stderr, "Failed to get startscript address\n");
        return false;
    }
    
    // Create the hook
    bool success = Hooks::HookManager::CreateHook(
        "startscript",
        (void*)startscriptAddr,
        (void*)hkstartscript,
        (void**)&Hooks::origstartscript
    );
    
    if (!success) {
        fprintf(stderr, "Failed to create startscript hook\n");
        return false;
    }
    
    fprintf(stderr, "Hooks initialized successfully\n");
    return true;
}

// Cleanup function to remove all hooks
void CleanupHooks() {
    // Disable hook protections during cleanup
    Hooks::HookProtection::SetProtectionEnabled(false);
    
    // Remove all hooks
    Hooks::HookManager::RemoveAllHooks();
    
    // Clean up hidden threads
    Hooks::ThreadConcealer::CleanupHiddenThreads();
    
    fprintf(stderr, "Hooks cleaned up\n");
}