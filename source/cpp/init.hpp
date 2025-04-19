// init.hpp - Main initialization and shutdown functions for the library

#pragma once

#include <memory>
#include <string>
#include <functional>
#include <vector>

#include "security/anti_tamper.hpp"
#include "performance.hpp"
#include "logging.hpp"
#include "ios/ExecutionEngine.h"
#include "ios/UIController.h"
#include "ios/ScriptManager.h"
#include "ios/PatternScanner.h"
#include "ios/ai_features/AIIntegrationManager.h"
#include "ios/ai_features/ScriptAssistant.h"
#include "ios/ai_features/SignatureAdaptation.h"

// Public API for the executor library
namespace RobloxExecutor {

// Pre-initialization options
struct InitOptions {
    // Features to enable
    bool enableLogging = true;                // Enable logging
    bool enableErrorReporting = true;         // Enable error reporting
    bool enableSecurity = true;               // Enable security features
    bool enableJailbreakBypass = false;       // Enable jailbreak bypass
    bool enablePerformanceMonitoring = false; // Enable performance monitoring
    bool enableUI = true;                     // Enable UI features
    bool enableScriptCaching = true;          // Enable script caching
    bool enableAI = false;                    // Enable AI features
    
    // Feature configurations
    bool showFloatingButton = true;            // Show floating button
    bool startSecurityMonitoring = true;       // Start security monitoring
    bool enableCrashReporting = true;          // Enable crash reporting
    bool enableAutoPerformanceLogging = false; // Enable automatic performance logging
    
    // Paths
    std::string crashReportDir;               // Crash report directory
    
    // Thresholds
    uint32_t performanceThresholdMs = 16;     // Performance threshold in milliseconds (1/60 second)
    
    // Callbacks
    using Callback = std::function<void()>;
    using ValidationCallback = std::function<bool()>;
    
    Callback preInitCallback;                 // Called before initialization
    Callback postInitCallback;                // Called after initialization
    ValidationCallback customValidationCallback; // Custom validation callback
};

// System status
struct SystemStatus {
    bool loggingInitialized = false;          // Logging initialized
    bool errorHandlingInitialized = false;    // Error handling initialized
    bool securityInitialized = false;         // Security initialized
    bool jailbreakBypassInitialized = false;  // Jailbreak bypass initialized
    bool performanceInitialized = false;      // Performance monitoring initialized
    bool scriptManagerInitialized = false;    // Script manager initialized
    bool executionEngineInitialized = false;  // Execution engine initialized
    bool uiInitialized = false;               // UI initialized
    bool aiInitialized = false;               // AI initialized
    bool allSystemsInitialized = false;       // All systems initialized
};

// Global system state
class SystemState {

// Make members public so they can be accessed from anywhere
public:
    static bool s_initialized;            // Whether the system is initialized
    static InitOptions s_options;         // Initialization options
    static SystemStatus s_status;         // Current system status
    static std::shared_ptr<iOS::ExecutionEngine> s_executionEngine;
    static std::shared_ptr<iOS::ScriptManager> s_scriptManager;
    static std::unique_ptr<iOS::UIController> s_uiController;
    
    // AI Components
    static void* s_aiIntegration;
    static std::shared_ptr<iOS::AIFeatures::AIIntegrationManager> s_aiManager;
    static std::shared_ptr<iOS::AIFeatures::ScriptAssistant> s_scriptAssistant;
    static std::shared_ptr<iOS::AIFeatures::SignatureAdaptation> s_signatureAdaptation;
    
    // Public API
    // Get system status
    static const SystemStatus& GetStatus() {
        return s_status;
    }
    
    // Check if initialized
    static bool IsInitialized() {
        return s_initialized;
    }
    
    // Get execution engine
    static std::shared_ptr<iOS::ExecutionEngine> GetExecutionEngine() {
        return s_executionEngine;
    }
    
    // Get script manager
    static std::shared_ptr<iOS::ScriptManager> GetScriptManager() {
        return s_scriptManager;
    }
    
    // Get UI controller
    static iOS::UIController* GetUIController() {
        return s_uiController.get();
    }
    
    // Get AI integration
    static void* GetAIIntegration() {
        return s_aiIntegration;
    }
    
    // Get AI manager
    static std::shared_ptr<iOS::AIFeatures::AIIntegrationManager> GetAIManager() {
        return s_aiManager;
    }
    
    // Get script assistant
    static std::shared_ptr<iOS::AIFeatures::ScriptAssistant> GetScriptAssistant() {
        return s_scriptAssistant;
    }
    
    // Get signature adaptation
    static std::shared_ptr<iOS::AIFeatures::SignatureAdaptation> GetSignatureAdaptation() {
        return s_signatureAdaptation;
    }
    
    // Initialize the system with options - implementation in init.cpp
    static bool Initialize(const InitOptions& options = InitOptions());
    
    // Clean up and shutdown all systems - implementation in init.cpp
    static void Shutdown();
};

// Static member variable definitions
bool SystemState::s_initialized = false;
InitOptions SystemState::s_options;
SystemStatus SystemState::s_status;
std::shared_ptr<iOS::ExecutionEngine> SystemState::s_executionEngine;
std::shared_ptr<iOS::ScriptManager> SystemState::s_scriptManager;
std::unique_ptr<iOS::UIController> SystemState::s_uiController;

// AI Components
void* SystemState::s_aiIntegration = nullptr;
std::shared_ptr<iOS::AIFeatures::AIIntegrationManager> SystemState::s_aiManager = nullptr;
std::shared_ptr<iOS::AIFeatures::ScriptAssistant> SystemState::s_scriptAssistant = nullptr;
std::shared_ptr<iOS::AIFeatures::SignatureAdaptation> SystemState::s_signatureAdaptation = nullptr;

// Convenience function for global initialization
inline bool Initialize(const InitOptions& options = InitOptions()) {
    return SystemState::Initialize(options);
}

// Convenience function for global shutdown
inline void Shutdown() {
    SystemState::Shutdown();
}

} // namespace RobloxExecutor
