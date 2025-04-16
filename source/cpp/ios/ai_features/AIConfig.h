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
    
    // Model improvement enum
    enum class ModelImprovement {
            return ModelImprovement::Local;
        }
    }
    
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
    
    /**
     * @brief Enable/disable cloud features
     * @param enabled Whether cloud features are enabled
     */
        return GetOption("cloud_enabled", "0") == "1";
    }
    
    /**
     * @brief Enable/disable offline model generation
     * @param enabled Whether offline generation is enabled
     */
    void SetOfflineModelGenerationEnabled(bool enabled) {
        SetOption("offline_model_generation", enabled ? "1" : "0");
    }
    
    /**
     * @brief Check if offline model generation is enabled
     * @return Whether offline generation is enabled
     */
    bool GetOfflineModelGenerationEnabled() const {
        return GetOption("offline_model_generation", "1") == "1";
    }
    
    /**
     * @brief Enable/disable continuous learning
     * @param enabled Whether continuous learning is enabled
     */
    void SetContinuousLearningEnabled(bool enabled) {
        SetOption("continuous_learning", enabled ? "1" : "0");
    }
    
    /**
     * @brief Check if continuous learning is enabled
     * @return Whether continuous learning is enabled
     */
    bool GetContinuousLearningEnabled() const {
        return GetOption("continuous_learning", "0") == "1";
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
    
public:
    /**
     * @brief Constructor 
     */
    explicit AIConfig();
    
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
     * @brief Set API endpoi# Let's make a copy of the AIConfig.h file
cp source/cpp/ios/ai_features/AIConfig.h /tmp/AIConfig.h.bak

# Let's create a clean version of AIConfig.h
cat > source/cpp/ios/ai_features/AIConfig.h << 'EOF'
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
    
    // Model improvement enum (local only, no cloud options)
    enum class ModelImprovement {
            return ModelImprovement::Local;
        }
    }
    
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
    
    /**
     * @brief Enable/disable offline model generation
     * @param enabled Whether offline generation is enabled
     */
    void SetOfflineModelGenerationEnabled(bool enabled) {
        SetOption("offline_model_generation", enabled ? "1" : "0");
    }
    
    /**
     * @brief Check if offline model generation is enabled
     * @return Whether offline generation is enabled
     */
    bool GetOfflineModelGenerationEnabled() const {
        return GetOption("offline_model_generation", "1") == "1";
    }
    
    /**
     * @brief Enable/disable continuous learning
     * @param enabled Whether continuous learning is enabled
     */
    void SetContinuousLearningEnabled(bool enabled) {
        SetOption("continuous_learning", enabled ? "1" : "0");
    }
    
    /**
     * @brief Check if continuous learning is enabled
     * @return Whether continuous learning is enabled
     */
    bool GetContinuousLearningEnabled() const {
        return GetOption("continuous_learning", "1") == "1";
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
    
public:
    /**
     * @brief Constructor 
     */
    explicit AIConfig();
    
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
    void SetOnlineMode(O# Let's fix the model headers first to ensure IsInitialized and SetModelPath are properly defined
sed -i '189s/
^
/    bool IsInitialized() const;\n    bool SetModelPath(const std::string& path);\n    /' source/cpp/ios/ai_features/local_models/ScriptGenerationModel.h 
sed -i '492s/
^
/    bool IsInitialized() const;\n    bool SetModelPath(const std::string& path);\n    /' source/cpp/ios/ai_features/local_models/VulnerabilityDetectionModel.h

# Now create a shorter version of AIConfig.h with just the ModelImprovement enum and related methods
cat > /tmp/model_improvement_fix.patch << 'EOF'
    // Model improvement enum (local only, no cloud options)
    enum class ModelImprovement {
            return ModelImprovement::Local;
        }
    }
