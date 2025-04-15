#include "../ios_compat.h"
#define CI_BUILD

#pragma once

#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <unordered_map>

namespace iOS {
namespace AIFeatures {

/**
 * @class OfflineService
 * @brief Service for handling locally-trained AI capabilities
 * 
 * This class provides methods for working with locally-trained AI models,
 * managing training data, and coordinating between different AI components
 * without requiring network connectivity.
 */
class OfflineService {
public:
    // Training data entry
    struct TrainingData {
        std::string m_input;          // Input text/query
        std::string m_output;         // Expected output
        std::string m_context;        // Additional context
        std::string m_type;           // Data type (e.g., "script", "explanation")
        std::string m_source;         // Source (e.g., "user_feedback", "execution_result")
        uint64_t m_timestamp;         // Timestamp
        
        TrainingData() : m_timestamp(0) {}
    };
    
    // Model update result
    struct UpdateResult {
        bool m_success;               // Success flag
        std::string m_modelName;      // Model name
        std::string m_message;        // Status message
        float m_improvement;          // Performance improvement metric
        uint64_t m_durationMs;        // Duration in milliseconds
        
    };
    
    // Inference request
    struct Request {
        std::string m_input;          // Input text/query
        std::string m_context;        // Additional context
        std::string m_type;           // Request type (e.g., "script_generation", "debug")
        std::unordered_map<std::string, std::string> m_parameters; // Additional parameters
        bool m_collectStats;          // Whether to collect performance statistics
        
        Request() : m_collectStats(false) {}
    };
    
    // Inference response
    struct Response {
        bool m_success;               // Success flag
        std::string m_output;         // Output text
        std::unordered_map<std::string, std::string> m_metadata; // Response metadata
        uint64_t m_durationMs;        // Processing time in milliseconds
        
    };
    
    // Data collection settings
    struct DataCollectionSettings {
        bool m_enabled;               // Whether data collection is enabled
        bool m_collectUserQueries;    // Collect user queries
        bool m_collectExecutionResults; // Collect script execution results
        bool m_collectGameAnalysis;   // Collect game analysis data
        uint32_t m_maxSamplesPerDay;  // Maximum samples to collect per day
        
        DataCollectionSettings() 
            : m_enabled(true),
              m_collectUserQueries(true),
              m_collectExecutionResults(true),
              m_collectGameAnalysis(true),
              m_maxSamplesPerDay(100) {}
    };
    
    // Callback types
    using ResponseCallback = std::function<void(const Response&)>;
    using UpdateCallback = std::function<void(const UpdateResult&)>;
    using TrainingProgressCallback = std::function<void(float progress, const std::string& status)>;
    
private:
    // Member variables
    bool m_isInitialized;             // Initialization flag
    std::string m_dataPath;           // Path to training data
    DataCollectionSettings m_dataCollectionSettings; // Data collection settings
    std::vector<TrainingData> m_trainingBuffer; // Training data buffer
    std::unordered_map<std::string, std::string> m_modelVersions; // Model versions
    uint64_t m_requestCount;          // Request counter
    uint64_t m_lastTrainingTimestamp; // Timestamp of last training
    bool m_isTraining;                // Whether training is in progress
    
    // Private methods
    bool LoadTrainingData();
    bool SaveTrainingData();
    void ProcessTrainingBuffer();
    std::string GetModelPath(const std::string& modelName) const;
    std::string GetDataPath(const std::string& dataType) const;
    void UpdateModelVersion(const std::string& modelName);
    bool ValidateTrainingData(const TrainingData& data) const;
    void* SelectModelForRequest(const Request& request);
    
public:
    /**
     * @brief Constructor
     */
    OfflineService();
    
    /**
     * @brief Destructor
     */
    ~OfflineService();
    
    /**
     * @brief Initialize the service
     * @param dataPath Path to training data
     * @return True if initialization succeeded, false otherwise
     */
    bool Initialize(const std::string& modelPath, const std::string& dataPath);
    
    /**
     * @brief Check if the service is initialized
     * @return True if initialized, false otherwise
     */
    bool IsInitialized() const;
    
    /**
     * @brief Process an inference request
     * @param request Request to process
     * @param callback Function to call with the response
     */
    void ProcessRequest(const Request& request, ResponseCallback callback);
    
    /**
     * @brief Process an inference request synchronously
     * @param request Request to process
     * @return Response
     */
    Response ProcessRequestSync(const Request& request);
    
    /**
     * @brief Add training data
     * @param data Training data to add
     * @return True if data was added, false otherwise
     */
    bool AddTrainingData(const TrainingData& data);
    
    /**
     * @brief Add multiple training data entries
     * @param data Training data to add
     * @return Number of entries added
     */
    uint32_t AddTrainingData(const std::vector<TrainingData>& data);
    
    /**
     * @brief Start model update/training
     * @param modelName Model name (empty for all models)
     * @param callback Function to call with the update result
     * @param progressCallback Function to call with progress updates
     * @return True if update was started, false otherwise
     */
    bool StartModelUpdate(const std::string& modelName = "", 
                        UpdateCallback callback = nullptr,
                        TrainingProgressCallback progressCallback = nullptr);
    
    /**
     * @brief Cancel model update/training
     * @return True if update was cancelled, false otherwise
     */
    bool CancelModelUpdate();
    
    /**
     * @brief Check if model update/training is in progress
     * @return True if update is in progress, false otherwise
     */
    bool IsModelUpdateInProgress() const;
    
    /**
     * @brief Get available models
     * @return List of available model names
     */
    std::vector<std::string> GetAvailableModels() const;
    
    /**
     * @brief Get model version
     * @param modelName Model name
     * @return Model version
     */
    std::string GetModelVersion(const std::string& modelName) const;
    
    /**
     * @brief Get training data count
     * @param dataType Data type (empty for all types)
     * @return Number of training data entries
     */
    uint32_t GetTrainingDataCount(const std::string& dataType = "") const;
    
    /**
     * @brief Clear training data
     * @param dataType Data type (empty for all types)
     * @return Number of entries cleared
     */
    uint32_t ClearTrainingData(const std::string& dataType = "");
    
    /**
     * @brief Set data collection settings
     * @param settings Data collection settings
     */
    void SetDataCollectionSettings(const DataCollectionSettings& settings);
    
    /**
     * @brief Get data collection settings
     * @return Data collection settings
     */
    DataCollectionSettings GetDataCollectionSettings() const;
    
    /**
     * @brief Create a script generation request
     * @param description Script description
     * @param context Additional context
     * @return Request object ready to process
     */
    Request CreateScriptGenerationRequest(const std::string& description, const std::string& context = "");
    
    /**
     * @brief Create a script debugging request
     * @param script Script to debug
     * @return Request object ready to process
     */
    Request CreateScriptDebuggingRequest(const std::string& script);
    
    /**
     * @brief Create a game analysis request
     * @param gameData Game data
     * @return Request object ready to process
     */
    Request CreateGameAnalysisRequest(const std::string& gameData);
    
    /**
     * @brief Create a general query request
     * @param query User query
     * @param context Additional context
     * @return Request object ready to process
     */
    Request CreateGeneralQueryRequest(const std::string& query, const std::string& context = "");
};

} // namespace AIFeatures
} // namespace iOS
