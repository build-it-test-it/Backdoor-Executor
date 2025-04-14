#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>
#include <chrono>

namespace iOS {
namespace AIFeatures {

    /**
     * @class ScriptAssistant
     * @brief AI assistant for Lua scripting and game analysis
     * 
     * This class implements an AI assistant that can help users with Lua scripting,
     * generate scripts, analyze games, and provide contextual help. It integrates
     * with the execution system to run scripts directly when requested.
     */
    class ScriptAssistant {
    public:
        // Message type enumeration
        enum class MessageType {
            UserQuery,        // Question or request from user
            AssistantResponse, // Response from assistant
            SystemMessage,    // System message
            ScriptExecution,  // Script execution
            GameAnalysis      // Game analysis
        };
        
        // Message structure
        struct Message {
            MessageType m_type;               // Type of message
            std::string m_content;            // Message content
            uint64_t m_timestamp;             // Message timestamp
            std::unordered_map<std::string, std::string> m_metadata; // Additional metadata
            
            Message()
                : m_type(MessageType::UserQuery),
                  m_timestamp(std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now().time_since_epoch()).count()) {}
                    
            Message(MessageType type, const std::string& content)
                : m_type(type), m_content(content),
                  m_timestamp(std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now().time_since_epoch()).count()) {}
        };
        
        // Game object structure
        struct GameObject {
            std::string m_name;                // Object name
            std::string m_className;           // Object class
            std::unordered_map<std::string, std::string> m_properties; // Object properties
            std::vector<std::shared_ptr<GameObject>> m_children;      // Child objects
            
            GameObject() {}
            
            GameObject(const std::string& name, const std::string& className)
                : m_name(name), m_className(className) {}
        };
        
        // Game context structure
        struct GameContext {
            std::string m_gameName;            // Game name
            std::string m_placeId;             // Place ID
            std::shared_ptr<GameObject> m_rootObject; // Root game object
            std::vector<std::string> m_availableServices; // Available services
            std::unordered_map<std::string, std::string> m_gameMetadata; // Game metadata
            
            GameContext() : m_rootObject(std::make_shared<GameObject>("Game", "DataModel")) {}
        };
        
        // Script template structure
        struct ScriptTemplate {
            std::string m_name;                // Template name
            std::string m_description;         // Template description
            std::string m_code;                // Template code
            std::vector<std::string> m_tags;   // Template tags
            std::unordered_map<std::string, std::string> m_parameters; // Template parameters
            
            ScriptTemplate() {}
            
            ScriptTemplate(const std::string& name, const std::string& description,
                          const std::string& code)
                : m_name(name), m_description(description), m_code(code) {}
        };
        
        // Callback for generated scripts
        using ScriptGeneratedCallback = std::function<void(const std::string& script)>;
        
        // Callback for script execution
        using ScriptExecutionCallback = std::function<void(bool success, const std::string& output)>;
        
        // Callback for responses
        using ResponseCallback = std::function<void(const Message& response)>;
        
    private:
        // Member variables with consistent m_ prefix
        bool m_initialized;                   // Whether the assistant is initialized
        std::vector<Message> m_conversationHistory; // Conversation history
        GameContext m_currentContext;         // Current game context
        std::vector<ScriptTemplate> m_scriptTemplates; // Script templates
        std::unordered_map<std::string, std::string> m_userPreferences; // User preferences
        void* m_languageModel;                // Opaque pointer to language model
        void* m_gameAnalyzer;                 // Opaque pointer to game analyzer
        void* m_scriptGenerator;              // Opaque pointer to script generator
        void* m_executionInterface;           // Opaque pointer to execution interface
        ResponseCallback m_responseCallback;  // Callback for responses
        ScriptExecutionCallback m_executionCallback; // Callback for script execution
        std::mutex m_mutex;                   // Mutex for thread safety
        uint32_t m_maxHistorySize;            // Maximum history size
        bool m_autoExecute;                   // Whether to auto-execute generated scripts
        
        // Private methods
        Message ProcessUserQuery(const std::string& query);
        std::string GenerateScript(const std::string& description);
        std::string AnalyzeScript(const std::string& script);
        std::string ExplainScript(const std::string& script);
        void UpdateGameContext();
        std::shared_ptr<GameObject> AnalyzeGameObject(void* ptr);
        std::string SuggestScriptForContext();
        std::string FormatResponse(const std::string& response);
        std::vector<std::string> ExtractIntents(const std::string& query);
        bool IsScriptExecutionRequest(const std::string& query);
        std::string ExtractScriptFromQuery(const std::string& query);
        void TrimConversationHistory();
        
    public:
        /**
         * @brief Constructor
         */
        ScriptAssistant();
        
        /**
         * @brief Destructor
         */
        ~ScriptAssistant();
        
        /**
         * @brief Initialize the script assistant
         * @return True if initialization succeeded, false otherwise
         */
        bool Initialize();
        
        /**
         * @brief Send a user query
         * @param query User query
         * @return Response message
         */
        Message ProcessQuery(const std::string& query);
        
        /**
         * @brief Generate a script based on description
         * @param description Script description
         * @param callback Callback function
         */
        void GenerateScriptAsync(const std::string& description, ScriptGeneratedCallback callback);
        
        /**
         * @brief Execute a script
         * @param script Script to execute
         * @param callback Callback function
         */
        void ExecuteScript(const std::string& script, ScriptExecutionCallback callback);
        
        /**
         * @brief Analyze the current game
         * @return Analysis message
         */
        Message AnalyzeGame();
        
        /**
         * @brief Set response callback
         * @param callback Callback function
         */
        void SetResponseCallback(const ResponseCallback& callback);
        
        /**
         * @brief Set script execution callback
         * @param callback Callback function
         */
        void SetExecutionCallback(const ScriptExecutionCallback& callback);
        
        /**
         * @brief Set the current game context
         * @param context Game context
         */
        void SetGameContext(const GameContext& context);
        
        /**
         * @brief Get the current game context
         * @return Current game context
         */
        GameContext GetGameContext() const;
        
        /**
         * @brief Add a script template
         * @param template Script template
         * @return True if template was added, false otherwise
         */
        bool AddScriptTemplate(const ScriptTemplate& scriptTemplate);
        
        /**
         * @brief Get matching script templates
         * @param tags Tags to match
         * @return Vector of matching templates
         */
        std::vector<ScriptTemplate> GetMatchingTemplates(const std::vector<std::string>& tags);
        
        /**
         * @brief Clear conversation history
         */
        void ClearHistory();
        
        /**
         * @brief Get conversation history
         * @return Vector of messages
         */
        std::vector<Message> GetHistory();
        
        /**
         * @brief Set maximum history size
         * @param size Maximum history size
         */
        void SetMaxHistorySize(uint32_t size);
        
        /**
         * @brief Enable or disable auto-execution
         * @param enable Whether to auto-execute generated scripts
         */
        void SetAutoExecute(bool enable);
        
        /**
         * @brief Check if auto-execution is enabled
         * @return True if auto-execution is enabled, false otherwise
         */
        bool GetAutoExecute() const;
        
        /**
         * @brief Set user preference
         * @param key Preference key
         * @param value Preference value
         */
        void SetUserPreference(const std::string& key, const std::string& value);
        
        /**
         * @brief Get user preference
         * @param key Preference key
         * @param defaultValue Default value if preference doesn't exist
         * @return Preference value
         */
        std::string GetUserPreference(const std::string& key, const std::string& defaultValue = "") const;
        
        /**
         * @brief Get example queries
         * @return Vector of example queries
         */
        static std::vector<std::string> GetExampleQueries();
        
        /**
         * @brief Get example script descriptions
         * @return Vector of example script descriptions
         */
        static std::vector<std::string> GetExampleScriptDescriptions();
        
        /**
         * @brief Release unused resources to save memory
         */
        void ReleaseUnusedResources() {
            // Clear history beyond necessary size
            if (m_conversationHistory.size() > m_maxHistorySize) {
                TrimConversationHistory();
            }
        }
        
        /**
         * @brief Get memory usage of this component
         * @return Memory usage in bytes
         */
        uint64_t GetMemoryUsage() const {
            // Estimate memory usage based on history size and other components
            uint64_t total = 0;
            // Each message takes approximately 1KB
            total += m_conversationHistory.size() * 1024;
            // Templates take approximately 2KB each
            total += m_scriptTemplates.size() * 2048;
            // Base usage is approximately 10MB
            total += 10 * 1024 * 1024;
            return total;
        }
    };

} // namespace AIFeatures
} // namespace iOS
