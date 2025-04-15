#define CI_BUILD
#include "../ios_compat.h"
#include "AISystemInitializer.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <algorithm>
#include <thread>
#include <regex>

namespace iOS {
namespace AIFeatures {

// Initialize static members
std::unique_ptr<AISystemInitializer> AISystemInitializer::s_instance = nullptr;
std::mutex AISystemInitializer::s_instanceMutex;

// Constructor
AISystemInitializer::AISystemInitializer()
    : m_initState(InitState::NotStarted),
      m_trainingThreadRunning(false),
      m_currentFallbackStrategy(FallbackStrategy::HybridApproach),
      m_vulnDetectionCount(0),
      m_scriptGenCount(0),
      m_fallbackUsageCount(0) {
}

// Destructor
AISystemInitializer::~AISystemInitializer() {
    // Stop training thread
    m_trainingThreadRunning = false;
    if (m_trainingThread.joinable()) {
        m_trainingThread.join();
    }
    
    // Save model states
    if (m_vulnDetectionModel && m_vulnDetectionModel->IsInitialized()) {
        m_vulnDetectionModel->SaveModel();
    }
    
    if (m_scriptGenModel && m_scriptGenModel->IsInitialized()) {
        m_scriptGenModel->SaveModel();
    }
    
    // Save self-modifying system state
    if (m_selfModifyingSystem && m_selfModifyingSystem->IsInitialized()) {
        m_selfModifyingSystem->SaveState();
    }
}

// Get singleton instance
AISystemInitializer& AISystemInitializer::GetInstance() {
    std::lock_guard<std::mutex> lock(s_instanceMutex);
    if (!s_instance) {
        s_instance = std::unique_ptr<AISystemInitializer>(new AISystemInitializer());
    }
    return *s_instance;
}

// Initialize AI system
bool AISystemInitializer::Initialize(const std::string& dataRootPath, std::shared_ptr<AIConfig> config) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // Check if already initialized
    if (m_initState == InitState::Completed || m_initState == InitState::InProgress) {
        return true;
    }
    
    std::cout << "AISystemInitializer: Starting initialization..." << std::endl;
    
    // Set initialization state
    m_initState = InitState::InProgress;
    
    // Store config
    m_config = config;
    
    // Set data root path
    m_dataRootPath = dataRootPath;
    
    // Initialize data paths
    if (!InitializeDataPaths()) {
        std::cerr << "AISystemInitializer: Failed to initialize data paths" << std::endl;
        m_initState = InitState::Failed;
        return false;
    }
    
    // Initialize fallback systems first (for immediate use)
    if (!InitializeFallbackSystems()) {
        std::cerr << "AISystemInitializer: Failed to initialize fallback systems" << std::endl;
        m_initState = InitState::Failed;
        return false;
    }
    
    // Initialize self-modifying system
    if (!InitializeSelfModifyingSystem()) {
        std::cerr << "AISystemInitializer: Failed to initialize self-modifying system" << std::endl;
        m_initState = InitState::Failed;
        return false;
    }
    
    // Initialize models (can be asynchronous)
    if (!InitializeModels()) {
        std::cerr << "AISystemInitializer: Failed to initialize models" << std::endl;
        m_initState = InitState::Failed;
        return false;
    }
    
    // Start training thread
    m_trainingThreadRunning = true;
    m_trainingThread = std::thread(&AISystemInitializer::TrainingThreadFunc, this);
    
    // Request initial training with medium priority
    RequestTraining("VulnerabilityDetectionModel", TrainingPriority::Medium);
    RequestTraining("ScriptGenerationModel", TrainingPriority::Medium);
    
    // Initialization succeeded or is in progress
    std::cout << "AISystemInitializer: Initialization completed successfully" << std::endl;
    m_initState = InitState::Completed;
    
    return true;
}

// Initialize data paths
bool AISystemInitializer::InitializeDataPaths() {
    // Define paths
    m_modelDataPath = m_dataRootPath + "/models";
    m_trainingDataPath = m_dataRootPath + "/training_data";
    m_cacheDataPath = m_dataRootPath + "/cache";
    
    // Create directories
    NSFileManager* fileManager = [NSFileManager defaultManager];
    
    for (const auto& path : {m_dataRootPath, m_modelDataPath, m_trainingDataPath, m_cacheDataPath}) {
        NSString* nsPath = [NSString stringWithUTF8String:path.c_str()];
        
        if (![fileManager fileExistsAtPath:nsPath]) {
            NSError* error = nil;
            if (![fileManager createDirectoryAtPath:nsPath
                        withIntermediateDirectories:YES
                                         attributes:nil
                                              error:&error]) {
                std::cerr << "AISystemInitializer: Failed to create directory " << path
                          << ": " << [error.localizedDescription UTF8String] << std::endl;
                return false;
            }
        }
    }
    
    std::cout << "AISystemInitializer: Data paths initialized" << std::endl;
    return true;
}

// Initialize models
bool AISystemInitializer::InitializeModels() {
    // Create vulnerability detection model
    m_vulnDetectionModel = std::make_shared<LocalModels::VulnerabilityDetectionModel>();
    
    // Set model data path
    std::string vulnModelPath = m_modelDataPath + "/vulnerability_detection";
    
    // Initialize model
    if (!m_vulnDetectionModel->SetModelPath(vulnModelPath)) {
        std::cerr << "AISystemInitializer: Failed to set vulnerability detection model path" << std::endl;
        return false;
    }
    
    // Lazy initialization - don't load full model yet
    UpdateModelStatus("VulnerabilityDetectionModel", InitState::NotStarted, 0.0f, 0.0f);
    
    // Create script generation model
    m_scriptGenModel = std::make_shared<LocalModels::ScriptGenerationModel>();
    
    // Set model data path
    std::string scriptGenModelPath = m_modelDataPath + "/script_generation";
    
    // Initialize model
    if (!m_scriptGenModel->SetModelPath(scriptGenModelPath)) {
        std::cerr << "AISystemInitializer: Failed to set script generation model path" << std::endl;
        return false;
    }
    
    // Lazy initialization - don't load full model yet
    UpdateModelStatus("ScriptGenerationModel", InitState::NotStarted, 0.0f, 0.0f);
    
    std::cout << "AISystemInitializer: Models initialized" << std::endl;
    return true;
}

// Initialize self-modifying system
bool AISystemInitializer::InitializeSelfModifyingSystem() {
    // Create self-modifying code system
    m_selfModifyingSystem = std::make_shared<SelfModifyingCodeSystem>();
    
    // Set data path
    std::string selfModPath = m_dataRootPath + "/self_modifying";
    
    // Initialize system
    if (!m_selfModifyingSystem->Initialize(selfModPath)) {
        std::cerr << "AISystemInitializer: Failed to initialize self-modifying system" << std::endl;
        return false;
    }
    
    std::cout << "AISystemInitializer: Self-modifying system initialized" << std::endl;
    return true;
}

// Initialize fallback systems
bool AISystemInitializer::InitializeFallbackSystems() {
    // Initialize fallback patterns for vulnerability detection
    m_fallbackPatterns["loadstring"] = "Critical:Script injection vulnerability:Using loadstring to execute dynamic code";
    m_fallbackPatterns["getfenv"] = "High:Environment access vulnerability:Using getfenv to access environment";
    m_fallbackPatterns["setfenv"] = "High:Environment modification vulnerability:Using setfenv to modify environment";
    m_fallbackPatterns["HttpService"] = "Medium:HTTP request vulnerability:Making HTTP requests to external services";
    m_fallbackPatterns["FireServer"] = "Medium:Remote event vulnerability:Sending data to server via RemoteEvent";
    m_fallbackPatterns["InvokeServer"] = "Medium:Remote function vulnerability:Sending data to server via RemoteFunction";
    m_fallbackPatterns["_G"] = "Low:Global variable vulnerability:Accessing global variables";
    
    // Load self-modifying system segments for additional patterns
    if (m_selfModifyingSystem && m_selfModifyingSystem->IsInitialized()) {
        auto vulnPatterns = m_selfModifyingSystem->GetSegment("VulnerabilityPatterns");
        if (!vulnPatterns.m_name.empty() && !vulnPatterns.m_originalCode.empty()) {
            // Parse JSON patterns and add to fallback patterns
            // In a real implementation, this would use a proper JSON parser
            std::string patternsJson = vulnPatterns.m_originalCode;
            std::regex patternRegex("\"name\"\\s*:\\s*\"([^\"]+)\".*?\"regex\"\\s*:\\s*\"([^\"]+)\".*?\"severity\"\\s*:\\s*\"([^\"]+)\".*?\"description\"\\s*:\\s*\"([^\"]+)\"");
            
            std::sregex_iterator it(patternsJson.begin(), patternsJson.end(), patternRegex);
            std::sregex_iterator end;
            
            while (it != end) {
                std::smatch match = *it;
                if (match.size() >= 5) {
                    std::string name = match[1].str();
                    std::string regex = match[2].str();
                    std::string severity = match[3].str();
                    std::string description = match[4].str();
                    
                    m_fallbackPatterns[regex] = severity + ":" + name + ":" + description;
                }
                ++it;
            }
        }
        
        // Load advanced patterns as well
        auto advPatterns = m_selfModifyingSystem->GetSegment("AdvancedVulnerabilityPatterns");
        if (!advPatterns.m_name.empty() && !advPatterns.m_originalCode.empty()) {
            std::string patternsJson = advPatterns.m_originalCode;
            std::regex patternRegex("\"name\"\\s*:\\s*\"([^\"]+)\".*?\"regex\"\\s*:\\s*\"([^\"]+)\".*?\"severity\"\\s*:\\s*\"([^\"]+)\".*?\"description\"\\s*:\\s*\"([^\"]+)\"");
            
            std::sregex_iterator it(patternsJson.begin(), patternsJson.end(), patternRegex);
            std::sregex_iterator end;
            
            while (it != end) {
                std::smatch match = *it;
                if (match.size() >= 5) {
                    std::string name = match[1].str();
                    std::string regex = match[2].str();
                    std::string severity = match[3].str();
                    std::string description = match[4].str();
                    
                    m_fallbackPatterns[regex] = severity + ":" + name + ":" + description;
                }
                ++it;
            }
        }
    }
    
    std::cout << "AISystemInitializer: Fallback systems initialized with " 
              << m_fallbackPatterns.size() << " vulnerability patterns" << std::endl;
    
    return true;
}

// Training thread function
void AISystemInitializer::TrainingThreadFunc() {
    std::cout << "AISystemInitializer: Training thread started" << std::endl;
    
    while (m_trainingThreadRunning) {
        TrainingRequest request;
        bool hasRequest = false;
        
        // Get next training request
        {
            std::lock_guard<std::mutex> lock(m_trainingQueueMutex);
            if (!m_trainingQueue.empty()) {
                // Sort queue by priority
                std::sort(m_trainingQueue.begin(), m_trainingQueue.end(),
                         [](const TrainingRequest& a, const TrainingRequest& b) {
                             return static_cast<int>(a.m_priority) > static_cast<int>(b.m_priority);
                         });
                
                // Get highest priority request
                request = m_trainingQueue.front();
                m_trainingQueue.erase(m_trainingQueue.begin());
                hasRequest = true;
            }
        }
        
        // Process request
        if (hasRequest) {
            // Train model
            TrainModel(request);
        } else {
            // No requests, sleep for a bit
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    
    std::cout << "AISystemInitializer: Training thread stopped" << std::endl;
}

// Train a model
bool AISystemInitializer::TrainModel(const TrainingRequest& request) {
    std::cout << "AISystemInitializer: Training model " << request.m_modelName << std::endl;
    
    // Update model status
    UpdateModelStatus(request.m_modelName, InitState::InProgress, 0.0f, 0.0f);
    
    bool success = false;
    
    // Progress callback
    auto progressCallback = [this, &request](float progress, float accuracy) {
        UpdateModelStatus(request.m_modelName, InitState::InProgress, progress, accuracy);
        
        // Call user progress callback if provided
        if (request.m_progressCallback) {
            request.m_progressCallback(progress, accuracy);
        }
    };
    
    // Train the model
    if (request.m_modelName == "VulnerabilityDetectionModel") {
        if (m_vulnDetectionModel) {
            if (!m_vulnDetectionModel->IsInitialized() || request.m_forceRetrain) {
                // Initialize model if needed
                if (!m_vulnDetectionModel->IsInitialized()) {
                    m_vulnDetectionModel->Initialize();
                }
                
                // Train model
                success = m_vulnDetectionModel->Train(progressCallback);
            } else {
                // Model already trained
                success = true;
            }
        }
    } else if (request.m_modelName == "ScriptGenerationModel") {
        if (m_scriptGenModel) {
            if (!m_scriptGenModel->IsInitialized() || request.m_forceRetrain) {
                // Initialize model if needed
                if (!m_scriptGenModel->IsInitialized()) {
                    m_scriptGenModel->Initialize();
                }
                
                // Train model
                success = m_scriptGenModel->Train(progressCallback);
            } else {
                // Model already trained
                success = true;
            }
        }
    }
    
    // Update model status
    InitState state = success ? InitState::Completed : InitState::Failed;
    float accuracy = 0.0f;
    
    if (success) {
        if (request.m_modelName == "VulnerabilityDetectionModel" && m_vulnDetectionModel) {
            accuracy = m_vulnDetectionModel->GetAccuracy();
        } else if (request.m_modelName == "ScriptGenerationModel" && m_scriptGenModel) {
            accuracy = m_scriptGenModel->GetAccuracy();
        }
    }
    
    UpdateModelStatus(request.m_modelName, state, 1.0f, accuracy);
    
    // Log result
    std::cout << "AISystemInitializer: Training " << request.m_modelName 
              << (success ? " succeeded" : " failed")
              << " with accuracy " << accuracy << std::endl;
    
    return success;
}

// Update model status
void AISystemInitializer::UpdateModelStatus(
    const std::string& modelName, InitState state, float progress, float accuracy) {
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto& status = m_modelStatus[modelName];
    status.m_name = modelName;
    status.m_state = state;
    status.m_trainingProgress = progress;
    status.m_accuracy = accuracy;
    
    // Set timestamp
    auto now = std::chrono::system_clock::now();
    uint64_t timestamp = std::chrono::duration_cast<std::chrono::seconds>(
        now.time_since_epoch()).count();
    
    if (state == InitState::InProgress) {
        status.m_lastTrainingTime = timestamp;
    }
    
    // Set version
    if (modelName == "VulnerabilityDetectionModel" && m_vulnDetectionModel) {
        status.m_version = m_vulnDetectionModel->GetVersion();
    } else if (modelName == "ScriptGenerationModel" && m_scriptGenModel) {
        status.m_version = m_scriptGenModel->GetVersion();
    }
}

// Get initialization state
AISystemInitializer::InitState AISystemInitializer::GetInitState() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_initState;
}

// Get vulnerability detection model
std::shared_ptr<LocalModels::VulnerabilityDetectionModel> AISystemInitializer::GetVulnerabilityDetectionModel() {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // Initialize model if needed
    if (m_vulnDetectionModel && !m_vulnDetectionModel->IsInitialized()) {
        m_vulnDetectionModel->Initialize();
        
        // Request training
        RequestTraining("VulnerabilityDetectionModel", TrainingPriority::High);
    }
    
    return m_vulnDetectionModel;
}

// Enable ALL vulnerability types for comprehensive detection
bool AISystemInitializer::EnableAllVulnerabilityTypes() {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (!m_vulnDetectionModel) {
        std::cerr << "AISystemInitializer: VulnerabilityDetectionModel not initialized" << std::endl;
        return false;
    }
    
    try {
        // Configure for comprehensive detection
        m_vulnDetectionModel->ConfigureDetection(
            true,  // enableDataFlow
            true,  // enableSemantic
            true,  // enableZeroDay
            true,  // enableAllVulnTypes
            0.1f   // detectionThreshold (low to catch everything)
        );
        
        // Explicitly enable all vulnerability types
        m_vulnDetectionModel->EnableAllVulnerabilityTypes();
        
        std::cout << "AISystemInitializer: Successfully enabled ALL vulnerability types" << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "AISystemInitializer: Exception enabling all vulnerability types: " 
                 << e.what() << std::endl;
        return false;
    }
}

// Get script generation model
std::shared_ptr<LocalModels::ScriptGenerationModel> AISystemInitializer::GetScriptGenerationModel() {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // Initialize model if needed
    if (m_scriptGenModel && !m_scriptGenModel->IsInitialized()) {
        m_scriptGenModel->Initialize();
        
        // Request training
        RequestTraining("ScriptGenerationModel", TrainingPriority::High);
    }
    
    return m_scriptGenModel;
}

// Get self-modifying code system
std::shared_ptr<SelfModifyingCodeSystem> AISystemInitializer::GetSelfModifyingSystem() {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_selfModifyingSystem;
}

// Detect vulnerabilities in script
std::string AISystemInitializer::DetectVulnerabilities(
    const std::string& script, const std::string& gameType, bool isServerScript) {
    
    // Increment usage count
    m_vulnDetectionCount++;
    
    // Check if model is ready
    auto status = GetModelStatus("VulnerabilityDetectionModel");
    if (status.m_state == InitState::Completed && m_vulnDetectionModel && m_vulnDetectionModel->IsInitialized()) {
        // Use trained model
        auto model = m_vulnDetectionModel;
        
        try {
            // Update last used time
            auto now = std::chrono::system_clock::now();
            uint64_t timestamp = std::chrono::duration_cast<std::chrono::seconds>(
                now.time_since_epoch()).count();
            status.m_lastUsedTime = timestamp;
            
            // Detect vulnerabilities
            std::vector<LocalModels::VulnerabilityDetectionModel::Vulnerability> vulns = 
                model->AnalyzeCode(script, gameType, isServerScript);
            
            // Check if we have self-modifying system patterns to enhance detection
            if (m_selfModifyingSystem && m_selfModifyingSystem->IsInitialized()) {
                // Get advanced patterns
                auto advPatterns = m_selfModifyingSystem->GetSegment("AdvancedVulnerabilityPatterns");
                if (!advPatterns.m_name.empty() && !advPatterns.m_originalCode.empty()) {
                    // Enhanced scanning with advanced patterns would be implemented here
                    // This would involve parsing the JSON patterns and applying them
                }
            }
            
            // Convert to JSON
            NSMutableArray* vulnArray = [NSMutableArray array];
            
            for (const auto& vuln : vulns) {
                NSMutableDictionary* vulnDict = [NSMutableDictionary dictionary];
                
                [vulnDict setObject:[NSString stringWithUTF8String:m_vulnDetectionModel->GetVulnTypeString(vuln.m_type).c_str()] 
                            forKey:@"type"];
                
                NSString* sevStr = nil;
                switch (vuln.m_severity) {
                    case LocalModels::VulnerabilityDetectionModel::VulnSeverity::Critical:
                        sevStr = @"Critical";
                        break;
                    case LocalModels::VulnerabilityDetectionModel::VulnSeverity::High:
                        sevStr = @"High";
                        break;
                    case LocalModels::VulnerabilityDetectionModel::VulnSeverity::Medium:
                        sevStr = @"Medium";
                        break;
                    case LocalModels::VulnerabilityDetectionModel::VulnSeverity::Low:
                        sevStr = @"Low";
                        break;
                    default:
                        sevStr = @"Info";
                        break;
                }
                
                [vulnDict setObject:sevStr forKey:@"severity"];
                [vulnDict setObject:[NSString stringWithUTF8String:vuln.m_description.c_str()] 
                            forKey:@"description"];
                
                if (!vuln.m_affectedCode.empty()) {
                    [vulnDict setObject:[NSString stringWithUTF8String:vuln.m_affectedCode.c_str()] 
                                forKey:@"affectedCode"];
                }
                
                if (vuln.m_lineNumber > 0) {
                    [vulnDict setObject:@(vuln.m_lineNumber) forKey:@"lineNumber"];
                }
                
                [vulnDict setObject:[NSString stringWithUTF8String:vuln.m_mitigation.c_str()] 
                            forKey:@"mitigation"];
                
                [vulnArray addObject:vulnDict];
            }
            
            // Convert to JSON string
            NSError* error = nil;
            NSData* jsonData = [NSJSONSerialization dataWithJSONObject:vulnArray
                                                               options:NSJSONWritingPrettyPrinted
                                                                 error:&error];
            
            if (error || !jsonData) {
                return GetFallbackVulnerabilityDetectionResult(script);
            }
            
            NSString* jsonString = [[NSString alloc] initWithData:jsonData encoding:NSUTF8StringEncoding];
            return [jsonString UTF8String];
        } catch (const std::exception& e) {
            std::cerr << "AISystemInitializer: Exception during vulnerability detection: " 
                      << e.what() << std::endl;
            
            // Fall back to basic detection
            return GetFallbackVulnerabilityDetectionResult(script);
        }
    } else {
        // Use fallback detection
        m_fallbackUsageCount++;
        return GetFallbackVulnerabilityDetectionResult(script);
    }
}

// Generate script from description
std::string AISystemInitializer::GenerateScript(
    const std::string& description, const std::string& gameType, bool isServerScript) {
    
    // Increment usage count
    m_scriptGenCount++;
    
    // Check if model is ready
    auto status = GetModelStatus("ScriptGenerationModel");
    if (status.m_state == InitState::Completed && m_scriptGenModel && m_scriptGenModel->IsInitialized()) {
        // Use trained model
        auto model = m_scriptGenModel;
        
        try {
            // Update last used time
            auto now = std::chrono::system_clock::now();
            uint64_t timestamp = std::chrono::duration_cast<std::chrono::seconds>(
                now.time_since_epoch()).count();
            status.m_lastUsedTime = timestamp;
            
            // Generate script
            LocalModels::ScriptGenerationModel::GeneratedScript script = 
                model->GenerateScript(description, gameType + (isServerScript ? " server" : " client"));
            
            // Check if script was generated
            if (!script.m_code.empty()) {
                // Cache result
                m_cachedResults[description] = script.m_code;
                
                return script.m_code;
            } else {
                // Fall back to template based generation
                return GetFallbackScriptGenerationResult(description);
            }
        } catch (const std::exception& e) {
            std::cerr << "AISystemInitializer: Exception during script generation: " 
                      << e.what() << std::endl;
            
            // Fall back to template based generation
            return GetFallbackScriptGenerationResult(description);
        }
    } else {
        // Use fallback generation
        m_fallbackUsageCount++;
        return GetFallbackScriptGenerationResult(description);
    }
}

// Get fallback vulnerability detection result
std::string AISystemInitializer::GetFallbackVulnerabilityDetectionResult(const std::string& script) {
    std::vector<std::unordered_map<std::string, std::string>> results;
    
    // Split script into lines
    std::istringstream iss(script);
    std::string line;
    int lineNumber = 0;
    
    while (std::getline(iss, line)) {
        lineNumber++;
        
        // Check against fallback patterns
        for (const auto& pattern : m_fallbackPatterns) {
            try {
                std::regex re(pattern.first);
                if (std::regex_search(line, re)) {
                    // Parse pattern result
                    std::string patternResult = pattern.second;
                    std::vector<std::string> parts;
                    
                    // Split by colon
                    size_t pos = 0;
                    std::string token;
                    while ((pos = patternResult.find(':')) != std::string::npos) {
                        token = patternResult.substr(0, pos);
                        parts.push_back(token);
                        patternResult.erase(0, pos + 1);
                    }
                    parts.push_back(patternResult);
                    
                    // Create result
                    std::unordered_map<std::string, std::string> result;
                    
                    if (parts.size() >= 3) {
                        result["severity"] = parts[0];
                        result["description"] = parts[2];
                        
                        // Determine type based on pattern
                        if (pattern.first.find("loadstring") != std::string::npos || 
                            pattern.first.find("setfenv") != std::string::npos ||
                            pattern.first.find("getfenv") != std::string::npos) {
                            result["type"] = "ScriptInjection";
                        } else if (pattern.first.find("FireServer") != std::string::npos || 
                                 pattern.first.find("InvokeServer") != std::string::npos) {
                            result["type"] = "RemoteEvent";
                        } else if (pattern.first.find("HttpService") != std::string::npos) {
                            result["type"] = "InsecureHttpService";
                        } else if (pattern.first.find("_G") != std::string::npos) {
                            result["type"] = "AccessControl";
                        } else {
                            result["type"] = "Other";
                        }
                        
                        result["lineNumber"] = std::to_string(lineNumber);
                        result["affectedCode"] = line;
                        result["mitigation"] = "Review this code for potential security issues";
                        
                        results.push_back(result);
                    }
                }
            } catch (const std::exception& e) {
                // Ignore regex errors
            }
        }
    }
    
    // Convert to JSON
    NSMutableArray* vulnArray = [NSMutableArray array];
    
    for (const auto& result : results) {
        NSMutableDictionary* vulnDict = [NSMutableDictionary dictionary];
        
        for (const auto& pair : result) {
            NSString* key = [NSString stringWithUTF8String:pair.first.c_str()];
            NSString* value = [NSString stringWithUTF8String:pair.second.c_str()];
            
            if ([key isEqualToString:@"lineNumber"]) {
                [vulnDict setObject:@([value intValue]) forKey:key];
            } else {
                [vulnDict setObject:value forKey:key];
            }
        }
        
        [vulnArray addObject:vulnDict];
    }
    
    // Convert to JSON string
    NSError* error = nil;
    NSData* jsonData = [NSJSONSerialization dataWithJSONObject:vulnArray
                                                       options:NSJSONWritingPrettyPrinted
                                                         error:&error];
    
    if (error || !jsonData) {
        return "[]";
    }
    
    NSString* jsonString = [[NSString alloc] initWithData:jsonData encoding:NSUTF8StringEncoding];
    return [jsonString UTF8String];
}

// Get fallback script generation result
std::string AISystemInitializer::GetFallbackScriptGenerationResult(const std::string& description) {
    // Check if we have a cached result
    auto it = m_cachedResults.find(description);
    if (it != m_cachedResults.end()) {
        return it->second;
    }
    
    // Check if we have fallback model data
    if (m_selfModifyingSystem && m_selfModifyingSystem->IsInitialized()) {
        auto fallbackModel = m_selfModifyingSystem->GetSegment("FallbackModel");
        if (!fallbackModel.m_name.empty() && !fallbackModel.m_originalCode.empty()) {
            // Parse fallback model data (simple approach)
            std::string modelData = fallbackModel.m_originalCode;
            
            // Check if description contains keywords
            std::string lowerDesc = description;
            std::transform(lowerDesc.begin(), lowerDesc.end(), lowerDesc.begin(), ::tolower);
            
            // Check for speed/movement script
            if (lowerDesc.find("speed") != std::string::npos || 
                lowerDesc.find("move") != std::string::npos ||
                lowerDesc.find("walk") != std::string::npos ||
                lowerDesc.find("run") != std::string::npos) {
                
                // Simple pattern extraction
                std::regex codeRegex("\"BasicSpeed\".*?\"code\"\\s*:\\s*\"(.*?)\"\\s*\\}");
                std::smatch match;
                
                if (std::regex_search(modelData, match, codeRegex) && match.size() > 1) {
                    std::string code = match[1].str();
                    
                    // Unescape JSON string
                    code = std::regex_replace(code, std::regex("\\\\n"), "\n");
                    code = std::regex_replace(code, std::regex("\\\\\""), "\"");
                    code = std::regex_replace(code, std::regex("\\\\\\\\"), "\\");
                    
                    // Add description as comment
                    code = "-- " + description + "\n" + code;
                    
                    // Cache result
                    m_cachedResults[description] = code;
                    
                    return code;
                }
            }
            
            // Check for ESP/visual script
            else if (lowerDesc.find("esp") != std::string::npos || 
                   lowerDesc.find("visual") != std::string::npos ||
                   lowerDesc.find("see") != std::string::npos ||
                   lowerDesc.find("wall") != std::string::npos) {
                
                // Simple pattern extraction
                std::regex codeRegex("\"BasicESP\".*?\"code\"\\s*:\\s*\"(.*?)\"\\s*\\}");
                std::smatch match;
                
                if (std::regex_search(modelData, match, codeRegex) && match.size() > 1) {
                    std::string code = match[1].str();
                    
                    // Unescape JSON string
                    code = std::regex_replace(code, std::regex("\\\\n"), "\n");
                    code = std::regex_replace(code, std::regex("\\\\\""), "\"");
                    code = std::regex_replace(code, std::regex("\\\\\\\\"), "\\");
                    
                    // Add description as comment
                    code = "-- " + description + "\n" + code;
                    
                    // Cache result
                    m_cachedResults[description] = code;
                    
                    return code;
                }
            }
        }
    }
    
    // Default basic script
    std::string basicScript = "-- " + description + "\n\n"
        "local Players = game:GetService(\"Players\")\n"
        "local player = Players.LocalPlayer\n"
        "local character = player.Character or player.CharacterAdded:Wait()\n\n"
        "-- Basic functionality\n"
        "print(\"Script activated for: \" .. player.Name)\n";
    
    // Add description-specific functionality
    std::string lowerDesc = description;
    std::transform(lowerDesc.begin(), lowerDesc.end(), lowerDesc.begin(), ::tolower);
    
    if (lowerDesc.find("speed") != std::string::npos) {
        basicScript += "\n-- Modify speed\n"
            "local humanoid = character:WaitForChild(\"Humanoid\")\n"
            "humanoid.WalkSpeed = 50 -- Increased speed\n\n"
            "-- Handle respawn\n"
            "player.CharacterAdded:Connect(function(newCharacter)\n"
            "    local newHumanoid = newCharacter:WaitForChild(\"Humanoid\")\n"
            "    newHumanoid.WalkSpeed = 50\n"
            "end)\n";
    } else if (lowerDesc.find("jump") != std::string::npos) {
        basicScript += "\n-- Modify jump power\n"
            "local humanoid = character:WaitForChild(\"Humanoid\")\n"
            "humanoid.JumpPower = 100 -- Increased jump power\n\n"
            "-- Handle respawn\n"
            "player.CharacterAdded:Connect(function(newCharacter)\n"
            "    local newHumanoid = newCharacter:WaitForChild(\"Humanoid\")\n"
            "    newHumanoid.JumpPower = 100\n"
            "end)\n";
    }
    
    // Cache result
    m_cachedResults[description] = basicScript;
    
    return basicScript;
}

// Request model training
bool AISystemInitializer::RequestTraining(
    const std::string& modelName, TrainingPriority priority, 
    bool forceRetrain, std::function<void(float, float)> progressCallback) {
    
    // Create training request
    TrainingRequest request;
    request.m_modelName = modelName;
    request.m_priority = priority;
    request.m_forceRetrain = forceRetrain;
    request.m_progressCallback = progressCallback;
    
    // Add to queue
    {
        std::lock_guard<std::mutex> lock(m_trainingQueueMutex);
        m_trainingQueue.push_back(request);
    }
    
    return true;
}

// Get model status
AISystemInitializer::ModelStatus AISystemInitializer::GetModelStatus(const std::string& modelName) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_modelStatus.find(modelName);
    if (it != m_modelStatus.end()) {
        return it->second;
    }
    
    return ModelStatus();
}

// Get all model statuses
std::unordered_map<std::string, AISystemInitializer::ModelStatus> AISystemInitializer::GetAllModelStatuses() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_modelStatus;
}

// Set fallback strategy
void AISystemInitializer::SetFallbackStrategy(FallbackStrategy strategy) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_currentFallbackStrategy = strategy;
}

// Get fallback strategy
AISystemInitializer::FallbackStrategy AISystemInitializer::GetFallbackStrategy() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_currentFallbackStrategy;
}

// Force self-improvement cycle
bool AISystemInitializer::ForceSelfImprovement() {
    if (!m_selfModifyingSystem || !m_selfModifyingSystem->IsInitialized()) {
        return false;
    }
    
    // Generate optimization patches
    uint32_t patches = m_selfModifyingSystem->GenerateOptimizationPatches();
    
    // Apply available patches
    uint32_t applied = m_selfModifyingSystem->ApplyAvailablePatches();
    
    // Force model improvements
    if (m_vulnDetectionModel && m_vulnDetectionModel->IsInitialized()) {
        m_vulnDetectionModel->ForceSelfImprovement();
    }
    
    return patches > 0 || applied > 0;
}

// Add vulnerability training data
bool AISystemInitializer::AddVulnerabilityTrainingData(
    const std::string& script, const std::string& vulnerabilities) {
    
    if (!m_vulnDetectionModel) {
        return false;
    }
    
    // Initialize model if needed
    if (!m_vulnDetectionModel->IsInitialized()) {
        m_vulnDetectionModel->Initialize();
    }
    
    // Create training sample
    LocalModels::LocalModelBase::TrainingSample sample;
    sample.m_input = script;
    sample.m_output = vulnerabilities;
    sample.m_weight = 1.0f;
    
    // Add sample
    m_vulnDetectionModel->AddTrainingSample(sample);
    
    // Request training
    RequestTraining("VulnerabilityDetectionModel", TrainingPriority::Low);
    
    return true;
}

// Add script generation training data
bool AISystemInitializer::AddScriptGenerationTrainingData(
    const std::string& description, const std::string& script, float rating) {
    
    if (!m_scriptGenModel) {
        return false;
    }
    
    // Initialize model if needed
    if (!m_scriptGenModel->IsInitialized()) {
        m_scriptGenModel->Initialize();
    }
    
    // Create training sample
    LocalModels::LocalModelBase::TrainingSample sample;
    sample.m_input = description;
    sample.m_output = script;
    sample.m_weight = rating;
    
    // Add sample
    m_scriptGenModel->AddTrainingSample(sample);
    
    // Request training
    RequestTraining("ScriptGenerationModel", TrainingPriority::Low);
    
    return true;
}

// Provide vulnerability feedback
bool AISystemInitializer::ProvideVulnerabilityFeedback(
    const std::string& script, const std::string& detectionResult,
    const std::unordered_map<int, bool>& correctDetections) {
    
    if (!m_vulnDetectionModel || !m_vulnDetectionModel->IsInitialized()) {
        return false;
    }
    
    // Parse detection result
    NSData* jsonData = [NSData dataWithBytes:detectionResult.c_str() length:detectionResult.length()];
    NSError* error = nil;
    NSArray* vulnArray = [NSJSONSerialization JSONObjectWithData:jsonData
                                                         options:0
                                                           error:&error];
    
    if (error || !vulnArray || ![vulnArray isKindOfClass:[NSArray class]]) {
        return false;
    }
    
    // Convert to vulnerabilities
    std::vector<LocalModels::VulnerabilityDetectionModel::Vulnerability> vulnerabilities;
    
    for (NSUInteger i = 0; i < [vulnArray count]; i++) {
        NSDictionary* vulnDict = [vulnArray objectAtIndex:i];
        if (![vulnDict isKindOfClass:[NSDictionary class]]) {
            continue;
        }
        
        LocalModels::VulnerabilityDetectionModel::Vulnerability vuln;
        
        // Extract type
        NSString* typeStr = [vulnDict objectForKey:@"type"];
        if (typeStr) {
            if ([typeStr isEqualToString:@"ScriptInjection"]) {
                vuln.m_type = LocalModels::VulnerabilityDetectionModel::VulnType::ScriptInjection;
            } else if ([typeStr isEqualToString:@"RemoteEvent"]) {
                vuln.m_type = LocalModels::VulnerabilityDetectionModel::VulnType::RemoteEvent;
            } else if ([typeStr isEqualToString:@"RemoteFunction"]) {
                vuln.m_type = LocalModels::VulnerabilityDetectionModel::VulnType::RemoteFunction;
            } else if ([typeStr isEqualToString:@"InsecureHttpService"]) {
                vuln.m_type = LocalModels::VulnerabilityDetectionModel::VulnType::InsecureHttpService;
            } else if ([typeStr isEqualToString:@"UnsafeRequire"]) {
                vuln.m_type = LocalModels::VulnerabilityDetectionModel::VulnType::UnsafeRequire;
            } else if ([typeStr isEqualToString:@"TaintedInput"]) {
                vuln.m_type = LocalModels::VulnerabilityDetectionModel::VulnType::TaintedInput;
            } else if ([typeStr isEqualToString:@"AccessControl"]) {
                vuln.m_type = LocalModels::VulnerabilityDetectionModel::VulnType::AccessControl;
            } else if ([typeStr isEqualToString:@"LogicFlaw"]) {
                vuln.m_type = LocalModels::VulnerabilityDetectionModel::VulnType::LogicFlaw;
            } else if ([typeStr isEqualToString:@"DataStore"]) {
                vuln.m_type = LocalModels::VulnerabilityDetectionModel::VulnType::DataStore;
            } else {
                vuln.m_type = LocalModels::VulnerabilityDetectionModel::VulnType::Other;
            }
        }
        
        // Extract severity
        NSString* sevStr = [vulnDict objectForKey:@"severity"];
        if (sevStr) {
            if ([sevStr isEqualToString:@"Critical"]) {
                vuln.m_severity = LocalModels::VulnerabilityDetectionModel::VulnSeverity::Critical;
            } else if ([sevStr isEqualToString:@"High"]) {
                vuln.m_severity = LocalModels::VulnerabilityDetectionModel::VulnSeverity::High;
            } else if ([sevStr isEqualToString:@"Medium"]) {
                vuln.m_severity = LocalModels::VulnerabilityDetectionModel::VulnSeverity::Medium;
            } else if ([sevStr isEqualToString:@"Low"]) {
                vuln.m_severity = LocalModels::VulnerabilityDetectionModel::VulnSeverity::Low;
            } else {
                vuln.m_severity = LocalModels::VulnerabilityDetectionModel::VulnSeverity::Info;
            }
        }
        
        // Extract other fields
        NSString* description = [vulnDict objectForKey:@"description"];
        if (description) {
            vuln.m_description = [description UTF8String];
        }
        
        NSString* affectedCode = [vulnDict objectForKey:@"affectedCode"];
        if (affectedCode) {
            vuln.m_affectedCode = [affectedCode UTF8String];
        }
        
        NSNumber* lineNumber = [vulnDict objectForKey:@"lineNumber"];
        if (lineNumber) {
            vuln.m_lineNumber = [lineNumber intValue];
        }
        
        NSString* mitigation = [vulnDict objectForKey:@"mitigation"];
        if (mitigation) {
            vuln.m_mitigation = [mitigation UTF8String];
        }
        
        vulnerabilities.push_back(vuln);
    }
    
    // Provide feedback
    return m_vulnDetectionModel->ProvideFeedback(script, vulnerabilities, correctDetections);
}

// Provide script generation feedback
bool AISystemInitializer::ProvideScriptGenerationFeedback(
    const std::string& description, const std::string& generatedScript,
    const std::string& userScript, float rating) {
    
    if (!m_scriptGenModel || !m_scriptGenModel->IsInitialized()) {
        return false;
    }
    
    // Provide feedback
    return m_scriptGenModel->LearnFromFeedback(description, generatedScript, userScript, rating);
}

// Check if models are ready
bool AISystemInitializer::AreModelsReady() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // Check if both models are trained
    auto vulnStatus = m_modelStatus.find("VulnerabilityDetectionModel");
    auto scriptStatus = m_modelStatus.find("ScriptGenerationModel");
    
    return vulnStatus != m_modelStatus.end() && vulnStatus->second.m_state == InitState::Completed &&
           scriptStatus != m_modelStatus.end() && scriptStatus->second.m_state == InitState::Completed;
}

// Get system status report
std::string AISystemInitializer::GetSystemStatusReport() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // Create status report
    NSMutableDictionary* reportDict = [NSMutableDictionary dictionary];
    
    // Add system state
    [reportDict setObject:[NSString stringWithUTF8String:
                         (m_initState == InitState::Completed ? "Initialized" :
                          m_initState == InitState::InProgress ? "Initializing" :
                          m_initState == InitState::Failed ? "Failed" : "Not Started")]
                   forKey:@"state"];
    
    // Add model statuses
    NSMutableArray* modelsArray = [NSMutableArray array];
    
    for (const auto& pair : m_modelStatus) {
        const auto& status = pair.second;
        
        NSMutableDictionary* modelDict = [NSMutableDictionary dictionary];
        
        [modelDict setObject:[NSString stringWithUTF8String:status.m_name.c_str()] 
                      forKey:@"name"];
        
        [modelDict setObject:[NSString stringWithUTF8String:
                            (status.m_state == InitState::Completed ? "Trained" :
                             status.m_state == InitState::InProgress ? "Training" :
                             status.m_state == InitState::Failed ? "Failed" : "Not Started")]
                      forKey:@"state"];
        
        [modelDict setObject:@(status.m_trainingProgress) forKey:@"trainingProgress"];
        [modelDict setObject:@(status.m_accuracy) forKey:@"accuracy"];
        
        if (!status.m_version.empty()) {
            [modelDict setObject:[NSString stringWithUTF8String:status.m_version.c_str()] 
                          forKey:@"version"];
        }
        
        [modelsArray addObject:modelDict];
    }
    
    [reportDict setObject:modelsArray forKey:@"models"];
    
    // Add usage stats
    NSMutableDictionary* statsDict = [NSMutableDictionary dictionary];
    
    [statsDict setObject:@(m_vulnDetectionCount) forKey:@"vulnerabilityDetectionCount"];
    [statsDict setObject:@(m_scriptGenCount) forKey:@"scriptGenerationCount"];
    [statsDict setObject:@(m_fallbackUsageCount) forKey:@"fallbackUsageCount"];
    
    [reportDict setObject:statsDict forKey:@"usageStats"];
    
    // Add fallback info
    NSMutableDictionary* fallbackDict = [NSMutableDictionary dictionary];
    
    [fallbackDict setObject:[NSString stringWithUTF8String:
                           (m_currentFallbackStrategy == FallbackStrategy::BasicPatterns ? "BasicPatterns" :
                            m_currentFallbackStrategy == FallbackStrategy::CachedResults ? "CachedResults" :
                            m_currentFallbackStrategy == FallbackStrategy::PatternMatching ? "PatternMatching" :
                            m_currentFallbackStrategy == FallbackStrategy::RuleBased ? "RuleBased" : "HybridApproach")]
                     forKey:@"strategy"];
    
    [fallbackDict setObject:@(m_fallbackPatterns.size()) forKey:@"patternCount"];
    [fallbackDict setObject:@(m_cachedResults.size()) forKey:@"cachedResultsCount"];
    
    [reportDict setObject:fallbackDict forKey:@"fallback"];
    
    // Add self-modifying system info
    if (m_selfModifyingSystem && m_selfModifyingSystem->IsInitialized()) {
        NSMutableDictionary* selfModDict = [NSMutableDictionary dictionary];
        
        [selfModDict setObject:@(m_selfModifyingSystem->GetAllSegmentNames().size()) 
                        forKey:@"segmentCount"];
        
        [selfModDict setObject:@(m_selfModifyingSystem->GetAppliedPatches().size()) 
                        forKey:@"appliedPatchCount"];
        
        [selfModDict setObject:@(m_selfModifyingSystem->GetAvailablePatches().size()) 
                        forKey:@"availablePatchCount"];
        
        [reportDict setObject:selfModDict forKey:@"selfModifyingSystem"];
    }
    
    // Convert to JSON string
    NSError* error = nil;
    NSData* jsonData = [NSJSONSerialization dataWithJSONObject:reportDict
                                                       options:NSJSONWritingPrettyPrinted
                                                         error:&error];
    
    if (error || !jsonData) {
        return "{}";
    }
    
    NSString* jsonString = [[NSString alloc] initWithData:jsonData encoding:NSUTF8StringEncoding];
    return [jsonString UTF8String];
}

// Check if system is in fallback mode
bool AISystemInitializer::IsInFallbackMode() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // Check if models are trained
    return !AreModelsReady();
}

// Resume training thread
bool AISystemInitializer::ResumeTraining() {
    // If thread is not running, restart it
    if (!m_trainingThreadRunning) {
        m_trainingThreadRunning = true;
        
        // Join previous thread if needed
        if (m_trainingThread.joinable()) {
            m_trainingThread.join();
        }
        
        // Start new thread
        m_trainingThread = std::thread(&AISystemInitializer::TrainingThreadFunc, this);
        
        return true;
    }
    
    return false; // Already running
}

// Pause training thread
bool AISystemInitializer::PauseTraining() {
    // If thread is running, stop it
    if (m_trainingThreadRunning) {
        m_trainingThreadRunning = false;
        
        // Don't join here, let thread finish naturally
        return true;
    }
    
    return false; // Already paused
}

} // namespace AIFeatures
} // namespace iOS
