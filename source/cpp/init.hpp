#pragma once

#include <string>
#include <functional>
#include <memory>
#include <atomic>

// Forward declarations
namespace iOS {
    class ExecutionEngine;
    class ScriptManager;
    class UIController;
    
    namespace AIFeatures {
        class AIIntegrationManager;
        class ScriptAssistant;
        class SignatureAdaptation;
    }
}

namespace RobloxExecutor {

/**
 * @struct SystemStatus
 * @brief Tracks the initialization status of various system components
 */
struct SystemStatus {
    bool loggingInitialized = false;
    bool errorHandlingInitialized = false;
    bool securityInitialized = false;
    bool jailbreakBypassInitialized = false;
    bool performanceInitialized = false;
    bool executionEngineInitialized = false;
    bool scriptManagerInitialized = false;
    bool uiInitialized = false;
    bool aiInitialized = false;
    bool namingConventionsInitialized = false;
    bool allSystemsInitialized = false;
};

/**
 * @struct InitOptions
 * @brief Options for initializing the RobloxExecutor system
 */
struct InitOptions {
    // Logging options
    bool enableLogging = true;
    std::string logDir = "";
    
    // Error handling options
    bool enableErrorReporting = true;
    bool enableCrashReporting = true;
    std::string crashReportDir = "";
    
    // Security options
    bool enableSecurity = true;
    bool startSecurityMonitoring = true;
    
    // Bypass options
    bool enableJailbreakBypass = true;
    
    // Performance options
    bool enablePerformanceMonitoring = true;
    bool enableAutoPerformanceLogging = false;
    int performanceThresholdMs = 100;
    
    // Script options
    bool enableScriptCaching = true;
    
    // UI options
    bool enableUI = true;
    bool showFloatingButton = true;
    
    // AI options
    bool enableAI = true;
    
    // Naming conventions options
    bool enableNamingConventions = true;
    
    // Callback to run after initialization
    std::function<void()> postInitCallback = nullptr;
};

/**
 * @class SystemState
 * @brief Manages the global state of the RobloxExecutor system
 * 
 * This class provides static methods to initialize and shutdown the system,
 * as well as access to shared components like the execution engine and script manager.
 */
class SystemState {
public:
    /**
     * @brief Initialize the RobloxExecutor system
     * @param options Initialization options
     * @return True if initialization succeeded, false otherwise
     */
    static bool Initialize(const InitOptions& options = InitOptions());
    
    /**
     * @brief Shutdown the RobloxExecutor system
     */
    static void Shutdown();
    
    /**
     * @brief Check if the system is initialized
     * @return True if initialized, false otherwise
     */
    static bool IsInitialized() { return s_initialized; }
    
    /**
     * @brief Get the current system status
     * @return System status
     */
    static const SystemStatus& GetStatus() { return s_status; }
    
    /**
     * @brief Get the current initialization options
     * @return Initialization options
     */
    static const InitOptions& GetOptions() { return s_options; }
    
    /**
     * @brief Get the execution engine
     * @return Shared pointer to the execution engine
     */
    static std::shared_ptr<iOS::ExecutionEngine> GetExecutionEngine() { return s_executionEngine; }
    
    /**
     * @brief Get the script manager
     * @return Shared pointer to the script manager
     */
    static std::shared_ptr<iOS::ScriptManager> GetScriptManager() { return s_scriptManager; }
    
    /**
     * @brief Get the UI controller
     * @return Unique pointer to the UI controller
     */
    static std::unique_ptr<iOS::UIController>& GetUIController() { return s_uiController; }
    
    /**
     * @brief Get the AI integration manager
     * @return Shared pointer to the AI integration manager
     */
    static std::shared_ptr<iOS::AIFeatures::AIIntegrationManager> GetAIManager() { return s_aiManager; }
    
    /**
     * @brief Get the script assistant
     * @return Shared pointer to the script assistant
     */
    static std::shared_ptr<iOS::AIFeatures::ScriptAssistant> GetScriptAssistant() { return s_scriptAssistant; }
    
    /**
     * @brief Get the signature adaptation
     * @return Shared pointer to the signature adaptation
     */
    static std::shared_ptr<iOS::AIFeatures::SignatureAdaptation> GetSignatureAdaptation() { return s_signatureAdaptation; }
    
private:
    // Private static members
    static std::atomic<bool> s_initialized;
    static SystemStatus s_status;
    static InitOptions s_options;
    
    // Shared components
    static std::shared_ptr<iOS::ExecutionEngine> s_executionEngine;
    static std::shared_ptr<iOS::ScriptManager> s_scriptManager;
    static std::unique_ptr<iOS::UIController> s_uiController;
    
    // AI components
    static std::shared_ptr<iOS::AIFeatures::AIIntegrationManager> s_aiManager;
    static std::shared_ptr<iOS::AIFeatures::ScriptAssistant> s_scriptAssistant;
    static std::shared_ptr<iOS::AIFeatures::SignatureAdaptation> s_signatureAdaptation;
    static void* s_aiIntegration; // Placeholder for future AI integration
};

// Initialize static members
inline std::atomic<bool> SystemState::s_initialized(false);
inline SystemStatus SystemState::s_status;
inline InitOptions SystemState::s_options;
inline std::shared_ptr<iOS::ExecutionEngine> SystemState::s_executionEngine;
inline std::shared_ptr<iOS::ScriptManager> SystemState::s_scriptManager;
inline std::unique_ptr<iOS::UIController> SystemState::s_uiController;
inline std::shared_ptr<iOS::AIFeatures::AIIntegrationManager> SystemState::s_aiManager;
inline std::shared_ptr<iOS::AIFeatures::ScriptAssistant> SystemState::s_scriptAssistant;
inline std::shared_ptr<iOS::AIFeatures::SignatureAdaptation> SystemState::s_signatureAdaptation;
inline void* SystemState::s_aiIntegration = nullptr;

} // namespace RobloxExecutor
