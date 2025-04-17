// AIIntegrationTests.cpp - Comprehensive tests for AI integration
#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <future>
#include <chrono>
#include <thread>
#include <cassert>

#include "../AIIntegrationManager.h"
#include "../HybridAISystem.h"
#include "../AIConfig.h"
#include "../ScriptAssistant.h"
#include "../SignatureAdaptation.h"

using namespace iOS::AIFeatures;

// Test utilities
namespace {
    // Simple test result class
    class TestResult {
    public:
        enum Status { PASS, FAIL, SKIP };
        
        TestResult(const std::string& name) : m_name(name), m_status(SKIP), m_message("Not run") {}
        
        void Pass() {
            m_status = PASS;
            m_message = "Passed";
        }
        
        void Fail(const std::string& message) {
            m_status = FAIL;
            m_message = message;
        }
        
        void Skip(const std::string& message) {
            m_status = SKIP;
            m_message = message;
        }
        
        std::string GetStatusString() const {
            switch (m_status) {
                case PASS: return "PASS";
                case FAIL: return "FAIL";
                case SKIP: return "SKIP";
                default: return "UNKNOWN";
            }
        }
        
        std::string GetName() const { return m_name; }
        std::string GetMessage() const { return m_message; }
        bool IsPassed() const { return m_status == PASS; }
        bool IsFailed() const { return m_status == FAIL; }
        
    private:
        std::string m_name;
        Status m_status;
        std::string m_message;
    };
    
    // Test suite class
    class TestSuite {
    public:
        TestSuite(const std::string& name) : m_name(name) {}
        
        void AddTest(const std::function<TestResult()>& test) {
            m_tests.push_back(test);
        }
        
        std::vector<TestResult> RunAll() {
            std::vector<TestResult> results;
            
            std::cout << "=== Running Test Suite: " << m_name << " ===" << std::endl;
            
            for (const auto& test : m_tests) {
                try {
                    auto result = test();
                    results.push_back(result);
                    
                    std::cout << "[" << result.GetStatusString() << "] " 
                              << result.GetName() << ": " << result.GetMessage() << std::endl;
                } catch (const std::exception& e) {
                    TestResult result("Unknown Test");
                    result.Fail(std::string("Exception: ") + e.what());
                    results.push_back(result);
                    
                    std::cout << "[FAIL] Unknown Test: Exception: " << e.what() << std::endl;
                }
            }
            
            // Print summary
            int passed = 0, failed = 0, skipped = 0;
            for (const auto& result : results) {
                if (result.IsPassed()) passed++;
                else if (result.IsFailed()) failed++;
                else skipped++;
            }
            
            std::cout << "=== Test Suite Summary: " << m_name << " ===" << std::endl;
            std::cout << "Total: " << results.size() << ", Passed: " << passed 
                      << ", Failed: " << failed << ", Skipped: " << skipped << std::endl;
            
            return results;
        }
        
    private:
        std::string m_name;
        std::vector<std::function<TestResult()>> m_tests;
    };
}

// Test functions
namespace AITest {
    // Test AIConfig initialization and settings
    TestResult TestAIConfigInitialization() {
        TestResult result("AIConfig Initialization");
        
        try {
            // Get shared instance
            auto& config = AIConfig::GetSharedInstance();
            
            // Initialize
            bool initResult = config.Initialize();
            if (!initResult) {
                result.Fail("AIConfig initialization failed");
                return result;
            }
            
            // Test setter and getter for API key
            const std::string testApiKey = "test_api_key_12345";
            config.SetAPIKey(testApiKey);
            if (config.GetAPIKey() != testApiKey) {
                result.Fail("SetAPIKey/GetAPIKey failed");
                return result;
            }
            
            // Test setter and getter for model path
            const std::string testModelPath = "/tmp/test_models";
            config.SetModelPath(testModelPath);
            if (config.GetModelPath() != testModelPath) {
                result.Fail("SetModelPath/GetModelPath failed");
                return result;
            }
            
            // Test setter and getter for online mode
            config.SetOnlineMode(AIConfig::OnlineMode::PreferOffline);
            if (config.GetOnlineMode() != AIConfig::OnlineMode::PreferOffline) {
                result.Fail("SetOnlineMode/GetOnlineMode failed");
                return result;
            }
            
            // Test setter and getter for model quality
            config.SetModelQuality(AIConfig::ModelQuality::High);
            if (config.GetModelQuality() != AIConfig::ModelQuality::High) {
                result.Fail("SetModelQuality/GetModelQuality failed");
                return result;
            }
            
            result.Pass();
        } catch (const std::exception& e) {
            result.Fail(std::string("Exception: ") + e.what());
        }
        
        return result;
    }
    
    // Test HybridAISystem initialization and basic functionality
    TestResult TestHybridAISystemInitialization() {
        TestResult result("HybridAISystem Initialization");
        
        try {
            // Create system
            auto system = std::make_shared<HybridAISystem>();
            
            // Initialize with test paths
            bool initResult = system->Initialize(
                "/tmp/test_models",
                "https://api.example.com",
                "test_api_key"
            );
            
            if (!initResult) {
                result.Fail("HybridAISystem initialization failed");
                return result;
            }
            
            // Verify initialization state
            if (!system->IsInitialized()) {
                result.Fail("IsInitialized() returns false after successful initialization");
                return result;
            }
            
            // Set and get online mode
            system->SetOnlineMode(HybridAISystem::OnlineMode::OfflineOnly);
            if (system->GetOnlineMode() != HybridAISystem::OnlineMode::OfflineOnly) {
                result.Fail("SetOnlineMode/GetOnlineMode failed");
                return result;
            }
            
            // Test API key setting
            const std::string newApiKey = "new_test_api_key";
            system->SetAPIKey(newApiKey);
            
            // Test memory settings
            const uint64_t testMemory = 100 * 1024 * 1024; // 100MB
            system->SetMaxMemory(testMemory);
            
            result.Pass();
        } catch (const std::exception& e) {
            result.Fail(std::string("Exception: ") + e.what());
        }
        
        return result;
    }
    
    // Test script generation functionality
    TestResult TestScriptGeneration() {
        TestResult result("Script Generation");
        
        try {
            // Create system
            auto system = std::make_shared<HybridAISystem>();
            
            // Initialize
            bool initResult = system->Initialize(
                "/tmp/test_models",
                "",  // No API endpoint
                ""   // No API key
            );
            
            if (!initResult) {
                result.Skip("HybridAISystem initialization failed");
                return result;
            }
            
            // Force offline mode
            system->SetOnlineMode(HybridAISystem::OnlineMode::OfflineOnly);
            
            // Setup promise and future for callback
            std::promise<std::string> promise;
            auto future = promise.get_future();
            
            // Generate script
            system->GenerateScript(
                "Create a simple ESP script for Roblox",
                "fps game",
                [&promise](const std::string& script) {
                    promise.set_value(script);
                },
                false  // Force offline
            );
            
            // Wait for result with timeout
            auto futureStatus = future.wait_for(std::chrono::seconds(5));
            if (futureStatus != std::future_status::ready) {
                result.Fail("Timeout waiting for script generation");
                return result;
            }
            
            // Get generated script
            std::string generatedScript = future.get();
            
            // Verify script is not empty
            if (generatedScript.empty()) {
                result.Fail("Generated script is empty");
                return result;
            }
            
            // Check for error message
            if (generatedScript.find("Error:") == 0) {
                result.Fail("Script generation error: " + generatedScript);
                return result;
            }
            
            result.Pass();
        } catch (const std::exception& e) {
            result.Fail(std::string("Exception: ") + e.what());
        }
        
        return result;
    }
    
    // Test script debugging functionality
    TestResult TestScriptDebugging() {
        TestResult result("Script Debugging");
        
        try {
            // Create system
            auto system = std::make_shared<HybridAISystem>();
            
            // Initialize
            bool initResult = system->Initialize(
                "/tmp/test_models",
                "",  // No API endpoint
                ""   // No API key
            );
            
            if (!initResult) {
                result.Skip("HybridAISystem initialization failed");
                return result;
            }
            
            // Force offline mode
            system->SetOnlineMode(HybridAISystem::OnlineMode::OfflineOnly);
            
            // Test script with error
            const std::string testScript = 
                "function test()\n"
                "   print(\"Hello World\")\n"
                "   for i = 1, 10 do\n"
                "       print(i)\n"
                "   -- Missing end statement\n"
                "   return true\n"
                "end\n";
                
            // Setup promise and future for callback
            std::promise<std::string> promise;
            auto future = promise.get_future();
            
            // Debug script
            system->DebugScript(
                testScript,
                [&promise](const std::string& result) {
                    promise.set_value(result);
                },
                false  // Force offline
            );
            
            // Wait for result with timeout
            auto futureStatus = future.wait_for(std::chrono::seconds(5));
            if (futureStatus != std::future_status::ready) {
                result.Fail("Timeout waiting for script debugging");
                return result;
            }
            
            // Get debug result
            std::string debugResult = future.get();
            
            // Verify result is not empty
            if (debugResult.empty()) {
                result.Fail("Debug result is empty");
                return result;
            }
            
            // Check for error detection
            if (debugResult.find("Error") == std::string::npos && 
                debugResult.find("Missing") == std::string::npos) {
                result.Fail("Debug system failed to detect missing 'end' statement");
                return result;
            }
            
            result.Pass();
        } catch (const std::exception& e) {
            result.Fail(std::string("Exception: ") + e.what());
        }
        
        return result;
    }
    
    // Test AIIntegrationManager initialization and API
    TestResult TestAIIntegrationManager() {
        TestResult result("AIIntegrationManager Initialization");
        
        try {
            // Get shared instance
            auto& manager = AIIntegrationManager::GetSharedInstance();
            
            // Initialize
            bool initResult = manager.Initialize();
            if (!initResult) {
                result.Fail("AIIntegrationManager initialization failed");
                return result;
            }
            
            // Check if initialized
            if (!manager.IsInitialized()) {
                result.Fail("Manager reports not initialized after successful initialization");
                return result;
            }
            
            // Test components
            if (!manager.GetHybridAI()) {
                result.Fail("GetHybridAI() returned null");
                return result;
            }
            
            if (!manager.GetScriptAssistant()) {
                result.Fail("GetScriptAssistant() returned null");
                return result;
            }
            
            // Test capability check
            bool hasScriptGen = manager.HasCapability(AIIntegrationManager::AICapability::SCRIPT_GENERATION);
            bool hasScriptDebug = manager.HasCapability(AIIntegrationManager::AICapability::SCRIPT_DEBUGGING);
            
            // Note: Just checking API works, not actual capabilities
            std::cout << "  - Script Generation capability: " << (hasScriptGen ? "Yes" : "No") << std::endl;
            std::cout << "  - Script Debugging capability: " << (hasScriptDebug ? "Yes" : "No") << std::endl;
            
            result.Pass();
        } catch (const std::exception& e) {
            result.Fail(std::string("Exception: ") + e.what());
        }
        
        return result;
    }
    
    // Test AIIntegrationManager script generation
    TestResult TestIntegratedScriptGeneration() {
        TestResult result("Integrated Script Generation");
        
        try {
            // Get shared instance
            auto& manager = AIIntegrationManager::GetSharedInstance();
            
            // Ensure initialized
            if (!manager.IsInitialized()) {
                result.Skip("AIIntegrationManager not initialized");
                return result;
            }
            
            // Setup promise and future for callback
            std::promise<std::string> promise;
            auto future = promise.get_future();
            
            // Generate script
            manager.GenerateScript(
                "Create a simple ESP script",
                "fps game",
                [&promise](const std::string& script) {
                    promise.set_value(script);
                },
                false  // Prefer offline
            );
            
            // Wait for result with timeout
            auto futureStatus = future.wait_for(std::chrono::seconds(5));
            if (futureStatus != std::future_status::ready) {
                result.Fail("Timeout waiting for integrated script generation");
                return result;
            }
            
            // Get generated script
            std::string generatedScript = future.get();
            
            // Verify script is not empty
            if (generatedScript.empty()) {
                result.Fail("Generated script is empty");
                return result;
            }
            
            // Check for error message
            if (generatedScript.find("Error:") == 0) {
                result.Fail("Script generation error: " + generatedScript);
                return result;
            }
            
            result.Pass();
        } catch (const std::exception& e) {
            result.Fail(std::string("Exception: ") + e.what());
        }
        
        return result;
    }
}

// Main test runner
int main() {
    // Create test suite
    TestSuite aiTestSuite("AI Integration Tests");
    
    // Add tests
    aiTestSuite.AddTest(AITest::TestAIConfigInitialization);
    aiTestSuite.AddTest(AITest::TestHybridAISystemInitialization);
    aiTestSuite.AddTest(AITest::TestScriptGeneration);
    aiTestSuite.AddTest(AITest::TestScriptDebugging);
    aiTestSuite.AddTest(AITest::TestAIIntegrationManager);
    aiTestSuite.AddTest(AITest::TestIntegratedScriptGeneration);
    
    // Run tests
    auto results = aiTestSuite.RunAll();
    
    // Return success if all tests passed or skipped (no failures)
    for (const auto& result : results) {
        if (result.IsFailed()) {
            return 1; // Return non-zero on failure
        }
    }
    
    return 0;
}
