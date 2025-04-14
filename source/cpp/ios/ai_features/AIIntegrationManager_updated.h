#pragma once

#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <unordered_map>
#include "ScriptAssistant.h"
#include "SignatureAdaptation.h"
#include "HybridAISystem.h"
#include "AIConfig.h"
#include "OfflineService.h"

namespace iOS {
namespace AIFeatures {

/**
 * @class AIIntegrationManager
 * @brief Manages the integration of all AI components
 * 
 * This class serves as the main entry point for using AI features in the executor.
 * It coordinates the ScriptAssistant, SignatureAdaptation, and other AI components
 * to provide a unified interface that works completely offline with local training.
 */
class AIIntegrationManager {
public:
    // AI capability flags
    enum AICapability {
        SCRIPT_GENERATION = 0x01,    // Generate scripts
        SCRIPT_DEBUGGING = 0x02,     // Debug scripts
        SCRIPT_ANALYSIS = 0x04,      // Analyze scripts
        GAME_ANALYSIS = 0x08,        // Analyze games
        SIGNATURE_ADAPTATION = 0x10, // Anti-cheat adaptation
        VULNERABILITY_DETECTION = 0x20, // Detect game vulnerabilities
        FULL_CAPABILITIES = 0xFF     // All capabilities
    };
    
    // Status update structure
    struct StatusUpdate {
        std::string m_status;         // Status message
        float m_progress;             // Progress value (0.0-1.0)
        bool m_isError;               // Whether status is an error
        
        StatusUpdate() : m_progress(0.0f), m_isError(false) {}
        
        StatusUpdate(const std::string& status, float progress = 0.0f, bool isError = false)
            : m_status(status), m_progress(progress), m_isError(isError) {}
    };
    
    // Callback types
    using StatusCallback = std::function<void(const StatusUpdate&)>;
    using ScriptGenerationCallback = std::function<void(const std::string&)>;
    using DebugResultCallback = std::function<void(const std::string&)>;
    using QueryResponseCallback = std::function<void(const std::string&)>;
    
private:
    // Singleton instance
    static AIIntegrationManager* s_instance;
    
    // Member variables
    std::shared_ptr<ScriptAssistant> m_scriptAssistant;       // Script assistant
    std::shared_ptr<SignatureAdaptation> m_signatureAdaptation; // Signature adaptation
    std::shared_ptr<HybridAISystem> m_hybridAI;               // Hybrid AI system
    std::shared_ptr<OfflineService> m_offlineService;         // Offline service
    AIConfig& m_config;                                       // AI configuration
    StatusCallback m_statusCallback;                          // Status callback
    uint32_t m_availableCapabilities;                         // Available capabilities
    bool m_initialized;                                       // Initialization flag
    bool m_initializing;                                      // Initialization in progress flag
    
    // Private constructor for singleton
    AIIntegrationManager();
    
    // Initialize components
    void InitializeComponents();
    
    // Report status update
    void ReportStatus(const StatusUpdate& status);
    
    // Get optimal operation mode
    AIConfig::OperationMode GetOptimalOperationMode() const;
    
public:
    /**
     * @brief Get shared instance
     * @return Shared instance
     */
    static AIIntegrationManager& GetSharedInstance();
    
    /**
     * @brief Destructor
     */
    ~AIIntegrationManager();
    
    /**
     * @brief Initialize the manager
     * @param statusCallback Optional callback for initialization status
     * @return True if initialization started successfully
     */
    bool Initialize(StatusCallback statusCallback = nullptr);
    
    /**
     * @brief Check if manager is initialized
     * @return True if initialized
     */
    bool IsInitialized() const;
    
    /**
     * @brief Get available AI capabilities
     * @return Bit flags of available capabilities
     */
    uint32_t GetAvailableCapabilities() const;
    
    /**
     * @brief Check if a specific capability is available
     * @param capability Capability to check
     * @return True if capability is available
     */
    bool HasCapability(AICapability capability) const;
    
    /**
     * @brief Generate a script
     * @param description Script description
     * @param context Additional context (e.g., game type)
     * @param callback Function to call with the generated script
     */
    void GenerateScript(const std::string& description, 
                       const std::string& context,
                       ScriptGenerationCallback callback);
    
    /**
     * @brief Debug a script
     * @param script Script to debug
     * @param callback Function to call with debug results
     */
    void DebugScript(const std::string& script,
                    DebugResultCallback callback);
    
    /**
     * @brief Process a general query
     * @param query User query
     * @param callback Function to call with the response
     */
    void ProcessQuery(const std::string& query,
                     QueryResponseCallback callback);
    
    /**
     * @brief Report detection event to signature adaptation
     * @param detectionType Type of detection
     * @param signature Binary signature data
     */
    void ReportDetection(const std::string& detectionType,
                        const std::vector<uint8_t>& signature);
    
    /**
     * @brief Get signature adaptation system
     * @return Signature adaptation system
     */
    std::shared_ptr<SignatureAdaptation> GetSignatureAdaptation() const;
    
    /**
     * @brief Get script assistant
     * @return Script assistant
     */
    std::shared_ptr<ScriptAssistant> GetScriptAssistant() const;
    
    /**
     * @brief Get hybrid AI system
     * @return Hybrid AI system
     */
    std::shared_ptr<HybridAISystem> GetHybridAI() const;
    
    /**
     * @brief Get offline service
     * @return Offline service
     */
    std::shared_ptr<OfflineService> GetOfflineService() const;
    
    /**
     * @brief Set operation mode
     * @param mode Operation mode
     */
    void SetOperationMode(AIConfig::OperationMode mode);
    
    /**
     * @brief Get operation mode
     * @return Current operation mode
     */
    AIConfig::OperationMode GetOperationMode() const;
    
    /**
     * @brief Set learning mode
     * @param mode Learning mode
     */
    void SetLearningMode(AIConfig::LearningMode mode);
    
    /**
     * @brief Get learning mode
     * @return Current learning mode
     */
    AIConfig::LearningMode GetLearningMode() const;
    
    /**
     * @brief Enable or disable vulnerability scanner
     * @param enable Whether to enable
     */
    void SetEnableVulnerabilityScanner(bool enable);
    
    /**
     * @brief Check if vulnerability scanner is enabled
     * @return Whether enabled
     */
    bool GetEnableVulnerabilityScanner() const;
    
    /**
     * @brief Handle memory warning
     */
    void HandleMemoryWarning();
    
    /**
     * @brief Handle app entering foreground
     */
    void HandleAppForeground();
    
    /**
     * @brief Handle app entering background
     */
    void HandleAppBackground();
    
    /**
     * @brief Get memory usage
     * @return Memory usage in bytes
     */
    uint64_t GetMemoryUsage() const;
    
    /**
     * @brief Save configuration changes
     * @return True if save was successful
     */
    bool SaveConfig();
    
    /**
     * @brief Set up data collection settings
     * @param enableCollection Whether to collect training data
     * @param maxSamplesPerDay Maximum samples to collect per day
     */
    void SetupDataCollection(bool enableCollection, uint32_t maxSamplesPerDay = 100);
    
    /**
     * @brief Start model update/training
     * @param callback Status callback for training progress
     * @return True if training started successfully
     */
    bool StartModelTraining(StatusCallback callback = nullptr);
    
    /**
     * @brief Cancel model update/training
     * @return True if training was cancelled successfully
     */
    bool CancelModelTraining();
    
    /**
     * @brief Check if model training is in progress
     * @return True if training is in progress
     */
    bool IsModelTrainingInProgress() const;
};

} // namespace AIFeatures
} // namespace iOS
