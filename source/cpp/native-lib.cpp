#include <thread>
#include <unistd.h>
#include <iostream>

#include "hooks/hooks.hpp"
#include "memory/mem.hpp"
#include "exec/funcs.hpp"

// Include Dobby only if available (controlled by CMake)
#ifndef NO_DOBBY_HOOKS
  // Skip including dobby.h for iOS builds as it's not available
  #if !defined(IOS_TARGET) && !defined(__APPLE__)
    #include <dobby.h>
  #endif
  #define HOOKING_AVAILABLE 1
#else
  #define HOOKING_AVAILABLE 0
#endif

// Forward declarations for AI integration
namespace iOS {
namespace AIFeatures {
    class AIIntegrationManager;
}}

// Function to initialize the AI subsystem
void initializeAISystem() {
#ifdef ENABLE_AI_FEATURES
    try {
        // Try to access the AI integration manager
        auto& aiManager = iOS::AIFeatures::AIIntegrationManager::GetSharedInstance();
        aiManager.Initialize([](const auto& status) {
            std::cout << "AI System: " << status.m_status << " (" 
                      << (status.m_progress * 100) << "%)" << std::endl;
        });
        
        std::cout << "AI system initialized successfully" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Failed to initialize AI system: " << e.what() << std::endl;
    }
#else
    std::cout << "AI features are disabled" << std::endl;
#endif
}

void mainfunc() {
    std::cout << "Roblox Executor initializing..." << std::endl;

    // Only start when roblox is loaded
    while (!isLibraryLoaded("RobloxLib.framework")) {
        std::cout << "Waiting for Roblox to load..." << std::endl;
        sleep(1);
    }
    
    std::cout << "Roblox loaded, initializing executor..." << std::endl;

    // Init our function pointers
    initfuncs();
    
    // Initialize AI system in the background
    std::thread(initializeAISystem).detach();

#if HOOKING_AVAILABLE
    // Thanks to no memcheck we can just hook StartScript and steal first arg to get script context
    std::cout << "Setting up Roblox script hooks..." << std::endl;
    DobbyHook(reinterpret_cast<void*>(getAddress(startscript_addy)), 
              (void*)&hkstartscript, 
              (void**)&origstartscript);
    std::cout << "Hooks installed successfully" << std::endl;
#else
    std::cout << "Hooking functionality is disabled (Dobby not available)" << std::endl;
#endif

    // testing execution
    // sleep(5);
    // executescript(eL,"loadstring(\"print('Executor loaded successfully!')\")()");
    
    std::cout << "Roblox Executor initialized successfully" << std::endl;
}

[[maybe_unused]] __attribute__((constructor))
void EntryPoint() { 
    std::thread{mainfunc}.detach(); 
}
