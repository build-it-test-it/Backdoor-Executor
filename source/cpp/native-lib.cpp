#include <iostream>
#include <string>
#include "ios/ExecutionEngine.h"
#include "ios/ScriptManager.h"
#include "hooks/hooks.hpp"
#include "memory/mem.hpp"

// Entry point for the dylib
extern "C" {
    __attribute__((constructor))
    void dylib_initializer() {
        std::cout << "Roblox Executor dylib initialized" << std::endl;
        
        // Initialize hooks
        Hooks::HookEngine::Initialize();
        
        // Initialize memory system
        Memory::Initialize();
    }
    
    __attribute__((destructor))
    void dylib_finalizer() {
        std::cout << "Roblox Executor dylib shutting down" << std::endl;
        
        // Clean up hooks
        Hooks::HookEngine::ClearAllHooks();
    }
    
    // Lua module entry point
    int luaopen_mylibrary(void* L) {
        std::cout << "Lua module loaded: mylibrary" << std::endl;
        
        // This will be called when the Lua state loads our library
        // Perform any Lua-specific initialization here
        
        return 1; // Return 1 to indicate success
    }
}
