#pragma once

#include "../../objc_isolation.h"
#include "AIConfig.h"
#include "AIIntegration.h"
#include "local_models/VulnerabilityDetectionModel.h"
#include "local_models/ScriptGenerationModel.h"
#include "local_models/GeneralAssistantModel.h"
#include "SelfModifyingCodeSystem.h"

#include <string>
#include <memory>
#include <mutex>
#include <functional>
#include <vector>
#include <map>
#include <chrono>

namespace iOS {
namespace AIFeatures {

/**
 * @class AISystemInitializer
 * @brief Initializes and manages the AI system components
 * 
 * This class is responsible for initializing and managing all AI system
 * components, including models, script assistants, and other functionality.
 * It provides a unified interface for the rest of the application to access
 * AI features.
 */
class AISystemInitializer {
public:
    // Initialization state enum
    enum class InitState {
        NotInitialized,
        Initializing,
        Initialized,
        Error
    };
    
    // Return initialization status
    bool IsInitialized() const;
    bool IsInitializing() const;
    InitState GetInitState() const;
    
private:
    // Singleton instance
    static std::unique_ptr<AISystemInitializer> s_instance;
    static std::mutex s_instanceMutex;
    
    // Configuration
    ::iOS::AIFeatures::AIConfig m_config;
    std::string m_dataPath;
    std::string m_modelDataPath;
    
    // Initialization state
    InitState m_initState;
    
    // Mutex for thread safety
    mutable std::mutex m_mutex;
    
    // Models
    std::shared_ptr<::iOS::AIFeatures::LocalModels::VulnerabilityDetectionModel> m_vulnDetectionModel;
    std::shared_ptr<::iOS::AIFeatures::LocalModels::GeneralAssistantModel> m_generalAssistantModel;
    std::shared_ptr<::iOS::AIFeatures::LocalModels::ScriptGenerationModel> m_scriptGenModel;
    
    // Self-modifying code system
    std::shared_ptr<SelfModifyingCodeSystem> m_selfModifyingSystem;
    
    // Script assistant
    std::shared_ptr<::iOS::AIFeatures::ScriptAssistant> m_scriptAssistant;
    
    // Model statuses
    struct ModelStatus {
        bool initialized;
        bool loaded;
        int version;
        
        ModelStatus() : initialized(false), loaded(false), version(0) {}
    };
    
    std::map<std::string, ModelStatus> m_modelStatus;
    
    // Constructor/destructor
    AISystemInitializer();
    ~AISystemInitializer();
    
    // Private initialization methods
    bool InitializeModels();
    bool InitializeVulnerabilityDetection();
    bool InitializeScriptAssistant();
    
    // Load models from disk
    bool LoadModels();
    
public:
    /**
     * @brief Get singleton instance
     * @return Reference to singleton instance
     */
    static AISystemInitializer& GetInstance();
    
    /**
     * @brief Initialize the AI system
     * @param config AI configuration
     * @param progressCallback Progress callback
     * @return True if initialization succeeded or was already complete
     */
    bool Initialize(const ::iOS::AIFeatures::AIConfig& config, std::function<void(float)> progressCallback = nullptr);
    
    /**
     * @brief Get initialization progress
     * @return Progress value from 0.0 to 1.0
     */
    float GetInitializationProgress() const;
    
    /**
     * @brief Get configuration
     * @return Current configuration
     */
    const ::iOS::AIFeatures::AIConfig& GetConfig() const;
    
    /**
     * @brief Update configuration
     * @param config New configuration
     * @return True if update succeeded
     */
    bool UpdateConfig(const ::iOS::AIFeatures::AIConfig& config);
    
    /**
     * @brief Get model version
     * @param modelName Name of the model
     * @return Model version or 0 if not available
     */
    int GetModelVersion(const std::string& modelName) const;
    
    /**
     * @brief Get vulnerability detection model
     * @return Shared pointer to vulnerability detection model
     */
    std::shared_ptr<::iOS::AIFeatures::LocalModels::VulnerabilityDetectionModel> GetVulnerabilityDetectionModel();
    
    /**
     * @brief Get script generation model
     * @return Shared pointer to script generation model
     */
    std::shared_ptr<::iOS::AIFeatures::LocalModels::ScriptGenerationModel> GetScriptGenerationModel();
    
    /**
     * @brief Get general assistant model
     * @return Shared pointer to general assistant model
     */
    std::shared_ptr<::iOS::AIFeatures::LocalModels::GeneralAssistantModel> GetGeneralAssistantModel() const;
    
    /**
     * @brief Get script assistant
     * @return Shared pointer to script assistant
     */
    std::shared_ptr<::iOS::AIFeatures::ScriptAssistant> GetScriptAssistant();
    
    /**
     * @brief Detect vulnerabilities in a script
     * @param script Script to analyze
     * @param onComplete Callback for when detection completes
     */
    void DetectVulnerabilities(const std::string& script, std::function<void(const std::vector<::iOS::AIFeatures::LocalModels::VulnerabilityDetectionModel::Vulnerability>&)> onComplete);
    
    /**
     * @brief Generate a script
     * @param prompt User prompt
     * @param onComplete Callback for when generation completes
     */
    void GenerateScript(const std::string& prompt, std::function<void(const std::string&)> onComplete);
    
    /**
     * @brief Enhance a script
     * @param script Original script
     * @param prompt Enhancement instructions
     * @param onComplete Callback for when enhancement completes
     */
    void EnhanceScript(const std::string& script, const std::string& prompt, std::function<void(const std::string&)> onComplete);
    
    /**
     * @brief Train models
     * @param exampleData Training data
     * @param onComplete Callback for when training completes
     * @return True if training started successfully
     */
    bool TrainModels(const std::vector<std::string>& exampleData, std::function<void(bool)> onComplete);
    
    /**
     * @brief Get model improvement mode
     * @return Current model improvement mode
     */
    ::iOS::AIFeatures::AIConfig::ModelImprovement GetModelImprovementMode() const;
    
    /**
     * @brief Set model improvement mode
     * @param mode New mode
     */
    void SetModelImprovementMode(::iOS::AIFeatures::AIConfig::ModelImprovement mode);
    
    /**
     * @brief Clean up resources and prepare for shutdown
     */
    void Cleanup();
};

} // namespace AIFeatures
} // namespace iOS
