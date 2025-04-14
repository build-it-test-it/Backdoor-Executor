#pragma once

#include <string>
#include <unordered_map>
#include <functional>
#import <Foundation/Foundation.h>

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
    
private:
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
