#include "../objc_isolation.h"


#pragma once

#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <unordered_map>
#include <mutex>
#include "SelfModifyingCodeSystem.h"

namespace iOS {
namespace AIFeatures {

/**
 * @class HybridAISystem
 * @brief AI system that works both online and offline
 * 
 * This class implements a hybrid AI system that can work in both online and offline
 * modes, automatically switching between them based on connectivity and performance
 * requirements.
 */
class HybridAISystem {
public:
    // Request structure for AI processing
    struct AIRequest {
        std::string m_query;         // User query or request
        std::string m_context;       // Additional context information
        std::string m_gameInfo;      // Game-specific information
        std::vector<std::string> m_history; // Conversation history
        
        AIRequest(const std::string& query = "", const std::string& context = "", 
                 const std::string& gameInfo = "")
            : m_query(query), m_context(context), m_gameInfo(gameInfo) {}
    };
    
    // Response structure for AI processing
    struct AIResponse {
        bool m_success;              // Success flag
        std::string m_content;       // Response content
        std::string m_scriptCode;    // Generated script code (if applicable)
        std::vector<std::string> m_suggestions; // Suggested actions
        uint64_t m_processingTime;   // Processing time in milliseconds
        std::string m_errorMessage;  // Error message if failed
        
        AIResponse(bool success, const std::string& content = "", 
                  const std::string& scriptCode = "", uint64_t processingTime = 0,
                  const std::string& errorMessage = "") 
            : m_success(success), 
              m_content(content), 
              m_scriptCode(scriptCode),
              m_processingTime(processingTime), 
              m_errorMessage(errorMessage) {}
    };
    
    // Define ResponseCallback type
    typedef std::function<void(const AIResponse&)> ResponseCallback;
    
    // Online mode enum
    enum class OnlineMode {
        Auto,       // Automatically use online when available, fallback to offline
        PreferOffline, // Prefer offline, use online only when offline fails
        PreferOnline,  // Prefer online, use offline only when online fails
        OfflineOnly,   // Always use offline mode
        OnlineOnly     // Always use online mode (will fail if no connectivity)
    };
    void* m_scriptAssistantModel;             // Opaque pointer to script assistant model
    void* m_scriptGeneratorModel;             // Opaque pointer to script generator model
    void* m_debugAnalyzerModel;               // Opaque pointer to debug analyzer model
    void* m_patternRecognitionModel;          // Opaque pointer to pattern recognition model
    
    // Enhanced AI capabilities
    std::shared_ptr<SelfModifyingCodeSystem> m_selfModifyingSystem; // Self-improving code system
    
    std::unordered_map<std::string, void*> m_modelCache; // Model cache
    std::vector<std::string> m_loadedModelNames; // Names of loaded models
    std::vector<AIRequest> m_requestHistory;  // Request history for learning
    std::vector<AIResponse> m_responseHistory; // Response history for learning
    std::unordered_map<std::string, std::string> m_templateCache; // Script template cache
    uint64_t m_totalMemoryUsage;              // Total memory usage in bytes
    uint64_t m_maxMemoryAllowed;              // Maximum allowed memory in bytes
    std::unordered_map<std::string, std::string> m_dataStore; // Persistent data store
    ResponseCallback m_responseCallback;      // Response callback
    std::mutex m_mutex;                       // Mutex for thread safety
    std::mutex m_networkMutex;                // Mutex for network operations
    
    // Private methods
    bool LoadModel(const std::string& modelName, int priority);
    bool LoadFallbackModel(const std::string& modelName);
    void UnloadModel(const std::string& modelName);
    void OptimizeMemoryUsage();
    bool IsModelLoaded(const std::string& modelName) const;
    void* GetModel(const std::string& modelName) const;
    AIResponse ProcessScriptGeneration(const AIRequest& request);
    AIResponse ProcessScriptDebugging(const AIRequest& request);
    AIResponse ProcessGeneralQuery(const AIRequest& request);
    std::string GenerateScriptFromTemplate(const std::string& templateName, 
                                         const std::unordered_map<std::string, std::string>& parameters);
    AIResponse GenerateScriptFromRules(const std::string& query, const std::string& context);
    AIResponse DebugScriptWithRules(const std::string& script);
    AIResponse GenerateResponseFromRules(const std::string& query, const std::string& context);
    std::vector<std::string> ExtractCodeBlocks(const std::string& text);
    std::vector<std::string> ExtractIntents(const std::string& query);
    uint64_t CalculateModelMemoryUsage(void* model) const;
    void LoadScriptTemplates();
    bool CheckNetworkConnectivity();
    AIResponse ProcessRequestOnline(const AIRequest& request);
    std::string PrepareOnlineAPIRequest(const AIRequest& request);
    AIResponse ParseOnlineAPIResponse(const std::string& response, const AIRequest& request);
    std::string GenerateProtectionStrategyFromRules(const std::string& detectionType);
    bool SaveToDataStore(const std::string& key, const std::string& value);
    std::string LoadFromDataStore(const std::string& key, const std::string& defaultValue = "");
    
public:
    /**
     * @brief Constructor
     */
    HybridAISystem();
    
    /**
     * @brief Destructor
     */
    ~HybridAISystem();
    
    /**
     * @brief Initialize the AI system
     * @param apiEndpoint Optional API endpoint for online processing
     * @param progressCallback Function to call with initialization progress (0.0-1.0)
     * @return True if initialization succeeded, false otherwise
     */
    bool Initialize(const std::string& modelPath, 
                  const std::string& apiEndpoint = "", 
                  const std::string& apiKey = "",
                  std::function<void(float)> progressCallback = nullptr);
    
    /**
     * @brief Process an AI request
     * @param request AI request
     * @param callback Function to call with the response
     */
    void ProcessRequest(const AIRequest& request, ResponseCallback callback);
    
    /**
     * @brief Process an AI request synchronously
     * @param request AI request
     * @return AI response
     */
    AIResponse ProcessRequestSync(const AIRequest& request);
    
    /**
     * @brief Generate a script
     * @param description Script description
     * @param context Additional context (e.g., game type)
     * @param callback Function to call with the generated script
     * @param useOnline Whether to use online processing if available
     */
    void GenerateScript(const std::string& description, const std::string& context, 
                       std::function<void(const std::string&)> callback,
                       bool useOnline = true);
    
    /**
     * @brief Debug a script
     * @param script Script to debug
     * @param callback Function to call with debug information
     * @param useOnline Whether to use online processing if available
     */
    void DebugScript(const std::string& script, 
                    std::function<void(const std::string&)> callback,
                    bool useOnline = true);
    
    /**
     * @brief Process a general query
     * @param query User query
     * @param callback Function to call with the response
     * @param useOnline Whether to use online processing if available
     */
    void ProcessQuery(const std::string& query, 
                     std::function<void(const std::string&)> callback,
                     bool useOnline = true);
    
    /**
     * @brief Set online mode
     * @param mode Online mode to use
     */
    void SetOnlineMode(OnlineMode mode);
    
    /**
     * @brief Get current online mode
     * @return Current online mode
     */
    OnlineMode GetOnlineMode() const;
    
    /**
     * @brief Set API endpoint for online processing
     * @param endpoint API endpoint URL
     */
    void SetAPIEndpoint(const std::string& endpoint);
    
    /**
     * @param apiKey API key
     */
    void SetAPIKey(const std::string& apiKey);
    
    /**
     * @brief Check if online connectivity is available
     * @return True if online connectivity is available, false otherwise
     */
    bool IsOnlineAvailable();
    
    /**
     * @brief Handle memory warning
     */
    void HandleMemoryWarning();
    
    /**
     * @brief Handle network status change
     * @param isConnected Whether network is connected
     */
    void HandleNetworkStatusChange(bool isConnected);
    
    /**
     * @brief Check if the AI system is initialized
     * @return True if initialized, false otherwise
     */
    bool IsInitialized() const;
    
    /**
     * @brief Check if local models are loaded
     * @return True if loaded, false otherwise
     */
    bool AreLocalModelsLoaded() const;
    
    /**
     * @brief Get memory usage
     * @return Memory usage in bytes
     */
    uint64_t GetMemoryUsage() const;
    
    /**
     * @brief Set maximum allowed memory
     * @param maxMemory Maximum allowed memory in bytes
     */
    void SetMaxMemory(uint64_t maxMemory);
    
    /**
     * @brief Enable self-modifying code system
     * Activates advanced code improvement and optimization capabilities
     * @return True if successfully enabled
     */
    bool EnableSelfModifyingSystem();
    
    /**
     * @brief Get loaded model names
     * @return Vector of loaded model names
     */
    std::vector<std::string> GetLoadedModelNames() const;
    
    /**
     * @brief Get a list of script templates
     * @return Map of template names to descriptions
     */
    std::unordered_map<std::string, std::string> GetScriptTemplates() const;
    
    /**
     * @brief Generate response for a detection event
     * @param detectionType Detection type
     * @param signature Detection signature
     * @param useOnline Whether to use online processing if available
     * @return Protection strategy
     */
    std::string GenerateProtectionStrategy(const std::string& detectionType, 
                                         const std::vector<uint8_t>& signature,
                                         bool useOnline = true);
    
    /**
     * @brief Store data persistently
     * @param key Data key
     * @param value Data value
     * @return True if storage succeeded, false otherwise
     */
    bool StoreData(const std::string& key, const std::string& value);
    
    /**
     * @brief Retrieve persistently stored data
     * @param key Data key
     * @param defaultValue Default value to return if key not found
     * @return Retrieved data or default value
     */
    std::string RetrieveData(const std::string& key, const std::string& defaultValue = "");
};

} // namespace AIFeatures
} // namespace iOS
