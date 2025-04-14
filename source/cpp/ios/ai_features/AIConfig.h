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
 * default values for both online and offline operation.
 */
class AIConfig {
public:
    // Online mode enum
    enum class OnlineMode {
        Auto,           // Automatically use online when available, fallback to offline
        PreferOffline,  // Prefer offline, use online only when offline fails
        PreferOnline,   // Prefer online, use offline only when online fails
        OfflineOnly,    // Always use offline mode
        OnlineOnly      // Always use online mode (will fail if no connectivity)
    };
    
    // AI model quality enum
    enum class ModelQuality {
        Low,            // Minimal models (lowest memory usage)
        Medium,         // Standard models (balanced)
        High            // Full-size models (highest quality, more memory)
    };
    
private:
    // Singleton instance
    static AIConfig* s_instance;
    
    // Member variables
    std::string m_apiEndpoint;        // API endpoint for online AI
    std::string m_apiKey;             // API key for online AI
    OnlineMode m_onlineMode;          // Current online mode
    ModelQuality m_modelQuality;      // Model quality setting
    bool m_encryptCommunication;      // Whether to encrypt API communication
    bool m_shareUsageData;            // Whether to share usage data for improvements
    bool m_saveHistory;               // Whether to save conversation history
    uint32_t m_maxHistoryItems;       // Maximum number of history items to save
    bool m_useGPT4;                   // Whether to use GPT-4 (if available)
    bool m_debugLogging;              // Whether to enable debug logging
    std::string m_modelPath;          // Path to model files
    uint64_t m_maxMemoryUsage;        // Maximum memory usage in bytes
    bool m_showNetworkIndicator;      // Whether to show network activity indicator
    bool m_autoUpdate;                // Whether to auto-update models
    
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
     * @brief Set API endpoint
     * @param endpoint API endpoint
     */
    void SetAPIEndpoint(const std::string& endpoint);
    
    /**
     * @brief Get API endpoint
     * @return API endpoint
     */
    std::string GetAPIEndpoint() const;
    
    /**
     * @brief Set API key
     * @param apiKey API key
     */
    void SetAPIKey(const std::string& apiKey);
    
    /**
     * @brief Get API key
     * @return API key
     */
    std::string GetAPIKey() const;
    
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
     * @brief Set model quality
     * @param quality Model quality
     */
    void SetModelQuality(ModelQuality quality);
    
    /**
     * @brief Get model quality
     * @return Model quality
     */
    ModelQuality GetModelQuality() const;
    
    /**
     * @brief Set whether to encrypt communication
     * @param encrypt Whether to encrypt
     */
    void SetEncryptCommunication(bool encrypt);
    
    /**
     * @brief Get whether to encrypt communication
     * @return Whether to encrypt
     */
    bool GetEncryptCommunication() const;
    
    /**
     * @brief Set whether to share usage data
     * @param share Whether to share
     */
    void SetShareUsageData(bool share);
    
    /**
     * @brief Get whether to share usage data
     * @return Whether to share
     */
    bool GetShareUsageData() const;
    
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
     * @brief Set whether to use GPT-4
     * @param use Whether to use
     */
    void SetUseGPT4(bool use);
    
    /**
     * @brief Get whether to use GPT-4
     * @return Whether to use
     */
    bool GetUseGPT4() const;
    
    /**
     * @brief Set whether to enable debug logging
     * @param enable Whether to enable
     */
    void SetDebugLogging(bool enable);
    
    /**
     * @brief Get whether to enable debug logging
     * @return Whether to enable
     */
    bool GetDebugLogging() const;
    
    /**
     * @brief Set model path
     * @param path Model path
     */
    void SetModelPath(const std::string& path);
    
    /**
     * @brief Get model path
     * @return Model path
     */
    std::string GetModelPath() const;
    
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
     * @brief Set whether to show network indicator
     * @param show Whether to show
     */
    void SetShowNetworkIndicator(bool show);
    
    /**
     * @brief Get whether to show network indicator
     * @return Whether to show
     */
    bool GetShowNetworkIndicator() const;
    
    /**
     * @brief Set whether to auto-update models
     * @param autoUpdate Whether to auto-update
     */
    void SetAutoUpdate(bool autoUpdate);
    
    /**
     * @brief Get whether to auto-update models
     * @return Whether to auto-update
     */
    bool GetAutoUpdate() const;
    
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
     * @brief Get appropriate model filename for current quality setting
     * @param baseModelName Base model name
     * @return Full model filename
     */
    std::string GetModelFilename(const std::string& baseModelName) const;
    
    /**
     * @brief Check if device supports high quality models
     * @return True if supported
     */
    bool DeviceSupportsHighQualityModels() const;
    
    /**
     * @brief Auto-detect optimal settings for device
     */
    void AutoDetectOptimalSettings();
    
    /**
     * @brief Convert online mode to string
     * @param mode Online mode
     * @return String representation
     */
    static std::string OnlineModeToString(OnlineMode mode);
    
    /**
     * @brief Convert string to online mode
     * @param str String representation
     * @return Online mode
     */
    static OnlineMode StringToOnlineMode(const std::string& str);
    
    /**
     * @brief Convert model quality to string
     * @param quality Model quality
     * @return String representation
     */
    static std::string ModelQualityToString(ModelQuality quality);
    
    /**
     * @brief Convert string to model quality
     * @param str String representation
     * @return Model quality
     */
    static ModelQuality StringToModelQuality(const std::string& str);
};

} // namespace AIFeatures
} // namespace iOS
