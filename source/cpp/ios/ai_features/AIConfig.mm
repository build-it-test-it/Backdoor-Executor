#include "../ios_compat.h"
#include "AIConfig.h"
#include <iostream>
#include <fstream>
#include <sstream>

namespace iOS {
namespace AIFeatures {

// Initialize static instance
AIConfig* AIConfig::s_instance = nullptr;

// Constructor
AIConfig::AIConfig()
    : m_operationMode(OperationMode::Standard),
      m_learningMode(LearningMode::Continuous),
      m_enableVulnerabilityScanner(true),
      m_enableScriptGeneration(true),
      m_enableCodeDebugging(true),
      m_enableUIAssistant(true),
      m_debugLogging(false),
      m_maxMemoryUsage(200 * 1024 * 1024), // 200MB default
      m_maxHistoryItems(100),
      m_saveHistory(true) {
    
    // Set default data path (will be updated during initialization)
    m_dataPath = "";
}

// Destructor
AIConfig::~AIConfig() {
    // Save config if modified
    SaveConfig();
}

// Get shared instance
AIConfig& AIConfig::GetSharedInstance() {
    if (!s_instance) {
        s_instance = new AIConfig();
        s_instance->Initialize();
    }
    return *s_instance;
}

// Initialize with default values
bool AIConfig::Initialize() {
    // Set path to app's Documents/AIData directory
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString* documentsDirectory = [paths objectAtIndex:0];
    NSString* aiDataPath = [documentsDirectory stringByAppendingPathComponent:@"AIData"];
    
    // Create AIData directory if it doesn't exist
    NSFileManager* fileManager = [NSFileManager defaultManager];
    if (![fileManager fileExistsAtPath:aiDataPath]) {
        [fileManager createDirectoryAtPath:aiDataPath withIntermediateDirectories:YES attributes:nil error:nil];
    }
    
    m_dataPath = [aiDataPath UTF8String];
    
    // Auto-detect optimal settings
    AutoDetectOptimalSettings();
    
    // Load config from file
    bool loaded = LoadConfig();
    
    // If not loaded, save default config
    if (!loaded) {
        SaveConfig();
    }
    
    return true;
}

// Load config from file
bool AIConfig::LoadConfig() {
    // Get config file path
    std::string configPath = GetConfigFilePath();
    
    // Check if file exists
    NSFileManager* fileManager = [NSFileManager defaultManager];
    NSString* nsConfigPath = [NSString stringWithUTF8String:configPath.c_str()];
    
    if (![fileManager fileExistsAtPath:nsConfigPath]) {
        return false;
    }
    
    // Read file
    NSData* data = [NSData dataWithContentsOfFile:nsConfigPath];
    if (!data) {
        return false;
    }
    
    // Parse JSON
    NSError* error = nil;
    NSDictionary* json = [NSJSONSerialization JSONObjectWithData:data options:0 error:&error];
    
    if (error || !json) {
        return false;
    }
    
    try {
        // Extract values
        if (NSString* dataPath = [json objectForKey:@"data_path"]) {
            m_dataPath = [dataPath UTF8String];
        }
        
        if (NSString* operationMode = [json objectForKey:@"operation_mode"]) {
            m_operationMode = StringToOperationMode([operationMode UTF8String]);
        }
        
        if (NSString* learningMode = [json objectForKey:@"learning_mode"]) {
            m_learningMode = StringToLearningMode([learningMode UTF8String]);
        }
        
        if (NSNumber* vulnerabilityScanner = [json objectForKey:@"enable_vulnerability_scanner"]) {
            m_enableVulnerabilityScanner = [vulnerabilityScanner boolValue];
        }
        
        if (NSNumber* scriptGeneration = [json objectForKey:@"enable_script_generation"]) {
            m_enableScriptGeneration = [scriptGeneration boolValue];
        }
        
        if (NSNumber* codeDebugging = [json objectForKey:@"enable_code_debugging"]) {
            m_enableCodeDebugging = [codeDebugging boolValue];
        }
        
        if (NSNumber* uiAssistant = [json objectForKey:@"enable_ui_assistant"]) {
            m_enableUIAssistant = [uiAssistant boolValue];
        }
        
        if (NSNumber* debug = [json objectForKey:@"debug_logging"]) {
            m_debugLogging = [debug boolValue];
        }
        
        if (NSNumber* maxMemory = [json objectForKey:@"max_memory_usage"]) {
            m_maxMemoryUsage = [maxMemory unsignedLongLongValue];
        }
        
        if (NSNumber* maxHistory = [json objectForKey:@"max_history_items"]) {
            m_maxHistoryItems = [maxHistory unsignedIntValue];
        }
        
        if (NSNumber* saveHistory = [json objectForKey:@"save_history"]) {
            m_saveHistory = [saveHistory boolValue];
        }
        
        // Load custom options
        if (NSDictionary* options = [json objectForKey:@"options"]) {
            for (NSString* key in options) {
                NSString* value = [options objectForKey:key];
                m_options[[key UTF8String]] = [value UTF8String];
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "AIConfig: Exception during loading: " << e.what() << std::endl;
        return false;
    }
}

// Save config to file
bool AIConfig::SaveConfig() {
    // Create JSON dictionary
    NSMutableDictionary* json = [NSMutableDictionary dictionary];
    
    // Add values
    [json setObject:[NSString stringWithUTF8String:m_dataPath.c_str()] forKey:@"data_path"];
    [json setObject:[NSString stringWithUTF8String:OperationModeToString(m_operationMode).c_str()] forKey:@"operation_mode"];
    [json setObject:[NSString stringWithUTF8String:LearningModeToString(m_learningMode).c_str()] forKey:@"learning_mode"];
    [json setObject:@(m_enableVulnerabilityScanner) forKey:@"enable_vulnerability_scanner"];
    [json setObject:@(m_enableScriptGeneration) forKey:@"enable_script_generation"];
    [json setObject:@(m_enableCodeDebugging) forKey:@"enable_code_debugging"];
    [json setObject:@(m_enableUIAssistant) forKey:@"enable_ui_assistant"];
    [json setObject:@(m_debugLogging) forKey:@"debug_logging"];
    [json setObject:@(m_maxMemoryUsage) forKey:@"max_memory_usage"];
    [json setObject:@(m_maxHistoryItems) forKey:@"max_history_items"];
    [json setObject:@(m_saveHistory) forKey:@"save_history"];
    
    // Add custom options
    NSMutableDictionary* options = [NSMutableDictionary dictionary];
    for (const auto& option : m_options) {
        [options setObject:[NSString stringWithUTF8String:option.second.c_str()] 
                    forKey:[NSString stringWithUTF8String:option.first.c_str()]];
    }
    [json setObject:options forKey:@"options"];
    
    // Convert to JSON data
    NSError* error = nil;
    NSData* data = [NSJSONSerialization dataWithJSONObject:json options:NSJSONWritingPrettyPrinted error:&error];
    
    if (error || !data) {
        return false;
    }
    
    // Write to file
    NSString* nsConfigPath = [NSString stringWithUTF8String:GetConfigFilePath().c_str()];
    return [data writeToFile:nsConfigPath atomically:YES];
}

// Get config file path
std::string AIConfig::GetConfigFilePath() const {
    // Get Documents directory
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString* documentsDirectory = [paths objectAtIndex:0];
    
    // Create AIData directory if it doesn't exist
    NSString* aiDataDir = [documentsDirectory stringByAppendingPathComponent:@"AIData"];
    NSFileManager* fileManager = [NSFileManager defaultManager];
    if (![fileManager fileExistsAtPath:aiDataDir]) {
        [fileManager createDirectoryAtPath:aiDataDir withIntermediateDirectories:YES attributes:nil error:nil];
    }
    
    // Config file path
    NSString* configPath = [aiDataDir stringByAppendingPathComponent:@"ai_config.json"];
    return [configPath UTF8String];
}

// Set data path
void AIConfig::SetDataPath(const std::string& path) {
    m_dataPath = path;
}

// Get data path
std::string AIConfig::GetDataPath() const {
    return m_dataPath;
}

// Set operation mode
void AIConfig::SetOperationMode(OperationMode mode) {
    m_operationMode = mode;
}

// Get operation mode
AIConfig::OperationMode AIConfig::GetOperationMode() const {
    return m_operationMode;
}

// Set learning mode
void AIConfig::SetLearningMode(LearningMode mode) {
    m_learningMode = mode;
}

// Get learning mode
AIConfig::LearningMode AIConfig::GetLearningMode() const {
    return m_learningMode;
}

// Set whether to enable vulnerability scanner
void AIConfig::SetEnableVulnerabilityScanner(bool enable) {
    m_enableVulnerabilityScanner = enable;
}

// Get whether vulnerability scanner is enabled
bool AIConfig::GetEnableVulnerabilityScanner() const {
    return m_enableVulnerabilityScanner;
}

// Set whether to enable script generation
void AIConfig::SetEnableScriptGeneration(bool enable) {
    m_enableScriptGeneration = enable;
}

// Get whether script generation is enabled
bool AIConfig::GetEnableScriptGeneration() const {
    return m_enableScriptGeneration;
}

// Set whether to enable code debugging
void AIConfig::SetEnableCodeDebugging(bool enable) {
    m_enableCodeDebugging = enable;
}

// Get whether code debugging is enabled
bool AIConfig::GetEnableCodeDebugging() const {
    return m_enableCodeDebugging;
}

// Set whether to enable UI assistant
void AIConfig::SetEnableUIAssistant(bool enable) {
    m_enableUIAssistant = enable;
}

// Get whether UI assistant is enabled
bool AIConfig::GetEnableUIAssistant() const {
    return m_enableUIAssistant;
}

// Set whether to enable debug logging
void AIConfig::SetDebugLogging(bool enable) {
    m_debugLogging = enable;
}

// Get whether debug logging is enabled
bool AIConfig::GetDebugLogging() const {
    return m_debugLogging;
}

// Set maximum memory usage
void AIConfig::SetMaxMemoryUsage(uint64_t max) {
    m_maxMemoryUsage = max;
}

// Get maximum memory usage
uint64_t AIConfig::GetMaxMemoryUsage() const {
    return m_maxMemoryUsage;
}

// Set maximum history items
void AIConfig::SetMaxHistoryItems(uint32_t max) {
    m_maxHistoryItems = max;
}

// Get maximum history items
uint32_t AIConfig::GetMaxHistoryItems() const {
    return m_maxHistoryItems;
}

// Set whether to save history
void AIConfig::SetSaveHistory(bool save) {
    m_saveHistory = save;
}

// Get whether to save history
bool AIConfig::GetSaveHistory() const {
    return m_saveHistory;
}

// Set custom option
void AIConfig::SetOption(const std::string& key, const std::string& value) {
    m_options[key] = value;
}

// Get custom option
std::string AIConfig::GetOption(const std::string& key, const std::string& defaultValue) const {
    auto it = m_options.find(key);
    if (it != m_options.end()) {
        return it->second;
    }
    return defaultValue;
}

// Reset all settings to defaults
void AIConfig::ResetToDefaults() {
    // Reset to constructor defaults
    m_operationMode = OperationMode::Standard;
    m_learningMode = LearningMode::Continuous;
    m_enableVulnerabilityScanner = true;
    m_enableScriptGeneration = true;
    m_enableCodeDebugging = true;
    m_enableUIAssistant = true;
    m_debugLogging = false;
    m_maxMemoryUsage = 200 * 1024 * 1024; // 200MB default
    m_maxHistoryItems = 100;
    m_saveHistory = true;
    
    // Keep data path as is
    
    // Clear custom options
    m_options.clear();
    
    // Auto-detect optimal settings
    AutoDetectOptimalSettings();
    
    // Save changes
    SaveConfig();
}

/**
 * @brief Set online mode
 * @param mode Online mode
 */
void AIConfig::SetOnlineMode(OnlineMode mode) {
    // Convert enum value to string representation
    std::string modeStr;
    switch (mode) {
        case OnlineMode::Auto:
            modeStr = "auto";
            break;
        case OnlineMode::PreferOffline:
            modeStr = "prefer_offline";
            break;
        case OnlineMode::PreferOnline:
            modeStr = "prefer_online";
            break;
        case OnlineMode::OfflineOnly:
            modeStr = "offline_only";
            break;
        case OnlineMode::OnlineOnly:
            modeStr = "online_only";
            break;
        default:
            modeStr = "auto";
            break;
    }
    
    // Save to options
    SetOption("online_mode", modeStr);
}

/**
 * @brief Get online mode
 * @return Online mode
 */
AIConfig::OnlineMode AIConfig::GetOnlineMode() const {
    // Get from options with default value - use auto as default for online training
    std::string modeStr = GetOption("online_mode", "auto");
    
    // Convert string to enum value
    if (modeStr == "auto") {
        return OnlineMode::Auto;
    } else if (modeStr == "prefer_offline") {
        return OnlineMode::PreferOffline;
    } else if (modeStr == "prefer_online") {
        return OnlineMode::PreferOnline;
    } else if (modeStr == "offline_only") {
        return OnlineMode::OfflineOnly;
    } else if (modeStr == "online_only") {
        return OnlineMode::OnlineOnly;
    } else {
        return OnlineMode::Auto; // Default to auto for best network usage
    }
}

// Save changes
bool AIConfig::Save() {
    return SaveConfig();
}

// Auto-detect optimal settings
void AIConfig::AutoDetectOptimalSettings() {
    // Detect device capabilities
    UIDevice* device = [UIDevice currentDevice];
    
    // Detect available memory
    if (@available(iOS 15.0, *)) {
        if ([device respondsToSelector:@selector(systemFreeSize)]) {
            // We can't use systemFreeSize directly as it's not available
            // Use a reasonable default value based on device model
            uint64_t freeMemory = 2ULL * 1024ULL * 1024ULL * 1024ULL; // Default to 2GB
            
            // Set max memory usage based on available memory
            // Use up to 25% of available memory, with upper limit
            uint64_t availableForUse = freeMemory / 4;
            
            // Cap at 500MB for high-end devices
            const uint64_t MAX_MEMORY = 500 * 1024 * 1024;
            
            if (availableForUse > MAX_MEMORY) {
                availableForUse = MAX_MEMORY;
            }
            
            // Ensure at least 50MB for minimum functionality
            const uint64_t MIN_MEMORY = 50 * 1024 * 1024;
            
            if (availableForUse < MIN_MEMORY) {
                availableForUse = MIN_MEMORY;
            }
            
            m_maxMemoryUsage = availableForUse;
            
            // Set operation mode based on available memory
            if (freeMemory < 500 * 1024 * 1024) {
                m_operationMode = OperationMode::LowMemory;
            } else if (freeMemory < 1024 * 1024 * 1024) {
                m_operationMode = OperationMode::HighPerformance;
            } else {
                m_operationMode = OperationMode::Standard;
            }
        }
    }
    
    // Check device model for older devices
    NSString* model = [device model];
    
    // Extract model number
    NSRegularExpression* regex = [NSRegularExpression 
                                  regularExpressionWithPattern:@"\\d+" 
                                  options:0 
                                  error:nil];
    
    NSTextCheckingResult* match = [regex firstMatchInString:model
                                   options:0
                                   range:NSMakeRange(0, [model length])];
    
    if (match) {
        NSString* modelNumber = [model substringWithRange:[match range]];
        int modelNum = [modelNumber intValue];
        
        // For older devices, use low memory mode
        if (([model containsString:@"iPhone"] && modelNum < 11) ||
            ([model containsString:@"iPad"] && modelNum < 6)) {
            m_operationMode = OperationMode::LowMemory;
        }
    }
}

// Convert operation mode to string
std::string AIConfig::OperationModeToString(OperationMode mode) {
    switch (mode) {
        case OperationMode::Standard:
            return "standard";
        case OperationMode::HighPerformance:
            return "high_performance";
        case OperationMode::HighQuality:
            return "high_quality";
        case OperationMode::LowMemory:
            return "low_memory";
        default:
            return "standard";
    }
}

// Convert string to operation mode
AIConfig::OperationMode AIConfig::StringToOperationMode(const std::string& str) {
    if (str == "standard") {
        return OperationMode::Standard;
    } else if (str == "high_performance") {
        return OperationMode::HighPerformance;
    } else if (str == "high_quality") {
        return OperationMode::HighQuality;
    } else if (str == "low_memory") {
        return OperationMode::LowMemory;
    } else {
        return OperationMode::Standard;
    }
}

// Convert learning mode to string
std::string AIConfig::LearningModeToString(LearningMode mode) {
    switch (mode) {
        case LearningMode::Continuous:
            return "continuous";
        case LearningMode::OnDemand:
            return "on_demand";
        case LearningMode::Scheduled:
            return "scheduled";
        case LearningMode::Disabled:
            return "disabled";
        default:
            return "continuous";
    }
}

// Convert string to learning mode
AIConfig::LearningMode AIConfig::StringToLearningMode(const std::string& str) {
    if (str == "continuous") {
        return LearningMode::Continuous;
    } else if (str == "on_demand") {
        return LearningMode::OnDemand;
    } else if (str == "scheduled") {
        return LearningMode::Scheduled;
    } else if (str == "disabled") {
        return LearningMode::Disabled;
    } else {
        return LearningMode::Continuous;
    }
}

} // namespace AIFeatures
} // namespace iOS
