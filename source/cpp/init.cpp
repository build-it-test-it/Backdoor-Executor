// init.cpp - Implementation for library initialization functionality
#include "init.hpp"
#include "logging.hpp"
#include "performance.hpp"
#include "security/anti_tamper.hpp"

namespace RobloxExecutor {

// Initialize static members
bool SystemState::s_initialized = false;
std::shared_ptr<iOS::ExecutionEngine> SystemState::s_executionEngine = nullptr;
std::shared_ptr<iOS::ScriptManager> SystemState::s_scriptManager = nullptr;
iOS::UIController* SystemState::s_uiController = nullptr;
InitOptions SystemState::s_initOptions;

// Initialize the executor system
bool Initialize(const InitOptions& options) {
    if (SystemState::s_initialized) {
        Logging::LogWarning("System", "RobloxExecutor already initialized");
        return true;
    }

    try {
        Logging::LogInfo("System", "Initializing RobloxExecutor system");
        
        // Store init options
        SystemState::s_initOptions = options;
        
        // Initialize logging system
        if (options.enableLogging) {
            Logging::Logger::InitializeWithFileLogging();
            Logging::LogInfo("System", "Logging system initialized");
        }
        
        // Initialize performance monitoring
        if (options.enablePerformanceMonitoring) {
            Performance::InitializePerformanceMonitoring(
                true,  // enableProfiling
                true,  // enableAutoLogging
                100,   // autoLogThresholdMs
                60000  // monitoringIntervalMs
            );
        }
        
        // Initialize security system
        if (options.enableSecurity) {
            if (Security::AntiTamper::Initialize()) {
                Security::AntiTamper::StartMonitoring();
                Logging::LogInfo("System", "Security system initialized");
            } else {
                Logging::LogError("System", "Failed to initialize security system");
            }
        }
        
        // Create execution engine
        SystemState::s_executionEngine = std::make_shared<iOS::ExecutionEngine>();
        if (!SystemState::s_executionEngine->Initialize()) {
            Logging::LogError("System", "Failed to initialize execution engine");
            return false;
        }
        
        // Create script manager
        SystemState::s_scriptManager = std::make_shared<iOS::ScriptManager>();
        if (!SystemState::s_scriptManager->Initialize()) {
            Logging::LogError("System", "Failed to initialize script manager");
            return false;
        }
        
        // Initialize UI controller if enabled
        if (options.enableUI) {
            SystemState::s_uiController = new iOS::UIController();
            if (!SystemState::s_uiController->Initialize()) {
                Logging::LogError("System", "Failed to initialize UI controller");
                // Continue anyway, as UI is non-critical
            } else {
                Logging::LogInfo("System", "UI controller initialized");
            }
        }
        
        // Initialize jailbreak bypass if enabled
        if (options.enableJailbreakBypass) {
            if (iOS::JailbreakBypass::Initialize()) {
                Logging::LogInfo("System", "Jailbreak bypass initialized");
            } else {
                Logging::LogError("System", "Failed to initialize jailbreak bypass");
                // Continue anyway, as jailbreak bypass is non-critical
            }
        }
        
        SystemState::s_initialized = true;
        Logging::LogInfo("System", "RobloxExecutor system initialization complete");
        return true;
        
    } catch (const std::exception& ex) {
        Logging::LogError("System", "Exception during initialization: " + std::string(ex.what()));
        return false;
    }
}

// Shutdown the executor system
void Shutdown() {
    if (!SystemState::s_initialized) {
        return;
    }

    try {
        Logging::LogInfo("System", "Shutting down RobloxExecutor system");
        
        // Clean up UI controller
        if (SystemState::s_uiController) {
            delete SystemState::s_uiController;
            SystemState::s_uiController = nullptr;
        }
        
        // Clean up script manager
        SystemState::s_scriptManager.reset();
        
        // Clean up execution engine
        SystemState::s_executionEngine.reset();
        
        // Stop security monitoring
        if (SystemState::s_initOptions.enableSecurity) {
            Security::AntiTamper::StopMonitoring();
        }
        
        // Stop performance monitoring
        if (SystemState::s_initOptions.enablePerformanceMonitoring) {
            Performance::Profiler::StopMonitoring();
            Performance::Profiler::SaveReport();
        }
        
        SystemState::s_initialized = false;
        Logging::LogInfo("System", "RobloxExecutor system shutdown complete");
        
    } catch (const std::exception& ex) {
        Logging::LogError("System", "Exception during shutdown: " + std::string(ex.what()));
    }
}

} // namespace RobloxExecutor
