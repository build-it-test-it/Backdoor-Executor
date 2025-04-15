#define CI_BUILD
#include "../ios_compat.h"
#pragma once

#include "AIConfig.h"
#include "AIIntegration.h"
#include "local_models/VulnerabilityDetectionModel.h"
#include "local_models/ScriptGenerationModel.h"
#include "SelfModifyingCodeSystem.h"

#include <string>
#include <memory>
#include <mutex>
#include <thread>
#include <atomic>
#include <functional>
#include <unordered_map>

namespace iOS {
namespace AIFeatures {

/**
 * @class AISystemInitializer
 * @brief Initializes and manages the AI system lifecycle
 * 
 * This class handles the initialization of the AI system on first use,
 * ensures models are created locally, provides fallback systems during 
 * training, and coordinates continuous self-improvement. It ensures
 * the AI system works completely offline without any cloud dependencies.
 */
class AISystemInitializer {
public:
    // Initialization state enumeration
    enum class InitState {
        NotStarted,    // Initialization not started
        InProgress,    // Initialization in progress
        Completed,     // Initialization completed
        Failed         // Initialization failed
    };
    
    // Training priority enumeration
    enum class TrainingPriority {
        Low,
        Medium,
        High,
        Critical
    };
    
    // Fallback strategy enumeration
    enum class FallbackStrategy {
        BasicPatterns,     // Use basic patterns only
        CachedResults,     // Use cached results
        PatternMatching,   // Use simple pattern matching
        RuleBased,         // Use rule-based approach
        HybridApproach     // Combine multiple strategies
    };
    
    // Training request structure
    struct TrainingRequest {
        std::string m_modelName;
        std::string m_dataPath;
        TrainingPriority m_priority;
        bool m_forceRetrain;
        std::function<void(float, float)> m_progressCallback;
        
        TrainingRequest()
            : m_priority(TrainingPriority::Medium), m_forceRetrain(false) {}
    };
    
    // Model status structure
    struct ModelStatus {
        std::string m_name;
        std::string m_version;
        InitState m_state;
        float m_trainingProgress;
        float m_accuracy;
        uint64_t m_lastTrainingTime;
        uint64_t m_lastUsedTime;
        
        ModelStatus()
            : m_state(InitState::NotStarted), m_trainingProgress(0.0f),
              m_accuracy(0.0f), m_lastTrainingTime(0), m_lastUsedTime(0) {}
    };
    
private:
    // Singleton instance
    static std::unique_ptr<AISystemInitializer> s_instance;
    static std::mutex s_instanceMutex;
    
    // Mutex for thread safety
    std::mutex m_mutex;
    
    // Initialization state
    InitState m_initState;
    
    // Models
    std::shared_ptr<LocalModels::VulnerabilityDetectionModel> m_vulnDetectionModel;
    std::shared_ptr<LocalModels::ScriptGenerationModel> m_scriptGenModel;
    
    // Self-modifying code system
    std::shared_ptr<SelfModifyingCodeSystem> m_selfModifyingSystem;
    
    // AI configuration
    std::shared_ptr<AIConfig> m_config;
    
    // Data paths
    std::string m_dataRootPath;
    std::string m_modelDataPath;
    std::string m_trainingDataPath;
    std::string m_cacheDataPath;
    
    // Training thread
    std::thread m_trainingThread;
    std::atomic<bool> m_trainingThreadRunning;
    std::mutex m_trainingQueueMutex;
    std::vector<TrainingRequest> m_trainingQueue;
    
    // Model status
    std::unordered_map<std::string, ModelStatus> m_modelStatus;
    
    // Fallback systems
    FallbackStrategy m_currentFallbackStrategy;
    std::unordered_map<std::string, std::string> m_fallbackPatterns;
    std::unordered_map<std::string, std::string> m_cachedResults;
    
    // Usage statistics
    uint64_t m_vulnDetectionCount;
    uint64_t m_scriptGenCount;
    uint64_t m_fallbackUsageCount;
    
    // Internal methods
    bool InitializeDataPaths();
    bool InitializeModels();
    bool InitializeSelfModifyingSystem();
    bool InitializeFallbackSystems();
    
    void TrainingThreadFunc();
    bool TrainModel(const TrainingRequest& request);
    void UpdateModelStatus(const std::string& modelName, InitState state, float progress, float accuracy);
    
    std::string GetFallbackVulnerabilityDetectionResult(const std::string& script);
    std::string GetFallbackScriptGenerationResult(const std::string& description);
    
    // Constructor (private for singleton)
    AISystemInitializer();
    
public:
    /**
     * @brief Destructor
     */
    ~AISystemInitializer();
    
    /**
     * @brief Get singleton instance
     * @return Reference to singleton instance
     */
    static AISystemInitializer& GetInstance();
    
    /**
     * @brief Initialize AI system
     * @param dataRootPath Root path for AI data
     * @param config AI configuration
     * @return True if initialization succeeded or is in progress
     */
    bool Initialize(const std::string& dataRootPath, std::shared_ptr<AIConfig> config);
    
    /**
     * @brief Get initialization state
     * @return Current initialization state
     */
    InitState GetInitState() const;
    
    /**
     * @brief Get vulnerability detection model
     * @return Shared pointer to vulnerability detection model
     */
    std::shared_ptr<LocalModels::VulnerabilityDetectionModel> GetVulnerabilityDetectionModel();
    
    /**
     * @brief Get script generation model
     * @return Shared pointer to script generation model
     */
    std::shared_ptr<LocalModels::ScriptGenerationModel> GetScriptGenerationModel();
    
    /**
     * @brief Get self-modifying code system
     * @return Shared pointer to self-modifying code system
     */
    std::shared_ptr<SelfModifyingCodeSystem> GetSelfModifyingSystem();
    
    /**
     * @brief Detect vulnerabilities in script
     * This will use the trained model if available, or fall back to basic detection
     * @param script Script to analyze
     * @param gameType Type of game (for context)
     * @param isServerScript Whether this is a server script (for context)
     * @return JSON string with detected vulnerabilities
     */
    std::string DetectVulnerabilities(const std::string& script, 
                                     const std::string& gameType = "Generic",
                                     bool isServerScript = false);
    
    /**
     * @brief Generate script from description
     * This will use the trained model if available, or fall back to templates
     * @param description Script description
     * @param gameType Type of game (for context)
     * @param isServerScript Whether this is a server script (for context)
     * @return Generated script
     */
    std::string GenerateScript(const std::string& description,
                              const std::string& gameType = "Generic",
                              bool isServerScript = false);
    
    /**
     * @brief Request model training
     * @param modelName Name of model to train
     * @param priority Training priority
     * @param forceRetrain Force retraining even if model is already trained
     * @param progressCallback Callback for training progress
     * @return True if training request was queued
     */
    bool RequestTraining(const std::string& modelName,
                        TrainingPriority priority = TrainingPriority::Medium,
                        bool forceRetrain = false,
                        std::function<void(float, float)> progressCallback = nullptr);
    
    /**
     * @brief Get model status
     * @param modelName Name of model
     * @return Model status
     */
    ModelStatus GetModelStatus(const std::string& modelName) const;
    
    /**
     * @brief Get all model statuses
     * @return Map of model names to statuses
     */
    std::unordered_map<std::string, ModelStatus> GetAllModelStatuses() const;
    
    /**
     * @brief Set fallback strategy
     * @param strategy Fallback strategy
     */
    void SetFallbackStrategy(FallbackStrategy strategy);
    
    /**
     * @brief Get fallback strategy
     * @return Current fallback strategy
     */
    FallbackStrategy GetFallbackStrategy() const;
    
    /**
     * @brief Force self-improvement cycle
     * @return True if improvement succeeded
     */
    bool ForceSelfImprovement();
    
    /**
     * @brief Add training data for vulnerability detection
     * @param script Script
     * @param vulnerabilities JSON string with vulnerabilities
     * @return True if data was added
     */
    bool AddVulnerabilityTrainingData(const std::string& script, const std::string& vulnerabilities);
    
    /**
     * @brief Add training data for script generation
     * @param description Script description
     * @param script Generated script
     * @param rating Rating (0-1)
     * @return True if data was added
     */
    bool AddScriptGenerationTrainingData(const std::string& description, 
                                        const std::string& script,
                                        float rating);
    
    /**
     * @brief Provide feedback on vulnerability detection
     * @param script Script
     * @param detectionResult Detection result
     * @param correctDetections Map of detection index to correctness
     * @return True if feedback was processed
     */
    bool ProvideVulnerabilityFeedback(const std::string& script,
                                     const std::string& detectionResult,
                                     const std::unordered_map<int, bool>& correctDetections);
    
    /**
     * @brief Provide feedback on script generation
     * @param description Script description
     * @param generatedScript Generated script
     * @param userScript User-modified script
     * @param rating Rating (0-1)
     * @return True if feedback was processed
     */
    bool ProvideScriptGenerationFeedback(const std::string& description,
                                        const std::string& generatedScript,
                                        const std::string& userScript,
                                        float rating);
    
    /**
     * @brief Check if models are ready for use
     * @return True if models are ready
     */
    bool AreModelsReady() const;
    
    /**
     * @brief Get a report on system status
     * @return JSON string with system status
     */
    std::string GetSystemStatusReport() const;
    
    /**
     * @brief Check if system is in fallback mode
     * @return True if in fallback mode
     */
    bool IsInFallbackMode() const;
    
    /**
     * @brief Resume training thread if paused
     * @return True if resumed
     */
    bool ResumeTraining();
    
    /**
     * @brief Pause training thread
     * @return True if paused
     */
    bool PauseTraining();
};

} // namespace AIFeatures
} // namespace iOS
