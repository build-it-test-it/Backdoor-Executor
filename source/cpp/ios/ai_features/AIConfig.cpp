#include "AIConfig.h"

namespace iOS {
namespace AIFeatures {

// Initialize static instance pointer
AIConfig* AIConfig::s_instance = nullptr;

// Get shared instance
AIConfig& AIConfig::GetSharedInstance() {
    if (!s_instance) {
        s_instance = new AIConfig();
    }
    return *s_instance;
}

// Initialize with default settings
bool AIConfig::Initialize() {
    // Set default paths and options
    if (m_options.empty()) {
        // AI data directory
        SetOption("model_path", "/var/mobile/Documents/AIData/Models");
        SetOption("config_path", "/var/mobile/Documents/AIData/config.json");
        
        // API settings
        SetOption("api_endpoint", "https://api.example.com/ai");
        SetOption("api_key", "");
        
        // Feature toggles
        SetOption("enable_ai_features", "1");
        SetOption("enable_script_analysis", "1");
        SetOption("enable_vulnerability_detection", "1");
        SetOption("enable_signature_adaptation", "1");
        
        // Learning settings
        SetOption("learning_mode", "continuous");
        SetOption("model_improvement", "local");
        
        // Network settings
        SetOption("online_mode", "auto");
        SetOption("encrypt_communication", "1");
        
        // Performance settings
        SetOption("model_quality", "medium");
        SetOption("max_memory_usage", "200000000"); // 200MB
    }
    
    return true;
}

// Check if initialized
bool AIConfig::IsInitialized() const {
    return !m_options.empty();
}

// Save config to file
bool AIConfig::Save() const {
    // In a real implementation, would save to file
    // For now, just return success
    return true;
}

// Get API endpoint
std::string AIConfig::GetAPIEndpoint() const {
    return GetOption("api_endpoint", "https://api.example.com/ai");
}

// Set API endpoint
void AIConfig::SetAPIEndpoint(const std::string& endpoint) {
    SetOption("api_endpoint", endpoint);
}

// Get API key
std::string AIConfig::GetAPIKey() const {
    return GetOption("api_key", "");
}

// Set API key
void AIConfig::SetAPIKey(const std::string& key) {
    SetOption("api_key", key);
}

// Get model path
std::string AIConfig::GetModelPath() const {
    return GetOption("model_path", "/var/mobile/Documents/AIData/Models");
}

// Set model path
void AIConfig::SetModelPath(const std::string& path) {
    SetOption("model_path", path);
}

// Get encrypt communication
bool AIConfig::GetEncryptCommunication() const {
    return GetOption("encrypt_communication", "1") == "1";
}

// Set encrypt communication
void AIConfig::SetEncryptCommunication(bool encrypt) {
    SetOption("encrypt_communication", encrypt ? "1" : "0");
}

// Get max memory usage
uint64_t AIConfig::GetMaxMemoryUsage() const {
    std::string value = GetOption("max_memory_usage", "200000000");
    return std::stoull(value);
}

// Set max memory usage
void AIConfig::SetMaxMemoryUsage(uint64_t maxMemory) {
    SetOption("max_memory_usage", std::to_string(maxMemory));
}

} // namespace AIFeatures
} // namespace iOS
