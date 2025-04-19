#include "../../ios_compat.h"
#include "OfflineAISystem.h"
#include "local_models/LocalModelBase.h"
#include "local_models/ScriptGenerationModel.h"
#include "vulnerability_detection/VulnerabilityDetector.h"
#include <iostream>
#include <sstream>
#include <thread>
#include <regex>
#include <set>

namespace iOS {
namespace AIFeatures {

// Constructor
OfflineAISystem::OfflineAISystem()
    : m_initialized(false),
      m_modelsLoaded(false),
      m_isInLowMemoryMode(false),
      m_scriptAssistantModel(nullptr),
      m_scriptGeneratorModel(nullptr),
      m_debugAnalyzerModel(nullptr),
      m_patternRecognitionModel(nullptr),
      m_totalMemoryUsage(0),
      m_maxMemoryAllowed(200 * 1024 * 1024), // 200MB default
      m_responseCallback(nullptr) {
}

// Destructor
OfflineAISystem::~OfflineAISystem() {
    // Clean up resources
    for (const auto& pair : m_modelCache) {
        // Nothing to do here with our locally trained models
    }
}

// Initialize the AI system
bool OfflineAISystem::Initialize(const std::string& modelPath, std::function<void(float)> progressCallback) {
    if (m_initialized) {
        return true;
    }
    
    try {
        m_modelPath = modelPath;
        
        // Create models directory if it doesn't exist
        NSString* dirPath = [NSString stringWithUTF8String:modelPath.c_str()];
        NSFileManager* fileManager = [NSFileManager defaultManager];
        
        if (![fileManager fileExistsAtPath:dirPath]) {
            NSError* error = nil;
            BOOL success = [fileManager createDirectoryAtPath:dirPath
                                  withIntermediateDirectories:YES
                                                   attributes:nil
                                                        error:&error];
            if (!success) {
                std::cerr << "OfflineAISystem: Failed to create models directory: " 
                         << [[error localizedDescription] UTF8String] << std::endl;
                return false;
            }
        }
        
        // Initialize local models
        if (progressCallback) progressCallback(0.1f);
        
        // Initialize Script Generator model
        auto scriptGenerator = std::make_shared<LocalModels::ScriptGenerationModel>();
        bool scriptGenInitialized = scriptGenerator->Initialize(modelPath + "/script_generator");
        
        if (scriptGenInitialized) {
            m_scriptGeneratorModel = scriptGenerator.get();
            m_modelCache["script_generator"] = scriptGenerator.get();
            m_loadedModelNames.push_back("script_generator");
        } else {
            std::cerr << "OfflineAISystem: Failed to initialize script generator model" << std::endl;
        }
        
        if (progressCallback) progressCallback(0.3f);
        
        // Initialize Vulnerability Detector
        auto vulnerabilityDetector = std::make_shared<VulnerabilityDetection::VulnerabilityDetector>();
        bool vulnerabilityInitialized = vulnerabilityDetector->Initialize(modelPath + "/vulnerability_detector");
        
        if (vulnerabilityInitialized) {
            m_patternRecognitionModel = vulnerabilityDetector.get();
            m_modelCache["vulnerability_detector"] = vulnerabilityDetector.get();
            m_loadedModelNames.push_back("vulnerability_detector");
        } else {
            std::cerr << "OfflineAISystem: Failed to initialize vulnerability detector" << std::endl;
        }
        
        if (progressCallback) progressCallback(0.5f);
        
        // Initialize Script Debugging model
        // In this implementation, we'll reuse the script generator for debugging
        if (scriptGenInitialized) {
            m_debugAnalyzerModel = scriptGenerator.get();
        }
        
        if (progressCallback) progressCallback(0.7f);
        
        // Load script templates
        LoadScriptTemplates();
        
        if (progressCallback) progressCallback(0.9f);
        
        m_initialized = true;
        m_modelsLoaded = true;
        
        if (progressCallback) progressCallback(1.0f);
        
        std::cout << "OfflineAISystem: Successfully initialized" << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "OfflineAISystem: Exception during initialization: " << e.what() << std::endl;
        return false;
    }
}

// Process a request
void OfflineAISystem::ProcessRequest(const AIRequest& request, ResponseCallback callback) {
    if (!callback) {
        return;
    }
    
    if (!m_initialized) {
        AIResponse response;
        response.m_success = false;
        response.m_errorMessage = "AI system not initialized";
        callback(response);
        return;
    }
    
    // Process request in background thread
    std::thread([this, request, callback]() {
        AIResponse response = ProcessRequestSync(request);
        callback(response);
    }).detach();
}

// Process a request synchronously
OfflineAISystem::AIResponse OfflineAISystem::ProcessRequestSync(const AIRequest& request) {
    AIResponse response;
    
    // Check if initialized
    if (!m_initialized) {
        response.m_success = false;
        response.m_errorMessage = "AI system not initialized";
        return response;
    }
    
    // Start timing
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Process request based on type
    if (request.m_requestType == "script_generation") {
        response = ProcessScriptGeneration(request);
    } else if (request.m_requestType == "debug") {
        response = ProcessScriptDebugging(request);
    } else {
        // General query
        response = ProcessGeneralQuery(request);
    }
    
    // Set processing time
    auto endTime = std::chrono::high_resolution_clock::now();
    response.m_processingTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        endTime - startTime).count();
    
    // Add to request history
    m_requestHistory.push_back(request);
    
    // Add to response history
    m_responseHistory.push_back(response);
    
    // Trim history if needed
    if (m_requestHistory.size() > 100) {
        m_requestHistory.erase(m_requestHistory.begin());
        m_responseHistory.erase(m_responseHistory.begin());
    }
    
    return response;
}

// Generate a script
void OfflineAISystem::GenerateScript(const std::string& description, const std::string& context, 
                               std::function<void(const std::string&)> callback) {
    if (!callback) {
        return;
    }
    
    // Create request
    AIRequest request(description, context, "script_generation");
    
    // Process request
    ProcessRequest(request, [callback](const AIResponse& response) {
        if (response.m_success) {
            callback(response.m_scriptCode.empty() ? response.m_content : response.m_scriptCode);
        } else {
            callback("Error: " + response.m_errorMessage);
        }
    });
}

// Debug a script
void OfflineAISystem::DebugScript(const std::string& script, 
                            std::function<void(const std::string&)> callback) {
    if (!callback) {
        return;
    }
    
    // Create request
    AIRequest request("Debug this script", script, "debug");
    
    // Process request
    ProcessRequest(request, [callback](const AIResponse& response) {
        if (response.m_success) {
            callback(response.m_content);
        } else {
            callback("Error: " + response.m_errorMessage);
        }
    });
}

// Process a general query
OfflineAISystem::AIResponse OfflineAISystem::ProcessGeneralQuery(const AIRequest& request) {
    AIResponse response;
    
    try {
        // Try to use the GeneralAssistantModel if available
        auto aiSystemInitializer = ::iOS::AIFeatures::AISystemInitializer::GetInstance();
        if (aiSystemInitializer) {
            auto generalAssistantModel = aiSystemInitializer->GetGeneralAssistantModel();
            if (generalAssistantModel && generalAssistantModel->IsInitialized()) {
                // Use the GeneralAssistantModel to process the query
                return generalAssistantModel->ProcessQuery(request);
            }
        }
        
        // Fall back to rule-based approach if model not available
        std::string query = request.m_query;
        std::transform(query.begin(), query.end(), query.begin(), 
                      [](unsigned char c) { return std::tolower(c); });
        
        std::stringstream output;
        
        // Handle script generation requests
        if (query.find("generat") != std::string::npos && 
            (query.find("script") != std::string::npos || query.find("code") != std::string::npos)) {
            
            output << "To generate a script, please provide a description of what you want the script to do.\n\n";
            output << "For example:\n";
            output << "- Generate a script for ESP\n";
            output << "- Create a speed hack script\n";
            output << "- Make an aimbot script\n";
            
            response.m_success = true;
            response.m_content = output.str();
            
            response.m_suggestions.push_back("Generate ESP script");
            response.m_suggestions.push_back("Generate speed hack");
            response.m_suggestions.push_back("Generate aimbot");
            
            return response;
        }
        
        // Handle debug requests
        if (query.find("debug") != std::string::npos || 
            query.find("fix") != std::string::npos || 
            query.find("error") != std::string::npos) {
            
            output << "To debug a script, please provide the script code along with your question.\n\n";
            output << "For example:\n";
            output << "- Debug this script: [paste your script here]\n";
            output << "- Fix errors in: [paste your script here]\n";
            
            response.m_success = true;
            response.m_content = output.str();
            
            return response;
        }
        
        // Handle help requests
        if (query.find("help") != std::string::npos || 
            query.find("how to") != std::string::npos || 
            query.find("explain") != std::string::npos) {
            
            output << "I'm here to help you with Lua scripting for Roblox games. Here are some things I can do:\n\n";
            output << "- Generate scripts based on your description\n";
            output << "- Debug existing scripts\n";
            output << "- Explain how to achieve specific effects or behaviors\n";
            output << "- Answer questions about Lua programming\n";
            output << "- Provide tips and best practices\n\n";
            
            output << "What would you like help with today?";
            
            response.m_success = true;
            response.m_content = output.str();
            
            response.m_suggestions.push_back("Generate a script");
            response.m_suggestions.push_back("Debug a script");
            response.m_suggestions.push_back("Explain Lua functions");
            
            return response;
        }
        
        // Handle script execution questions
        if (query.find("execute") != std::string::npos || 
            query.find("run") != std::string::npos) {
            
            output << "To execute a script, you can:\n\n";
            output << "1. Press the Execute button in the script editor\n";
            output << "2. Use the context menu on a saved script and select Execute\n";
            output << "3. Create a hotkey for quick execution\n\n";
            
            output << "Would you like to execute a specific script?";
            
            response.m_success = true;
            response.m_content = output.str();
            
            return response;
        }
        
        // Handle vulnerability scan requests
        if (query.find("vulnerabilit") != std::string::npos || 
            query.find("scan") != std::string::npos || 
            query.find("exploit") != std::string::npos || 
            query.find("backdoor") != std::string::npos) {
            
            output << "I can scan for vulnerabilities in Roblox games. To start a scan:\n\n";
            output << "1. Join the game you want to scan\n";
            output << "2. Click on 'Scan for Vulnerabilities' in the tools menu\n";
            output << "3. Wait for the scan to complete\n\n";
            
            output << "Would you like me to scan the current game for vulnerabilities?";
            
            response.m_success = true;
            response.m_content = output.str();
            
            response.m_suggestions.push_back("Scan current game");
            response.m_suggestions.push_back("View vulnerability types");
            response.m_suggestions.push_back("How to exploit vulnerabilities");
            
            return response;
        }
        
        // Default response for other queries
        output << "I'm not sure how to respond to that question. Here are some things I can help with:\n\n";
        output << "- Generate scripts for various purposes\n";
        output << "- Debug existing scripts\n";
        output << "- Explain Lua programming concepts\n";
        output << "- Scan games for vulnerabilities\n";
        output << "- Provide help and tutorials\n\n";
        
        output << "Could you rephrase your question or select one of these topics?";
        
        response.m_success = true;
        response.m_content = output.str();
        
        response.m_suggestions.push_back("Generate a script");
        response.m_suggestions.push_back("Debug a script");
        response.m_suggestions.push_back("Scan for vulnerabilities");
        
        return response;
    }
    catch (const std::exception& e) {
        response.m_success = false;
        response.m_errorMessage = std::string("Error processing query: ") + e.what();
    }
    
    return response;
}

// ... rest of the existing code ...

} // namespace AIFeatures
} // namespace iOS
