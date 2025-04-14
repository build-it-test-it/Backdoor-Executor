#pragma once

#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <unordered_map>
#include <mutex>

namespace iOS {
namespace AIFeatures {

/**
 * @class HybridAISystem
 * @brief AI system that works both online and offline
 * 
 * This class provides an AI system that can work in both online and offline modes.
 * It uses local models when offline or as a fallback, but can enhance its capabilities
 * by connecting to external services when online connectivity is available.
 */
class HybridAISystem {
public:
    // AI request structure
    struct AIRequest {
        std::string m_query;         // User query
        std::string m_context;       // Additional context (e.g., script content)
        std::string m_requestType;   // Request type (e.g., "script_generation", "debug")
        uint64_t m_timestamp;        // Request timestamp
        bool m_forceOffline;         // Force offline processing (even if online is available)
        
        AIRequest() : m_timestamp(0), m_forceOffline(false) {}
        
        AIRequest(const std::string& query, 
                 const std::string& context = "", 
                 const std::string& requestType = "general",
                 bool forceOffline = false)
            : m_query(query), m_context(context), m_requestType(requestType),
              m_forceOffline(forceOffline),
              m_timestamp(std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count()) {}
    };
    
    // AI response structure
    struct AIResponse {
        bool m_success;              // Success flag
        std::string m_content;       // Response content
        std::string m_scriptCode;    // Generated script code (if applicable)
        std::vector<std::string> m_suggestions; // Suggested actions
        uint64_t m_processingTime;   // Processing time in milliseconds
        std::string m_errorMessage;  // Error message if failed
        bool m_usedOnlineMode;       // Whether online mode was used
        
        AIResponse() : m_success(false), m_processingTime(0), m_usedOnlineMode(false) {}
        
        AIResponse(bool success, const std::string& content = "", 
                  const std::string& scriptCode = "", uint64_t processingTime = 0,
                  const std::string& errorMessage = "", bool usedOnlineMode = false)
            : m_success(success), m_content(content), m_scriptCode(scriptCode),
              m_processingTime(processingTime), m_errorMessage(errorMessage),
              m_usedOnlineMode(usedOnlineMode) {}
    };
    
    // Online mode enum
    enum class OnlineMode {
        Auto,       // Automatically use online when available, fallback to offline
        PreferOffline, // Prefer offline, use online only when offline fails
        PreferOnline,  // Prefer online, use offline only when online fails
        OfflineOnly,   // Always use offline mode
        OnlineOnly     // Always use online mode (will fail if no connectivity)
    };
    
    // Callback for AI responses
    using ResponseCallback = std::function<void(const AIResponse&)>;
    
private:
    // Member variables with consistent m_ prefix
    bool m_initialized;                       // Initialization flag
    bool m_localModelsLoaded;                 // Local models loaded flag
    bool m_isInLowMemoryMode;                 // Low memory mode flag
    bool m_networkConnected;                  // Network connectivity flag
    OnlineMode m_onlineMode;                  // Current online mode
    std::string m_apiEndpoint;                // API endpoint for online processing
    std::string m_apiKey;                     // API key for authentication
    std::string m_modelPath;                  // Path to local model files
    
    // Local models
    void* m_scriptAssistantModel;             // Opaque pointer to script assistant model
    void* m_scriptGeneratorModel;             // Opaque pointer to script generator model
    void* m_debugAnalyzerModel;               // Opaque pointer to debug analyzer model
    void* m_patternRecognitionModel;          // Opaque pointer to pattern recognition model
    
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
     * @param modelPath Path to model files
     * @param apiEndpoint Optional API endpoint for online processing
     * @param apiKey Optional API key for authentication
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
     * @brief Set API key for authentication
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
