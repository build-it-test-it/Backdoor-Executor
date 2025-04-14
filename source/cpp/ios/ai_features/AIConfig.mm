#include "AIConfig.h"
#include <iostream>
#include <fstream>
#include <sstream>
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

namespace iOS {
namespace AIFeatures {

// Initialize static instance
AIConfig* AIConfig::s_instance = nullptr;

// Constructor
AIConfig::AIConfig()
    : m_onlineMode(OnlineMode::Auto),
      m_modelQuality(ModelQuality::Medium),
      m_encryptCommunication(true),
      m_shareUsageData(false),
      m_saveHistory(true),
      m_maxHistoryItems(100),
      m_useGPT4(false),
      m_debugLogging(false),
      m_maxMemoryUsage(200 * 1024 * 1024), // 200MB default
      m_showNetworkIndicator(true),
      m_autoUpdate(true) {
    
    // Set default API endpoint
    m_apiEndpoint = "https://api.example.com/ai/v1";
    
    // Set default model path (will be updated during initialization)
    m_modelPath = "";
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
    // Set model path to app's Resources/Models directory
    NSBundle* mainBundle = [NSBundle mainBundle];
    NSString* resourcePath = [mainBundle resourcePath];
    NSString* modelsPath = [resourcePath stringByAppendingPathComponent:@"Models"];
    
    // Create models directory if it doesn't exist
    NSFileManager* fileManager = [NSFileManager defaultManager];
    if (![fileManager fileExistsAtPath:modelsPath]) {
        [fileManager createDirectoryAtPath:modelsPath withIntermediateDirectories:YES attributes:nil error:nil];
    }
    
    m_modelPath = [modelsPath UTF8String];
    
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
        if (NSString* endpoint = [json objectForKey:@"api_endpoint"]) {
            m_apiEndpoint = [endpoint UTF8String];
        }
        
        if (NSString* apiKey = [json objectForKey:@"api_key"]) {
            m_apiKey = [apiKey UTF8String];
        }
        
        if (NSString* onlineMode = [json objectForKey:@"online_mode"]) {
            m_onlineMode = StringToOnlineMode([onlineMode UTF8String]);
        }
        
        if (NSString* modelQuality = [json objectForKey:@"model_quality"]) {
            m_modelQuality = StringToModelQuality([modelQuality UTF8String]);
        }
        
        if (NSNumber* encrypt = [json objectForKey:@"encrypt_communication"]) {
            m_encryptCommunication = [encrypt boolValue];
        }
        
        if (NSNumber* shareData = [json objectForKey:@"share_usage_data"]) {
            m_shareUsageData = [shareData boolValue];
        }
        
        if (NSNumber* saveHistory = [json objectForKey:@"save_history"]) {
            m_saveHistory = [saveHistory boolValue];
        }
        
        if (NSNumber* maxHistory = [json objectForKey:@"max_history_items"]) {
            m_maxHistoryItems = [maxHistory unsignedIntValue];
        }
        
        if (NSNumber* useGPT4 = [json objectForKey:@"use_gpt4"]) {
            m_useGPT4 = [useGPT4 boolValue];
        }
        
        if (NSNumber* debug = [json objectForKey:@"debug_logging"]) {
            m_debugLogging = [debug boolValue];
        }
        
        if (NSString* modelPath = [json objectForKey:@"model_path"]) {
            m_modelPath = [modelPath UTF8String];
        }
        
        if (NSNumber* maxMemory = [json objectForKey:@"max_memory_usage"]) {
            m_maxMemoryUsage = [maxMemory unsignedLongLongValue];
        }
        
        if (NSNumber* showIndicator = [json objectForKey:@"show_network_indicator"]) {
            m_showNetworkIndicator = [showIndicator boolValue];
        }
        
        if (NSNumber* autoUpdate = [json objectForKey:@"auto_update"]) {
            m_autoUpdate = [autoUpdate boolValue];
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
    [json setObject:[NSString stringWithUTF8String:m_apiEndpoint.c_str()] forKey:@"api_endpoint"];
    [json setObject:[NSString stringWithUTF8String:m_apiKey.c_str()] forKey:@"api_key"];
    [json setObject:[NSString stringWithUTF8String:OnlineModeToString(m_onlineMode).c_str()] forKey:@"online_mode"];
    [json setObject:[NSString stringWithUTF8String:ModelQualityToString(m_modelQuality).c_str()] forKey:@"model_quality"];
    [json setObject:@(m_encryptCommunication) forKey:@"encrypt_communication"];
    [json setObject:@(m_shareUsageData) forKey:@"share_usage_data"];
    [json setObject:@(m_saveHistory) forKey:@"save_history"];
    [json setObject:@(m_maxHistoryItems) forKey:@"max_history_items"];
    [json setObject:@(m_useGPT4) forKey:@"use_gpt4"];
    [json setObject:@(m_debugLogging) forKey:@"debug_logging"];
    [json setObject:[NSString stringWithUTF8String:m_modelPath.c_str()] forKey:@"model_path"];
    [json setObject:@(m_maxMemoryUsage) forKey:@"max_memory_usage"];
    [json setObject:@(m_showNetworkIndicator) forKey:@"show_network_indicator"];
    [json setObject:@(m_autoUpdate) forKey:@"auto_update"];
    
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

// Set API endpoint
void AIConfig::SetAPIEndpoint(const std::string& endpoint) {
    m_apiEndpoint = endpoint;
}

// Get API endpoint
std::string AIConfig::GetAPIEndpoint() const {
    return m_apiEndpoint;
}

// Set API key
void AIConfig::SetAPIKey(const std::string& apiKey) {
    m_apiKey = apiKey;
}

// Get API key
std::string AIConfig::GetAPIKey() const {
    return m_apiKey;
}

// Set online mode
void AIConfig::SetOnlineMode(OnlineMode mode) {
    m_onlineMode = mode;
}

// Get online mode
AIConfig::OnlineMode AIConfig::GetOnlineMode() const {
    return m_onlineMode;
}

// Set model quality
void AIConfig::SetModelQuality(ModelQuality quality) {
    m_modelQuality = quality;
}

// Get model quality
AIConfig::ModelQuality AIConfig::GetModelQuality() const {
    return m_modelQuality;
}

// Set whether to encrypt communication
void AIConfig::SetEncryptCommunication(bool encrypt) {
    m_encryptCommunication = encrypt;
}

// Get whether to encrypt communication
bool AIConfig::GetEncryptCommunication() const {
    return m_encryptCommunication;
}

// Set whether to share usage data
void AIConfig::SetShareUsageData(bool share) {
    m_shareUsageData = share;
}

// Get whether to share usage data
bool AIConfig::GetShareUsageData() const {
    return m_shareUsageData;
}

// Set whether to save history
void AIConfig::SetSaveHistory(bool save) {
    m_saveHistory = save;
}

// Get whether to save history
bool AIConfig::GetSaveHistory() const {
    return m_saveHistory;
}

// Set maximum history items
void AIConfig::SetMaxHistoryItems(uint32_t max) {
    m_maxHistoryItems = max;
}

// Get maximum history items
uint32_t AIConfig::GetMaxHistoryItems() const {
    return m_maxHistoryItems;
}

// Set whether to use GPT-4
void AIConfig::SetUseGPT4(bool use) {
    m_useGPT4 = use;
}

// Get whether to use GPT-4
bool AIConfig::GetUseGPT4() const {
    return m_useGPT4;
}

// Set whether to enable debug logging
void AIConfig::SetDebugLogging(bool enable) {
    m_debugLogging = enable;
}

// Get whether to enable debug logging
bool AIConfig::GetDebugLogging() const {
    return m_debugLogging;
}

// Set model path
void AIConfig::SetModelPath(const std::string& path) {
    m_modelPath = path;
}

// Get model path
std::string AIConfig::GetModelPath() const {
    return m_modelPath;
}

// Set maximum memory usage
void AIConfig::SetMaxMemoryUsage(uint64_t max) {
    m_maxMemoryUsage = max;
}

// Get maximum memory usage
uint64_t AIConfig::GetMaxMemoryUsage() const {
    return m_maxMemoryUsage;
}

// Set whether to show network indicator
void AIConfig::SetShowNetworkIndicator(bool show) {
    m_showNetworkIndicator = show;
}

// Get whether to show network indicator
bool AIConfig::GetShowNetworkIndicator() const {
    return m_showNetworkIndicator;
}

// Set whether to auto-update models
void AIConfig::SetAutoUpdate(bool autoUpdate) {
    m_autoUpdate = autoUpdate;
}

// Get whether to auto-update models
bool AIConfig::GetAutoUpdate() const {
    return m_autoUpdate;
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
    m_onlineMode = OnlineMode::Auto;
    m_modelQuality = ModelQuality::Medium;
    m_encryptCommunication = true;
    m_shareUsageData = false;
    m_saveHistory = true;
    m_maxHistoryItems = 100;
    m_useGPT4 = false;
    m_debugLogging = false;
    m_maxMemoryUsage = 200 * 1024 * 1024; // 200MB default
    m_showNetworkIndicator = true;
    m_autoUpdate = true;
    
    // Set default API endpoint
    m_apiEndpoint = "https://api.example.com/ai/v1";
    
    // Keep model path as is
    
    // Clear custom options
    m_options.clear();
    
    // Auto-detect optimal settings
    AutoDetectOptimalSettings();
    
    // Save changes
    SaveConfig();
}

// Save changes
bool AIConfig::Save() {
    return SaveConfig();
}

// Get appropriate model filename for current quality setting
std::string AIConfig::GetModelFilename(const std::string& baseModelName) const {
    std::string suffix;
    
    switch (m_modelQuality) {
        case ModelQuality::Low:
            suffix = "_lite";
            break;
        case ModelQuality::Medium:
            suffix = "_medium";
            break;
        case ModelQuality::High:
            suffix = "_full";
            break;
        default:
            suffix = "_medium"; // Default to medium
            break;
    }
    
    return baseModelName + suffix + ".mlmodel";
}

// Check if device supports high quality models
bool AIConfig::DeviceSupportsHighQualityModels() const {
    // Check available memory
    UIDevice* device = [UIDevice currentDevice];
    
    if (@available(iOS 15.0, *)) {
        if ([device respondsToSelector:@selector(systemFreeSize)]) {
            uint64_t freeMemory = [device systemFreeSize];
            
            // Require at least 1GB free memory for high quality models
            return freeMemory >= 1024 * 1024 * 1024;
        }
    }
    
    // For older iOS versions or if can't determine memory, check device model
    NSString* model = [device model];
    
    // Check for newer devices that can handle high quality models
    if ([model containsString:@"iPhone"] || [model containsString:@"iPad"]) {
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
            
            // iPhone 11+ or iPad 6th gen+ should support high quality models
            if (([model containsString:@"iPhone"] && modelNum >= 11) ||
                ([model containsString:@"iPad"] && modelNum >= 6)) {
                return true;
            }
        }
    }
    
    return false;
}

// Auto-detect optimal settings
void AIConfig::AutoDetectOptimalSettings() {
    // Detect device capabilities
    bool supportsHighQuality = DeviceSupportsHighQualityModels();
    
    // Set model quality based on device capabilities
    if (supportsHighQuality) {
        m_modelQuality = ModelQuality::Medium; // Default to medium, user can upgrade to high
    } else {
        m_modelQuality = ModelQuality::Low;
    }
    
    // Detect available memory
    UIDevice* device = [UIDevice currentDevice];
    if (@available(iOS 15.0, *)) {
        if ([device respondsToSelector:@selector(systemFreeSize)]) {
            uint64_t freeMemory = [device systemFreeSize];
            
            // Set max memory usage based on available memory
            // Use up to 25% of available memory, with upper limit
            uint64_t availableForUse = freeMemory / 4;
            
            // Cap at 800MB for high-end devices
            const uint64_t MAX_MEMORY = 800 * 1024 * 1024;
            
            if (availableForUse > MAX_MEMORY) {
                availableForUse = MAX_MEMORY;
            }
            
            // Ensure at least 100MB for minimum functionality
            const uint64_t MIN_MEMORY = 100 * 1024 * 1024;
            
            if (availableForUse < MIN_MEMORY) {
                availableForUse = MIN_MEMORY;
            }
            
            m_maxMemoryUsage = availableForUse;
        }
    }
    
    // Check network capabilities
    // This is just a basic implementation - in a real app, you would use Reachability
    // to accurately determine network status
    bool wifiAvailable = [UIApplication sharedApplication].delegate != nil;
    
    if (wifiAvailable) {
        m_onlineMode = OnlineMode::Auto; // Use online when available
    } else {
        m_onlineMode = OnlineMode::PreferOffline; // Prefer offline when on cellular
    }
}

// Convert online mode to string
std::string AIConfig::OnlineModeToString(OnlineMode mode) {
    switch (mode) {
        case OnlineMode::Auto:
            return "auto";
        case OnlineMode::PreferOffline:
            return "prefer_offline";
        case OnlineMode::PreferOnline:
            return "prefer_online";
        case OnlineMode::OfflineOnly:
            return "offline_only";
        case OnlineMode::OnlineOnly:
            return "online_only";
        default:
            return "auto";
    }
}

// Convert string to online mode
AIConfig::OnlineMode AIConfig::StringToOnlineMode(const std::string& str) {
    if (str == "auto") {
        return OnlineMode::Auto;
    } else if (str == "prefer_offline") {
        return OnlineMode::PreferOffline;
    } else if (str == "prefer_online") {
        return OnlineMode::PreferOnline;
    } else if (str == "offline_only") {
        return OnlineMode::OfflineOnly;
    } else if (str == "online_only") {
        return OnlineMode::OnlineOnly;
    } else {
        return OnlineMode::Auto;
    }
}

// Convert model quality to string
std::string AIConfig::ModelQualityToString(ModelQuality quality) {
    switch (quality) {
        case ModelQuality::Low:
            return "low";
        case ModelQuality::Medium:
            return "medium";
        case ModelQuality::High:
            return "high";
        default:
            return "medium";
    }
}

// Convert string to model quality
AIConfig::ModelQuality AIConfig::StringToModelQuality(const std::string& str) {
    if (str == "low") {
        return ModelQuality::Low;
    } else if (str == "medium") {
        return ModelQuality::Medium;
    } else if (str == "high") {
        return ModelQuality::High;
    } else {
        return ModelQuality::Medium;
    }
}

} // namespace AIFeatures
} // namespace iOS
