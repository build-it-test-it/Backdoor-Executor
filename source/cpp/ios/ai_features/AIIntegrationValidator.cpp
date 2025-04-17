// AIIntegrationValidator.cpp - Validates integration between AI components and the rest of the codebase
#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <functional>

#include "AIIntegrationManager.h"
#include "HybridAISystem.h"
#include "AIConfig.h"
#include "ScriptAssistant.h"
#include "SignatureAdaptation.h"
#include "../ExecutionEngine.h"
#include "../ScriptManager.h"
#include "../UIController.h"
#include "../../init.hpp"

namespace iOS {
namespace AIFeatures {

/**
 * @class AIIntegrationValidator
 * @brief Validates the integration of AI components with the rest of the codebase
 * 
 * This class performs a series of tests to ensure that the AI components are properly
 * integrated with the rest of the codebase, including script execution, UI, and more.
 */
class AIIntegrationValidator {
public:
    /**
     * @brief Validation result structure
     */
    struct ValidationResult {
        bool success;
        std::string message;
        std::vector<std::string> issues;
        
        ValidationResult() : success(true) {}
        
        void AddIssue(const std::string& issue) {
            issues.push_back(issue);
            success = false;
        }
        
        std::string ToString() const {
            std::stringstream ss;
            ss << "Validation " << (success ? "Passed" : "Failed") << ": " << message << std::endl;
            
            if (!issues.empty()) {
                ss << "Issues:" << std::endl;
                for (const auto& issue : issues) {
                    ss << "- " << issue << std::endl;
                }
            }
            
            return ss.str();
        }
    };
    
    /**
     * @brief Validate AIConfig integration
     * @return Validation result
     */
    static ValidationResult ValidateAIConfig() {
        ValidationResult result;
        result.message = "AIConfig Integration";
        
        try {
            // Get AIConfig instance
            auto& config = AIConfig::GetSharedInstance();
            
            // Ensure it's initialized
            if (!config.IsInitialized()) {
                if (!config.Initialize()) {
                    result.AddIssue("Failed to initialize AIConfig");
                    return result;
                }
            }
            
            // Check for critical methods
            const std::string testModelPath = config.GetModelPath();
            if (testModelPath.empty()) {
                result.AddIssue("GetModelPath() returned empty string");
            }
            
            // Check configuration save/load
            bool saveResult = config.Save();
            if (!saveResult) {
                result.AddIssue("Failed to save configuration");
            }
            
            std::cout << "AIConfig validation successful. Using model path: " << testModelPath << std::endl;
        } catch (const std::exception& e) {
            result.AddIssue(std::string("Exception during AIConfig validation: ") + e.what());
        }
        
        return result;
    }
    
    /**
     * @brief Validate AIIntegrationManager integration
     * @return Validation result
     */
    static ValidationResult ValidateAIIntegrationManager() {
        ValidationResult result;
        result.message = "AIIntegrationManager Integration";
        
        try {
            // Get manager instance
            auto& manager = AIIntegrationManager::GetSharedInstance();
            
            // Ensure it's initialized
            if (!manager.IsInitialized()) {
                if (!manager.Initialize()) {
                    result.AddIssue("Failed to initialize AIIntegrationManager");
                    return result;
                }
            }
            
            // Check for critical components
            auto hybridAI = manager.GetHybridAI();
            if (!hybridAI) {
                result.AddIssue("GetHybridAI() returned null");
            }
            
            auto scriptAssistant = manager.GetScriptAssistant();
            if (!scriptAssistant) {
                result.AddIssue("GetScriptAssistant() returned null");
            }
            
            auto signatureAdaptation = manager.GetSignatureAdaptation();
            if (!signatureAdaptation) {
                result.AddIssue("GetSignatureAdaptation() returned null");
            }
            
            // Check capabilities
            uint32_t capabilities = manager.GetAvailableCapabilities();
            std::cout << "Available AI capabilities: 0x" << std::hex << capabilities << std::dec << std::endl;
            
            // Check online mode
            AIConfig::OnlineMode onlineMode = manager.GetOnlineMode();
            std::cout << "Current online mode: " << static_cast<int>(onlineMode) << std::endl;
            
            // Attempt to set online mode
            manager.SetOnlineMode(AIConfig::OnlineMode::PreferOffline);
            if (manager.GetOnlineMode() != AIConfig::OnlineMode::PreferOffline) {
                result.AddIssue("Failed to set online mode");
            }
            
            std::cout << "AIIntegrationManager validation successful" << std::endl;
        } catch (const std::exception& e) {
            result.AddIssue(std::string("Exception during AIIntegrationManager validation: ") + e.what());
        }
        
        return result;
    }
    
    /**
     * @brief Validate integration with ExecutionEngine
     * @return Validation result
     */
    static ValidationResult ValidateExecutionEngineIntegration() {
        ValidationResult result;
        result.message = "ExecutionEngine Integration";
        
        try {
            // Get manager instance to check AI functionality
            auto& manager = AIIntegrationManager::GetSharedInstance();
            
            // Ensure it's initialized
            if (!manager.IsInitialized()) {
                result.AddIssue("AIIntegrationManager not initialized");
                return result;
            }
            
            // Create an execution engine
            auto executionEngine = std::make_shared<ExecutionEngine>();
            if (!executionEngine->Initialize()) {
                result.AddIssue("Failed to initialize ExecutionEngine");
                return result;
            }
            
            // Generate a simple test script using AI
            std::string testScript;
            bool scriptGenerated = false;
            
            // Set up a callback to receive the generated script
            auto scriptCallback = [&testScript, &scriptGenerated](const std::string& script) {
                testScript = script;
                scriptGenerated = true;
            };
            
            // Generate script
            manager.GenerateScript("Generate a simple print hello world script", "", scriptCallback, false);
            
            // Wait for script generation (up to 5 seconds)
            for (int i = 0; i < 50; i++) {
                if (scriptGenerated) break;
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
            
            if (!scriptGenerated) {
                result.AddIssue("Timeout waiting for script generation");
                return result;
            }
            
            if (testScript.empty() || testScript.find("Error:") == 0) {
                result.AddIssue("Script generation failed: " + testScript);
                return result;
            }
            
            // Execute the generated script
            ExecutionEngine::ExecutionResult execResult = executionEngine->Execute(testScript);
            
            if (!execResult.m_success) {
                result.AddIssue("Failed to execute generated script: " + execResult.m_errorMessage);
                return result;
            }
            
            std::cout << "Successfully executed AI-generated script" << std::endl;
        } catch (const std::exception& e) {
            result.AddIssue(std::string("Exception during ExecutionEngine integration validation: ") + e.what());
        }
        
        return result;
    }
    
    /**
     * @brief Validate integration with Roblox Executor overall system
     * @return Validation result
     */
    static ValidationResult ValidateRobloxExecutorIntegration() {
        ValidationResult result;
        result.message = "RobloxExecutor System Integration";
        
        try {
            // Initialize RobloxExecutor system with AI features enabled
            RobloxExecutor::InitOptions options;
            options.enablePerformanceMonitoring = true;
            options.enableSecurity = true;
            
            bool initResult = RobloxExecutor::Initialize(options);
            if (!initResult) {
                result.AddIssue("Failed to initialize RobloxExecutor system");
                return result;
            }
            
            // Get components
            auto executionEngine = RobloxExecutor::SystemState::GetExecutionEngine();
            if (!executionEngine) {
                result.AddIssue("Failed to get ExecutionEngine from system state");
                return result;
            }
            
            auto scriptManager = RobloxExecutor::SystemState::GetScriptManager();
            if (!scriptManager) {
                result.AddIssue("Failed to get ScriptManager from system state");
                return result;
            }
            
            // Test execution
            const std::string testScript = "print('Hello from test script')";
            auto execResult = executionEngine->Execute(testScript);
            
            if (!execResult.m_success) {
                result.AddIssue("Failed to execute test script: " + execResult.m_errorMessage);
                return result;
            }
            
            // Clean up
            RobloxExecutor::Shutdown();
            
            std::cout << "RobloxExecutor system integration validation successful" << std::endl;
        } catch (const std::exception& e) {
            result.AddIssue(std::string("Exception during RobloxExecutor system validation: ") + e.what());
        }
        
        return result;
    }
    
    /**
     * @brief Run all validation tests
     * @return Overall validation result
     */
    static ValidationResult ValidateAll() {
        ValidationResult result;
        result.message = "Overall AI Integration Validation";
        
        // Run all validation tests
        auto configResult = ValidateAIConfig();
        auto managerResult = ValidateAIIntegrationManager();
        auto engineResult = ValidateExecutionEngineIntegration();
        auto systemResult = ValidateRobloxExecutorIntegration();
        
        // Collect all issues
        if (!configResult.success) {
            for (const auto& issue : configResult.issues) {
                result.AddIssue("AIConfig: " + issue);
            }
        }
        
        if (!managerResult.success) {
            for (const auto& issue : managerResult.issues) {
                result.AddIssue("AIIntegrationManager: " + issue);
            }
        }
        
        if (!engineResult.success) {
            for (const auto& issue : engineResult.issues) {
                result.AddIssue("ExecutionEngine Integration: " + issue);
            }
        }
        
        if (!systemResult.success) {
            for (const auto& issue : systemResult.issues) {
                result.AddIssue("RobloxExecutor Integration: " + issue);
            }
        }
        
        return result;
    }
};

} // namespace AIFeatures
} // namespace iOS

// Main function for standalone testing
int main() {
    using namespace iOS::AIFeatures;
    
    std::cout << "Running AI Integration Validation..." << std::endl;
    
    auto result = AIIntegrationValidator::ValidateAll();
    std::cout << result.ToString() << std::endl;
    
    return result.success ? 0 : 1;
}
