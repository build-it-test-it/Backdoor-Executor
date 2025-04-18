// init.cpp - Implementation for library initialization functionality
#include "init.hpp"
#include "logging.hpp"
#include "performance.hpp"
#include "security/anti_tamper.hpp"

namespace RobloxExecutor {

// Use existing static members from init.hpp, don't redefine

// Initialize the executor system - implementation of the method declared as friend in SystemState
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
        
        // Initialize AI features if enabled
        if (options.enableAIFeatures) {
            try {
                Logging::LogInfo("System", "Initializing AI features");
                
                // Initialize AI Integration with progress callback
                auto progressCallback = [](float progress) {
                    std::string progressStr = "AI Initialization Progress: " + 
                        std::to_string(static_cast<int>(progress * 100)) + "%";
                    Logging::LogInfo("System", progressStr);
                };
                
                // Create AI integration
                SystemState::s_aiIntegration = ::InitializeAI(progressCallback);
                
                if (!SystemState::s_aiIntegration) {
                    Logging::LogError("System", "Failed to initialize AI integration");
                } else {
                    // Get AI components
                    void* scriptAssistantPtr = ::GetScriptAssistant(SystemState::s_aiIntegration);
                    if (scriptAssistantPtr) {
                        SystemState::s_scriptAssistant = *static_cast<std::shared_ptr<iOS::AIFeatures::ScriptAssistant>*>(scriptAssistantPtr);
                    }
                    
                    void* signatureAdaptationPtr = ::GetSignatureAdaptation(SystemState::s_aiIntegration);
                    if (signatureAdaptationPtr) {
                        SystemState::s_signatureAdaptation = *static_cast<std::shared_ptr<iOS::AIFeatures::SignatureAdaptation>*>(signatureAdaptationPtr);
                    }
                    
                    // Get the AI Manager singleton instance instead of creating a new one
                    SystemState::s_aiManager = std::shared_ptr<iOS::AIFeatures::AIIntegrationManager>(
                        &iOS::AIFeatures::AIIntegrationManager::GetSharedInstance(), 
                        [](iOS::AIFeatures::AIIntegrationManager*){} // no-op deleter for singleton
                    );
                    
                    if (!SystemState::s_aiManager->Initialize()) {
                        Logging::LogWarning("System", "Failed to initialize AI manager");
                        // Continue anyway - manager is optional
                    }
                    
                    // Configure AI components based on options
                    if (SystemState::s_aiManager) {
                        uint32_t capabilities = 0;
                        
                        if (options.enableAIScriptGeneration) {
                            capabilities |= iOS::AIFeatures::AIIntegrationManager::SCRIPT_GENERATION;
                            capabilities |= iOS::AIFeatures::AIIntegrationManager::SCRIPT_DEBUGGING;
                            capabilities |= iOS::AIFeatures::AIIntegrationManager::SCRIPT_ANALYSIS;
                        }
                        
                        if (options.enableAIVulnerabilityDetection) {
                            capabilities |= iOS::AIFeatures::AIIntegrationManager::GAME_ANALYSIS;
                        }
                        
                        if (options.enableAISignatureAdaptation) {
                            capabilities |= iOS::AIFeatures::AIIntegrationManager::SIGNATURE_ADAPTATION;
                        }
                        
                        // Set model path if provided
                        if (!options.aiModelsPath.empty()) {
                            iOS::AIFeatures::AIConfig& config = iOS::AIFeatures::AIConfig::GetSharedInstance();
                            config.SetModelPath(options.aiModelsPath);
                        }
                    }
                    
                    Logging::LogInfo("System", "AI features initialized successfully");
                }
            } catch (const std::exception& ex) {
                Logging::LogError("System", "Exception initializing AI features: " + std::string(ex.what()));
                // Continue anyway, AI features are not critical
            }
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
        
        // Connect AI features to UI if both are initialized
        if (options.enableAIFeatures && options.enableUI && 
            SystemState::s_aiIntegration && SystemState::s_uiController) {
            try {
                // Get main view controller from UI controller
                std::shared_ptr<iOS::UI::MainViewController> mainViewController = 
                    SystemState::s_uiController->GetMainViewController();
                
                if (mainViewController) {
                    // Set up AI with UI
                    ::SetupAIWithUI(SystemState::s_aiIntegration, &mainViewController);
                    
                    // Connect script execution between AI and execution engine
                    if (SystemState::s_scriptAssistant && SystemState::s_executionEngine) {
                        SystemState::s_scriptAssistant->SetExecutionCallback(
                            [](const std::string& script) -> bool {
                                // Get execution engine
                                auto engine = SystemState::GetExecutionEngine();
                                if (!engine) {
                                    Logging::LogError("AI", "Execute failed: Execution engine not initialized");
                                    return false;
                                }
                                
                                // Execute script
                                auto result = engine->Execute(script);
                                return result.m_success;
                            });
                    }
                    
                    Logging::LogInfo("System", "AI features connected to UI successfully");
                }
            } catch (const std::exception& ex) {
                Logging::LogError("System", "Exception connecting AI to UI: " + std::string(ex.what()));
                // Continue anyway, AI-UI connection is not critical
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
        
        // Clean up AI features
        if (SystemState::s_scriptAssistant) {
            SystemState::s_scriptAssistant.reset();
        }
        
        if (SystemState::s_signatureAdaptation) {
            SystemState::s_signatureAdaptation.reset();
        }
        
        if (SystemState::s_aiManager) {
            SystemState::s_aiManager.reset();
        }
        
        if (SystemState::s_aiIntegration) {
            // No explicit cleanup needed for opaque pointer
            SystemState::s_aiIntegration = nullptr;
        }
        
        // Clean up script manager
        SystemState::s_scriptManager.reset();
        
        // Clean up execution engine
        SystemState::s_executionEngine.reset();
        
        // Stop security monitoring
        if (SystemState::s_options.enableSecurity) {
            Security::AntiTamper::StopMonitoring();
        }
        
        // Stop performance monitoring
        if (SystemState::s_options.enablePerformanceMonitoring) {
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
