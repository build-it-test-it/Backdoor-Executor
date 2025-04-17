// integration_test.cpp - Comprehensive integration test for the Roblox Executor
#include <iostream>
#include <string>
#include <memory>
#include <thread>
#include <chrono>

// Include all main components
#include "../ios/ExecutionEngine.h"
#include "../ios/ScriptManager.h"
#include "../ios/UIController.h"
#include "../ios/JailbreakBypass.h"
#include "../ios/PatternScanner.h"
#include "../anti_detection/obfuscator.hpp"
#include "../logging.hpp"
#include "../memory/mem.hpp"
#include "../dobby_wrapper.cpp"

using namespace Logging;

// Test scripts
const std::string TEST_SCRIPT_SIMPLE = R"(
-- Simple test script
print("Hello from test script")
local test = 42
print("Test value: " .. test)
)";

const std::string TEST_SCRIPT_COMPLEX = R"(
-- More complex test script with functions
local function generateSequence(n)
    local result = {}
    for i = 1, n do
        table.insert(result, i * 2)
    end
    return result
end

local sequence = generateSequence(5)
print("Generated sequence:")
for i, v in ipairs(sequence) do
    print("  " .. i .. ": " .. v)
end

-- Test table manipulation
local players = {
    {name = "Player1", score = 100},
    {name = "Player2", score = 200},
    {name = "Player3", score = 150}
}

-- Sort players by score
table.sort(players, function(a, b) return a.score > b.score end)

print("Players by score:")
for i, player in ipairs(players) do
    print("  " .. player.name .. ": " .. player.score)
end
)";

// Test function for script execution
bool ExecuteScript(const std::string& script) {
    LogInfo("TestExecutor", "Executing script: " + script.substr(0, 50) + "...");
    
    // In a real implementation, this would actually execute the script
    // For testing, we'll just simulate execution
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    
    LogInfo("TestExecutor", "Script executed successfully");
    return true;
}

// Test function for saving scripts
bool SaveScript(const iOS::UIController::ScriptInfo& scriptInfo) {
    LogInfo("TestExecutor", "Saving script: " + scriptInfo.m_name);
    
    // In a real implementation, this would save to storage
    // For testing, we'll just log it
    LogInfo("TestExecutor", "Script saved successfully");
    return true;
}

// Test function for loading scripts
std::vector<iOS::UIController::ScriptInfo> LoadScripts() {
    LogInfo("TestExecutor", "Loading scripts");
    
    // Create a few test scripts
    std::vector<iOS::UIController::ScriptInfo> scripts;
    scripts.push_back({"TestScript1", TEST_SCRIPT_SIMPLE, 1631234567});
    scripts.push_back({"TestScript2", TEST_SCRIPT_COMPLEX, 1631234568});
    scripts.push_back({"TestScript3", "print('Simple test script')", 1631234569});
    
    LogInfo("TestExecutor", "Loaded " + std::to_string(scripts.size()) + " scripts");
    return scripts;
}

// Test all major components working together
void TestIntegration() {
    LogInfo("TestIntegration", "Starting integration test");
    
    try {
        // Step 1: Initialize logging
        LogInfo("TestIntegration", "Initializing logging system");
        Logging::Logger::InitializeWithFileLogging();
        LogInfo("TestIntegration", "Logging system initialized");
        
        // Step 2: Initialize JailbreakBypass
        LogInfo("TestIntegration", "Initializing JailbreakBypass");
        bool jbResult = iOS::JailbreakBypass::Initialize();
        LogInfo("TestIntegration", "JailbreakBypass initialized: " + std::string(jbResult ? "success" : "failure"));
        
        if (jbResult) {
            // Add some test redirects
            iOS::JailbreakBypass::AddFileRedirect("/test/path", "");
            iOS::JailbreakBypass::AddFileRedirect("/another/test", "/safe/path");
        }
        
        // Step 3: Create and initialize ScriptManager
        LogInfo("TestIntegration", "Creating ScriptManager");
        auto scriptManager = std::make_shared<iOS::ScriptManager>(true, 10, "TestScripts");
        bool smResult = scriptManager->Initialize();
        LogInfo("TestIntegration", "ScriptManager initialized: " + std::string(smResult ? "success" : "failure"));
        
        // Step 4: Create and initialize ExecutionEngine
        LogInfo("TestIntegration", "Creating ExecutionEngine");
        auto executionEngine = std::make_shared<iOS::ExecutionEngine>(scriptManager);
        bool eeResult = executionEngine->Initialize();
        LogInfo("TestIntegration", "ExecutionEngine initialized: " + std::string(eeResult ? "success" : "failure"));
        
        // Set execution context
        iOS::ExecutionEngine::ExecutionContext context;
        context.m_isJailbroken = jbResult;
        context.m_enableObfuscation = true;
        context.m_enableAntiDetection = true;
        executionEngine->SetDefaultContext(context);
        
        // Step 5: Create and initialize UIController
        LogInfo("TestIntegration", "Creating UIController");
        auto uiController = std::make_unique<iOS::UIController>();
        bool uiResult = uiController->Initialize();
        LogInfo("TestIntegration", "UIController initialized: " + std::string(uiResult ? "success" : "failure"));
        
        // Set up callbacks
        uiController->SetExecuteCallback(ExecuteScript);
        uiController->SetSaveScriptCallback(SaveScript);
        uiController->SetLoadScriptsCallback(LoadScripts);
        
        // Step 6: Test script obfuscation
        LogInfo("TestIntegration", "Testing script obfuscation");
        
        std::string obfuscatedSimple = AntiDetection::Obfuscator::ObfuscateScript(TEST_SCRIPT_SIMPLE, 3);
        LogInfo("TestIntegration", "Obfuscated simple script (level 3): \n" + obfuscatedSimple.substr(0, 200) + "...");
        
        std::string obfuscatedComplex = AntiDetection::Obfuscator::ObfuscateScript(TEST_SCRIPT_COMPLEX, 5);
        LogInfo("TestIntegration", "Obfuscated complex script (level 5): \n" + obfuscatedComplex.substr(0, 200) + "...");
        
        // Step 7: Test memory scanning
        LogInfo("TestIntegration", "Testing pattern scanning");
        
        // Pattern scanning generally needs actual memory to scan, so this is just a test setup
        // In a real implementation, you'd scan for actual patterns in memory
        const char* testPattern = "\x48\x89\x5C\x24\x08\x48\x89\x74\x24\x10";
        const char* testMask = "xxxxxxxxxx";
        
        iOS::PatternScanner::ScanResult result = iOS::PatternScanner::ScanForPattern(
            testPattern, 
            testMask, 
            nullptr, // Let it find its own base address
            nullptr  // Let it calculate its own end address
        );
        
        if (result.address) {
            LogInfo("TestIntegration", "Found pattern at address: " + std::to_string(result.address));
        } else {
            LogInfo("TestIntegration", "Pattern not found (expected in test environment)");
        }
        
        // Step 8: Test script execution through ExecutionEngine
        LogInfo("TestIntegration", "Testing script execution through ExecutionEngine");
        
        // Prepare a test script
        std::string testScript = obfuscatedSimple; // Use the obfuscated version
        
        // Execute the script
        iOS::ExecutionEngine::ExecutionResult execResult = executionEngine->Execute(testScript, context);
        
        LogInfo("TestIntegration", "Execution result: " + 
                std::string(execResult.m_success ? "success" : "failure") + 
                ", Time: " + std::to_string(execResult.m_executionTime) + "ms");
        
        if (!execResult.m_error.empty()) {
            LogWarning("TestIntegration", "Execution error: " + execResult.m_error);
        }
        
        if (!execResult.m_output.empty()) {
            LogInfo("TestIntegration", "Execution output: " + execResult.m_output);
        }
        
        // Step 9: Test UI interaction
        LogInfo("TestIntegration", "Testing UI interaction");
        
        // Set some script content
        uiController->SetScriptContent(TEST_SCRIPT_COMPLEX);
        
        // Switch tabs
        uiController->SwitchTab(iOS::UIController::TabType::Scripts);
        uiController->SwitchTab(iOS::UIController::TabType::Console);
        uiController->SwitchTab(iOS::UIController::TabType::Editor);
        
        // Execute script via UI
        bool uiExecResult = uiController->ExecuteCurrentScript();
        LogInfo("TestIntegration", "UI execute result: " + std::string(uiExecResult ? "success" : "failure"));
        
        // Save script via UI
        bool uiSaveResult = uiController->SaveCurrentScript("TestSave");
        LogInfo("TestIntegration", "UI save result: " + std::string(uiSaveResult ? "success" : "failure"));
        
        // Step 10: Test JailbreakBypass statistics
        LogInfo("TestIntegration", "JailbreakBypass statistics");
        iOS::JailbreakBypass::PrintStatistics();
        
        // Final report
        LogInfo("TestIntegration", "Integration test completed successfully");
        
    } catch (const std::exception& e) {
        LogError("TestIntegration", "Exception during integration test: " + std::string(e.what()));
    }
}

// Entry point for integration tests
int main() {
    try {
        // Initialize console logging
        Logging::Logger::GetInstance().SetMinLevel(Logging::LogLevel::DEBUG);
        
        // Run integration test
        TestIntegration();
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Critical error: " << e.what() << std::endl;
        return 1;
    }
}
