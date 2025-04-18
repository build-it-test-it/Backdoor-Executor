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
    
    // Initialize the system with options
    // Initialize system - implementation in init.cpp
    static bool Initialize(const InitOptions& options = InitOptions());
    
    // Clean up and shutdown all systems
    static void Shutdown();

private:
    // Helper functions - implementations remain in this file
    static bool InitializeLogging() {
        try {
            // Initialize logging
            Logging::InitializeLogging(true, true, true);
            
            // Mark logging as initialized
            s_status.loggingInitialized = true;
            return true;
        } catch (const std::exception& ex) {
            std::cerr << "Exception during logging initialization: " << ex.what() << std::endl;
            return false;
        }
    }
    
    static bool InitializeErrorHandling() {
        try {
            // Initialize error handling
            ErrorHandling::ErrorManager::GetInstance().Initialize();
            
            // Configure error handling
            ErrorHandling::ErrorManager::GetInstance().EnableCrashReporting(SystemState::s_options.enableCrashReporting);
            
            if (!SystemState::s_options.crashReportDir.empty()) {
                ErrorHandling::ErrorManager::GetInstance().SetCrashReportPath(SystemState::s_options.crashReportDir);
            }
            
            // Mark error handling as initialized
            SystemState::s_status.errorHandlingInitialized = true;
            return true;
        } catch (const std::exception& ex) {
            Logging::LogCritical("System", "Exception during error handling initialization: " + std::string(ex.what()));
            return false;
        }
    }
    
    static bool InitializeSecurity() {
        try {
            // Initialize security
            bool result = Security::InitializeSecurity(SystemState::s_options.startSecurityMonitoring);
            
            if (result) {
                // Mark security as initialized
                SystemState::s_status.securityInitialized = true;
            }
            
            return result;
        } catch (const std::exception& ex) {
            Logging::LogCritical("System", "Exception during security initialization: " + std::string(ex.what()));
            return false;
        }
    }
    
    static bool InitializeJailbreakBypass() {
        try {
            // Initialize jailbreak bypass
            // Not implemented yet
            return true;
        } catch (const std::exception& ex) {
            Logging::LogCritical("System", "Exception during jailbreak bypass initialization: " + std::string(ex.what()));
            return false;
        }
    }
    
    static bool InitializePerformanceMonitoring() {
        try {
            // Initialize performance monitoring
            Performance::InitializePerformanceMonitoring(
                true,
                SystemState::s_options.enableAutoPerformanceLogging,
                SystemState::s_options.performanceThresholdMs
            );
            
            // Mark performance monitoring as initialized
            SystemState::s_status.performanceInitialized = true;
            return true;
        } catch (const std::exception& ex) {
            Logging::LogCritical("System", "Exception during performance monitoring initialization: " + std::string(ex.what()));
            return false;
        }
    }
    
    static bool InitializeExecutionEngine() {
        try {
            // Create script manager
            s_scriptManager = std::make_shared<iOS::ScriptManager>(
                s_options.enableScriptCaching,
                10, // Max script cache size
                "RobloxScripts"
            );
            
            // Initialize script manager
            if (!s_scriptManager->Initialize()) {
                Logging::LogCritical("System", "Failed to initialize script manager");
                return false;
            }
            
            // Mark script manager as initialized
            s_status.scriptManagerInitialized = true;
            
            // Create execution engine
            s_executionEngine = std::make_shared<iOS::ExecutionEngine>();
            
            // Initialize execution engine
            if (!s_executionEngine->Initialize()) {
                Logging::LogCritical("System", "Failed to initialize execution engine");
                return false;
            }
            
            // Mark execution engine as initialized
            s_status.executionEngineInitialized = true;
            return true;
        } catch (const std::exception& ex) {
            Logging::LogCritical("System", "Exception during execution engine initialization: " + std::string(ex.what()));
            return false;
        }
    }
    
    static bool InitializeUI() {
        try {
            // Create UI controller
            s_uiController = std::make_unique<iOS::UIController>();
            
            // Initialize UI
            if (!s_uiController->Initialize()) {
                Logging::LogWarning("System", "Failed to initialize UI controller");
                return false;
            }
            
            // Configure UI
            s_uiController->SetButtonVisible(s_options.showFloatingButton);
            
            // Connect UI events
            
            // Script execution
            s_uiController->SetExecutionCallback([](const std::string& script) -> bool {
                // Get execution engine
                auto engine = SystemState::GetExecutionEngine();
                if (!engine) {
                    Logging::LogError("UI", "Execute failed: Execution engine not initialized");
                    return false;
                }
                
                // Execute script
                auto result = engine->Execute(script);
                return result.m_success;
            });
            
            // Script saving
            s_uiController->SetSaveScriptCallback([](const std::string& script) -> bool {
                // Get script manager
                auto manager = SystemState::GetScriptManager();
                if (!manager) {
                    Logging::LogError("UI", "Save failed: Script manager not initialized");
                    return false;
                }
                
                // Save script with auto-generated name
                return manager->SaveScript(script);
            });
            
            // Script loading
            s_uiController->SetLoadScriptsCallback([]() -> std::vector<iOS::UIController::ScriptInfo> {
                // Get script manager
                auto manager = SystemState::GetScriptManager();
                if (!manager) {
                    Logging::LogError("UI", "Load failed: Script manager not initialized");
                    return {};
                }
                
                // Get scripts
                auto scripts = manager->GetAllScripts();
                
                // Convert to UI format
                std::vector<iOS::UIController::ScriptInfo> result;
                for (const auto& script : scripts) {
                    iOS::UIController::ScriptInfo info;
                    info.m_name = script.m_name;
                    info.m_content = script.m_content;
                    info.m_timestamp = script.m_modified; // Use the modified timestamp field
                    result.push_back(info);
                }
                
                return result;
            });
            
            // Configure UI
            s_uiController->SetButtonVisible(s_options.showFloatingButton);
            
            Logging::LogInfo("System", "UI system initialized");
            
            // Mark UI as initialized
            s_status.uiInitialized = true;
            return true;
        } catch (const std::exception& ex) {
            Logging::LogCritical("System", "Exception during UI initialization: " + std::string(ex.what()));
            return false;
        }
    }
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
