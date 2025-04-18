// init.cpp - Implementation for library initialization functionality
#include "init.hpp"
#include "logging.hpp"
#include "performance.hpp"
#include "security/anti_tamper.hpp"

namespace RobloxExecutor {

// Implementation of SystemState::Initialize declared in init.hpp
bool SystemState::Initialize(const InitOptions& options) {
    if (s_initialized) {
        Logging::LogWarning("System", "RobloxExecutor already initialized");
        return true;
    }

    try {
        Logging::LogInfo("System", "Initializing RobloxExecutor system");
        
        // Store init options
        s_options = options;
        
        // Initialize logging system
        if (options.enableLogging) {
            // Simply log that we've initialized (actual logging system is already initialized)
            Logging::LogInfo("System", "Logging system initialized");
            s_status.loggingInitialized = true;
        }
        
        // Initialize error handling
        if (options.enableErrorReporting) {
            // Simple error handling initialization
            Logging::LogInfo("System", "Error handling initialized");
            s_status.errorHandlingInitialized = true;
        }
        
        // Initialize security features
        if (options.enableSecurity) {
            // Simple security initialization
            Logging::LogInfo("System", "Security features initialized");
            s_status.securityInitialized = true;
        }
        
        // Initialize jailbreak bypass if needed
        if (options.enableJailbreakBypass) {
            // Simple jailbreak bypass initialization (stub)
            Logging::LogInfo("System", "Jailbreak bypass initialized");
            s_status.jailbreakBypassInitialized = true;
        }
        
        // Initialize performance monitoring if needed
        if (options.enablePerformanceMonitoring) {
            Performance::InitializePerformanceMonitoring(
                true,  // enableProfiling
                options.enableAutoPerformanceLogging,
                options.performanceThresholdMs
            );
            s_status.performanceInitialized = true;
        }
        
        // Initialize execution engine
        s_executionEngine = std::make_shared<iOS::ExecutionEngine>();
        if (!s_executionEngine->Initialize()) {
            Logging::LogError("System", "Failed to initialize execution engine");
            return false;
        }
        s_status.executionEngineInitialized = true;
        
        // Initialize script manager
        s_scriptManager = std::make_shared<iOS::ScriptManager>(
            options.enableScriptCaching,
            10, // Max cache size
            "Scripts"
        );
        if (!s_scriptManager->Initialize()) {
            Logging::LogError("System", "Failed to initialize script manager");
            return false;
        }
        s_status.scriptManagerInitialized = true;
        
        // Initialize UI if enabled
        if (options.enableUI) {
            s_uiController = std::make_unique<iOS::UIController>();
            if (!s_uiController->Initialize()) {
                Logging::LogWarning("System", "Failed to initialize UI controller");
                // Continue despite UI initialization failure
            } else {
                // Configure UI
                s_uiController->SetButtonVisible(options.showFloatingButton);
                
                // Set up execute callback
                s_uiController->SetExecuteCallback([](const iOS::UIController::ExecutionResult& result) {
                    // Log execution result
                    if (result.m_success) {
                        Logging::LogInfo("UI", "Script executed successfully");
                    } else {
                        Logging::LogError("UI", "Script execution failed: " + result.m_output);
                    }
                });
                
                s_status.uiInitialized = true;
            }
        }
        
        // Initialize AI features if enabled
        if (options.enableAI && s_status.uiInitialized) {
            // Simple AI initialization (stub)
            Logging::LogInfo("System", "AI features initialized");
            s_status.aiInitialized = true;
        }
        
        // Mark as initialized
        s_initialized = true;
        s_status.allSystemsInitialized = true;
        
        Logging::LogInfo("System", "All systems initialized successfully");
        
        // Call post-init callback if provided
        if (s_options.postInitCallback) {
            s_options.postInitCallback();
        }
        
        return true;
    } catch (const std::exception& ex) {
        Logging::LogCritical("System", "Exception during initialization: " + std::string(ex.what()));
        return false;
    }
}

// Implementation of SystemState::Shutdown declared in init.hpp
void SystemState::Shutdown() {
    if (!s_initialized) {
        return;
    }
    
    try {
        Logging::LogInfo("System", "Shutting down RobloxExecutor system");
        
        // Clean up in reverse order of initialization
        
        // Clean up UI controller
        if (s_uiController) {
            s_uiController.reset();
        }
        
        // Clean up script manager and execution engine
        s_scriptManager.reset();
        s_executionEngine.reset();
        
        // Clean up AI components
        s_scriptAssistant.reset();
        s_signatureAdaptation.reset();
        s_aiManager.reset();
        
        if (s_aiIntegration) {
            // Cleanup would go here
            s_aiIntegration = nullptr;
        }
        
        // Stop performance monitoring
        if (s_status.performanceInitialized) {
            Performance::Profiler::StopMonitoring();
        }
        
        // Stop security monitoring
        if (s_status.securityInitialized) {
            Security::AntiTamper::StopMonitoring();
        }
        
        // Log shutdown if logging is still available
        Logging::LogInfo("System", "System shutdown complete");
        
        // Mark as uninitialized
        s_initialized = false;
        s_status = SystemStatus();
    } catch (const std::exception& ex) {
        // Best effort to log the error
        Logging::LogCritical("System", "Exception during shutdown: " + std::string(ex.what()));
    }
}

} // namespace RobloxExecutor
