
#include "../objc_isolation.h"
#pragma once

#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <mutex>
#include <atomic>
#include <thread>
#include <queue>
#include <unordered_map>
#include <chrono>

// Forward declarations for local model types
namespace iOS {
namespace AIFeatures {
namespace LocalModels {
    class LocalModelBase;
}
}
}

namespace iOS {
namespace AIFeatures {

/**
 * @class SelfTrainingManager
 * @brief Manages the creation and training of local AI models
 * 
 * This class coordinates the training and improvement of local models,
 * handling scheduling, progress tracking, and model optimization.
 * It ensures all AI functionality operates completely locally without
 * any external API dependencies.
 */
class SelfTrainingManager {
public:
    // Training status enum
    enum class TrainingStatus {
        NotStarted,     // Training not started
        InProgress,     // Training in progress
        Completed,      // Training completed
        Failed          // Training failed
    };
    
    // Model initialization status enum
    enum class ModelInitStatus {
        NotInitialized, // Model not initialized
        Initializing,   // Model being initialized
        Ready,          // Model is ready
        Failed          // Model failed to initialize
    };
    
    // Callback for training progress updates
    using TrainingProgressCallback = std::function<void(const std::string&, float, bool)>;
    
private:
    // Member variables with m_ prefix convention
    std::string m_modelBasePath;               // Base path for model storage
    std::unordered_map<std::string, std::shared_ptr<LocalModels::LocalModelBase>> m_models; // Model map
    std::unordered_map<std::string, TrainingStatus> m_trainingStatus; // Training status by model name
    std::unordered_map<std::string, ModelInitStatus> m_modelStatus;   // Model status by model name
    std::unordered_map<std::string, float> m_trainingProgress;        // Training progress by model name
    std::atomic<bool> m_isTrainingActive;      // Flag to indicate if training is active
    std::mutex m_modelMutex;                   // Mutex for model access
    std::mutex m_queueMutex;                   // Mutex for queue access
    std::queue<std::function<void()>> m_trainingQueue; // Queue of training jobs
    std::thread m_trainingThread;              // Thread for background training
    std::atomic<bool> m_stopTrainingThread;    // Flag to stop training thread
    TrainingProgressCallback m_progressCallback; // Callback for training progress
    std::atomic<uint64_t> m_totalMemoryUsage;  // Total memory usage in bytes
    std::chrono::system_clock::time_point m_lastEvaluation; // Last model evaluation time
    
    // Base templates for each model type
    std::unordered_map<std::string, std::string> m_baseTemplates;
    
    // Training data samples
    std::unordered_map<std::string, std::vector<std::pair<std::string, std::string>>> m_trainingSamples;
    
    // Private methods
    void TrainingThreadFunction();
    void ProcessTrainingQueue();
    bool CreateModelDirectories();
    bool LoadBaseTemplates();
    void SaveTrainingProgress(const std::string& modelName, float progress);
    void SaveTrainingStatus(const std::string& modelName, TrainingStatus status);
    void NotifyProgressUpdate(const std::string& modelName, float progress, bool completed);
    bool EvaluateModel(const std::string& modelName);
    std::vector<std::pair<std::string, std::string>> GenerateBaseSamples(const std::string& modelType);
    
public:
    /**
     * @brief Constructor
     */
    SelfTrainingManager();
    
    /**
     * @brief Destructor
     */
    ~SelfTrainingManager();
    
    /**
     * @brief Initialize the training manager
     * @param basePath Base path for model storage
     * @return True if initialization succeeded
     */
    bool Initialize(const std::string& basePath);
    
    /**
     * @brief Add a model to be managed
     * @param modelName Name of the model
     * @param model Model pointer
     * @return True if model was added successfully
     */
    bool AddModel(const std::string& modelName, std::shared_ptr<LocalModels::LocalModelBase> model);
    
    /**
     * @brief Check if a model exists
     * @param modelName Name of the model
     * @return True if model exists
     */
    bool HasModel(const std::string& modelName) const;
    
    /**
     * @brief Get a model by name
     * @param modelName Name of the model
     * @return Model pointer or nullptr if not found
     */
    std::shared_ptr<LocalModels::LocalModelBase> GetModel(const std::string& modelName);
    
    /**
     * @brief Start the training thread
     * @return True if thread was started successfully
     */
    bool StartTrainingThread();
    
    /**
     * @brief Stop the training thread
     */
    void StopTrainingThread();
    
    /**
     * @brief Queue a model for training
     * @param modelName Name of the model to train
     * @return True if model was queued successfully
     */
    bool QueueModelForTraining(const std::string& modelName);
    
    /**
     * @brief Queue all models for training
     * @return Number of models queued
     */
    size_t QueueAllModelsForTraining();
    
    /**
     * @brief Add a training sample
     * @param modelName Name of the model
     * @param input Input data
     * @param output Expected output
     * @return True if sample was added successfully
     */
    bool AddTrainingSample(const std::string& modelName, const std::string& input, const std::string& output);
    
    /**
     * @brief Generate a new model
     * @param modelType Type of model to generate ("script", "debug", "vulnerability", "ui")
     * @return True if model generation was initiated successfully
     */
    bool GenerateModel(const std::string& modelType);
    
    /**
     * @brief Get training status
     * @param modelName Name of the model
     * @return Training status
     */
    TrainingStatus GetTrainingStatus(const std::string& modelName) const;
    
    /**
     * @brief Get model initialization status
     * @param modelName Name of the model
     * @return Model initialization status
     */
    ModelInitStatus GetModelStatus(const std::string& modelName) const;
    
    /**
     * @brief Get training progress
     * @param modelName Name of the model
     * @return Training progress (0.0-1.0)
     */
    float GetTrainingProgress(const std::string& modelName) const;
    
    /**
     * @brief Set training progress callback
     * @param callback Function to call with training progress
     */
    void SetProgressCallback(TrainingProgressCallback callback);
    
    /**
     * @brief Get all model names
     * @return Vector of model names
     */
    std::vector<std::string> GetAllModelNames() const;
    
    /**
     * @brief Get total memory usage
     * @return Memory usage in bytes
     */
    uint64_t GetTotalMemoryUsage() const;
    
    /**
     * @brief Prune models to reduce memory usage
     * @param targetUsage Target memory usage in bytes
     * @return Amount of memory freed in bytes
     */
    uint64_t PruneModels(uint64_t targetUsage);
    
    /**
     * @brief Get number of training samples
     * @param modelName Name of the model
     * @return Number of training samples
     */
    size_t GetTrainingSampleCount(const std::string& modelName) const;
    
    /**
     * @brief Schedule automatic training
     * @param intervalHours Hours between training sessions
     * @return True if scheduled successfully
     */
    bool ScheduleAutomaticTraining(uint32_t intervalHours);
    
    /**
     * @brief Generate base training samples for all models
     * @return Number of samples generated
     */
    size_t GenerateBaseTrainingSamples();
};

} // namespace AIFeatures
} // namespace iOS
