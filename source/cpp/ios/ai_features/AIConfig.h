
#include "../objc_isolation.h"
#pragma once

#include <string>
#include <unordered_map>
#include <functional>
#include "HybridAISystem.h" // Include for OnlineMode type

namespace iOS {
namespace AIFeatures {

/**
 * @class AIConfig
 * @brief Configuration manager for AI features
 * 
 * This class provides a centralized configuration system for all AI
 * features, allowing settings to be saved and loaded with proper
 * default values for local model training and operation.
 */
class AIConfig {
public:
    // AI operation mode enum
    enum class OperationMode {
        Standard,       // Standard operation (balanced performance/quality)
        HighPerformance, // Focus on speed (faster responses, lower quality)
        HighQuality,    // Focus on quality (better responses, slower)
        LowMemory       // Minimize memory usage
    };
    
    // AI learning mode enum
    enum class LearningMode {
        Continuous,     // Learn continuously from all interactions
        OnDemand,       // Learn only when explicitly requested 
        Scheduled,      // Learn on a schedule (e.g., daily)
        Disabled        // No learning
    };
    
    // Model quality enum
    enum class ModelQuality {
        Low,           // Lower quality models (faster, less memory)
        Medium,        // Medium quality models (balanced)
        High           // Higher quality models (slower, more memory)
    };
    
    /**
     * @brief Set model quality
     * @param quality Model quality
     */
    void SetModelQuality(ModelQuality quality) { 
        std::string qualityStr;
        switch (quality) {
            case ModelQuality::Low:
                qualityStr = "low";
                break;
            case ModelQuality::Medium:
                qualityStr = "medium";
                break;
            case ModelQuality::High:
                qualityStr = "high";
                break;
            default:
                qualityStr = "medium";
                break;
        }
        SetOption("model_quality", qualityStr);
    }
    
    /**
     * @brief Get model quality
     * @return Model quality
     */
    ModelQuality GetModelQuality() const {
        std::string qualityStr = GetOption("model_quality", "medium");
        
        if (qualityStr == "low") {
            return ModelQuality::Low;
        } else if (qualityStr == "high") {
            return ModelQuality::High;
        } else {
            return ModelQuality::Medium;
        }
    }
    
    // For compatibility - use HybridAISystem's OnlineMode
    typedef HybridAISystem::OnlineMode OnlineMode;
    
public:
    // Singleton instance
    static AIConfig* s_instance;
    
    // Member variables
    std::string m_dataPath;            // Path for AI data and models
    OperationMode m_operationMode;     // Current operation mode
    LearningMode m_learningMode;       // Current learning mode
    bool m_enableVulnerabilityScanner; // Whether to enable vulnerability scanner
    bool m_enableScriptGeneration;     // Whether to enable script generation
    bool m_enableCodeDebugging;        // Whether to enable code debugging
    bool m_enableUIAssistant;          // Whether to enable UI assistant
    bool m_debugLogging;               // Whether to enable debug logging
    uint64_t m_maxMemoryUsage;         // Maximum memory usage in bytes
    uint32_t m_maxHistoryItems;        // Maximum number of history items to save
    bool m_saveHistory;                // Whether to save conversation history
    
    // Options storage
    std::unordered_map<std::string, std::string> m_options;
    
    // Private constructor for singleton
    AIConfig();
    
    // Load config from file
    bool LoadConfig();
    
    // Save config to file
    bool SaveConfig();
    
    // Get config file path
    std::string GetConfigFilePath() const;
    
public:
    /**
     * @brief Get shared instance
     * @return Shared instance
     */
    static AIConfig& GetSharedInstance();
    
    /**
     * @brief Destructor
     */
    ~AIConfig();
    
    /**
     * @brief Initialize with default values
     * @return True if successful
     */
    bool Initialize();
    
    /**
     * @brief Check if initialized
     * @return True if initialized
     */
    bool IsInitialized() const { return !m_dataPath.empty(); }
    
    /**
     * @brief Set API key
     * @param apiKey API key
     */
    void SetAPIKey(const std::string& apiKey) { SetOption("api_key", apiKey); }
    
    /**
     * @brief Get API key
     * @return API key
     */
    std::string GetAPIKey() const { return GetOption("api_key"); }
    
    /**
     * @brief Set API endpoint
     * @param endpoint API endpoint
     */
    void SetAPIEndpoint(const std::string& endpoint) { SetOption("api_endpoint", endpoint); }
    
    /**
     * @brief Get API endpoint
     * @return API endpoint
     */
    std::string GetAPIEndpoint() const { return GetOption("api_endpoint"); }
    
    /**
     * @brief Set whether to encrypt communication
     * @param encrypt Whether to encrypt
     */
    void SetEncryptCommunication(bool encrypt) { SetOption("encrypt_communication", encrypt ? "1" : "0"); }
    
    /**
     * @brief Get whether to encrypt communication
     * @return Whether to encrypt
     */
    bool GetEncryptCommunication() const { return GetOption("encrypt_communication", "1") == "1"; }
    
    /**
     * @brief Set model path
     * @param path Model path
     */
    void SetModelPath(const std::string& path) { SetOption("model_path", path); }
    
    /**
     * @brief Get model path
     * @return Model path
     */
    std::string GetModelPath() const { return GetOption("model_path"); }
    
    /**
     * @brief Set online mode
     * @param mode Online mode
     */
    void SetOnlineMode(OnlineMode mode);
    
    /**
     * @brief Get online mode
     * @return Online mode
     */
    OnlineMode GetOnlineMode() const;
    
    /**
     * @brief Set data path
     * @param path Data path
     */
    void SetDataPath(const std::string& path);
    
    /**
     * @brief Get data path
     * @return Data path
     */
    std::string GetDataPath() const;
    
    /**
     * @brief Set operation mode
     * @param mode Operation mode
     */
    void SetOperationMode(OperationMode mode);
    
    /**
     * @brief Get operation mode
     * @return Operation mode
     */
    OperationMode GetOperationMode() const;
    
    /**
     * @brief Set learning mode
     * @param mode Learning mode
     */
    void SetLearningMode(LearningMode mode);
    
    /**
     * @brief Get learning mode
     * @return Learning mode
     */
    LearningMode GetLearningMode() const;
    
    /**
     * @brief Set whether to enable vulnerability scanner
     * @param enable Whether to enable
     */
    void SetEnableVulnerabilityScanner(bool enable);
    
    /**
     * @brief Get whether vulnerability scanner is enabled
     * @return Whether enabled
     */
    bool GetEnableVulnerabilityScanner() const;
    
    /**
     * @brief Set whether to enable script generation
     * @param enable Whether to enable
     */
    void SetEnableScriptGeneration(bool enable);
    
    /**
     * @brief Get whether script generation is enabled
     * @return Whether enabled
     */
    bool GetEnableScriptGeneration() const;
    
    /**
     * @brief Set whether to enable code debugging
     * @param enable Whether to enable
     */
    void SetEnableCodeDebugging(bool enable);
    
    /**
     * @brief Get whether code debugging is enabled
     * @return Whether enabled
     */
    bool GetEnableCodeDebugging() const;
    
    /**
     * @brief Set whether to enable UI assistant
     * @param enable Whether to enable
     */
    void SetEnableUIAssistant(bool enable);
    
    /**
     * @brief Get whether UI assistant is enabled
     * @return Whether enabled
     */
    bool GetEnableUIAssistant() const;
    
    /**
     * @brief Set whether to enable debug logging
     * @param enable Whether to enable
     */
    void SetDebugLogging(bool enable);
    
    /**
     * @brief Get whether debug logging is enabled
     * @return Whether enabled
     */
    bool GetDebugLogging() const;
    
    /**
     * @brief Set maximum memory usage
     * @param max Maximum memory in bytes
     */
    void SetMaxMemoryUsage(uint64_t max);
    
    /**
     * @brief Get maximum memory usage
     * @return Maximum memory in bytes
     */
    uint64_t GetMaxMemoryUsage() const;
    
    /**
     * @brief Set maximum history items
     * @param max Maximum items
     */
    void SetMaxHistoryItems(uint32_t max);
    
    /**
     * @brief Get maximum history items
     * @return Maximum items
     */
    uint32_t GetMaxHistoryItems() const;
    
    /**
     * @brief Set whether to save history
     * @param save Whether to save
     */
    void SetSaveHistory(bool save);
    
    /**
     * @brief Get whether to save history
     * @return Whether to save
     */
    bool GetSaveHistory() const;
    
    /**
     * @brief Set custom option
     * @param key Option key
     * @param value Option value
     */
    void SetOption(const std::string& key, const std::string& value);
    
    /**
     * @brief Get custom option
     * @param key Option key
     * @param defaultValue Default value
     * @return Option value
     */
    std::string GetOption(const std::string& key, const std::string& defaultValue = "") const;
    
    /**
     * @brief Reset all settings to defaults
     */
    void ResetToDefaults();
    
    /**
     * @brief Save changes
     * @return True if successful
     */
    bool Save();
    
    /**
     * @brief Auto-detect optimal settings for device
     */
    void AutoDetectOptimalSettings();
    
    /**
     * @brief Convert operation mode to string
     * @param mode Operation mode
     * @return String representation
     */
    static std::string OperationModeToString(OperationMode mode);
    
    /**
     * @brief Convert string to operation mode
     * @param str String representation
     * @return Operation mode
     */
    static OperationMode StringToOperationMode(const std::string& str);
    
    /**
     * @brief Convert learning mode to string
     * @param mode Learning mode
     * @return String representation
     */
    static std::string LearningModeToString(LearningMode mode);
    
    /**
     * @brief Convert string to learning mode
     * @param str String representation
     * @return Learning mode
     */
    static LearningMode StringToLearningMode(const std::string& str);
};

} // namespace AIFeatures
} // namespace iOS

    // Model improvement enum
    enum class ModelImprovement {
        None,           // No improvement
        Local,          // Local improvement only
        Cloud,          // Cloud-based improvement
        Hybrid          // Hybrid local and cloud
    };
    
    /**
     * @brief Set model improvement
     * @param improvement Model improvement setting
     */
    void SetModelImprovement(ModelImprovement improvement) {
        std::string improvementStr;
        switch (improvement) {
            case ModelImprovement::None:
                improvementStr = "none";
                break;
            case ModelImprovement::Local:
                improvementStr = "local";
                break;
            case ModelImprovement::Cloud:
                improvementStr = "cloud";
                break;
            case ModelImprovement::Hybrid:
                improvementStr = "hybrid";
                break;
            default:
                improvementStr = "local";
                break;
        }
        SetOption("model_improvement", improvementStr);
    }
    
    /**
     * @brief Get model improvement
     * @return Model improvement setting
     */
    ModelImprovement GetModelImprovement() const {
        std::string improvementStr = GetOption("model_improvement", "local");
        
        if (improvementStr == "none") {
            return ModelImprovement::None;
        } else if (improvementStr == "cloud") {
            return ModelImprovement::Cloud;
        } else if (improvementStr == "hybrid") {
            return ModelImprovement::Hybrid;
        } else {
            return ModelImprovement::Local;
        }
    }
    
    // Vulnerability detection level enum
    enum class DetectionLevel {
        Basic,          // Basic detection
        Standard,       // Standard detection
        Thorough,       // Thorough detection
        Exhaustive      // Exhaustive detection
    };
    
    /**
     * @brief Set vulnerability detection level
     * @param level Detection level
     */
    void SetVulnerabilityDetectionLevel(DetectionLevel level) {
        std::string levelStr;
        switch (level) {
            case DetectionLevel::Basic:
                levelStr = "basic";
                break;
            case DetectionLevel::Standard:
                levelStr = "standard";
                break;
            case DetectionLevel::Thorough:
                levelStr = "thorough";
                break;
            case DetectionLevel::Exhaustive:
                levelStr = "exhaustive";
                break;
            default:
                levelStr = "standard";
                break;
        }
        SetOption("vulnerability_detection_level", levelStr);
    }
    
    /**
     * @brief Get vulnerability detection level
     * @return Detection level
     */
    DetectionLevel GetVulnerabilityDetectionLevel() const {
        std::string levelStr = GetOption("vulnerability_detection_level", "standard");
        
        if (levelStr == "basic") {
            return DetectionLevel::Basic;
        } else if (levelStr == "thorough") {
            return DetectionLevel::Thorough;
        } else if (levelStr == "exhaustive") {
            return DetectionLevel::Exhaustive;
        } else {
            return DetectionLevel::Standard;
        }
    }
