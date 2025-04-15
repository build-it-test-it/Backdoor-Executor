
#include "../objc_isolation.h"
#pragma once

#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <unordered_map>
#include "../ai_features/ScriptAssistant.h"

// Forward declare Objective-C classes
#if defined(__OBJC__)
@class UIColor;
@class UIViewController;
@class UITextView;
#else
// For C++ code, define opaque types
#ifndef OBJC_OBJECT_DEFINED
#define OBJC_OBJECT_DEFINED
typedef struct objc_object objc_object;
#endif
typedef objc_object UIColor;
typedef objc_object UIViewController;
typedef objc_object UITextView;
#endif

namespace iOS {
namespace UI {

    /**
     * @class ScriptEditorViewController
     * @brief Main script editor interface with debugging and management
     * 
     * This class implements a feature-rich script editor with syntax highlighting,
     * debugging capabilities, and integration with the AI assistant. It's designed
     * for a pleasant user experience across iOS 15-18+ with memory optimization.
     */
    class ScriptEditorViewController {
    public:
        // Script structure
        struct Script {
            std::string m_id;              // Unique identifier
            std::string m_name;            // Script name
            std::string m_content;         // Script content
            std::string m_description;     // Optional description
            std::string m_category;        // Script category
            bool m_isFavorite;             // Is favorite
            uint64_t m_lastExecuted;       // Last executed timestamp
            uint64_t m_created;            // Created timestamp
            uint64_t m_modified;           // Last modified timestamp
            
            Script() : m_isFavorite(false), m_lastExecuted(0), m_created(0), m_modified(0) {}
        };
        
        // Execution result structure
        struct ExecutionResult {
            bool m_success;                // Execution succeeded
            std::string m_output;          // Execution output
            std::string m_error;           // Error message if failed
            uint64_t m_executionTime;      // Execution time in milliseconds
            std::vector<std::string> m_warnings; // Warnings during execution
            
            ExecutionResult() : m_success(false), m_executionTime(0) {}
        };
        
        // Debug information structure
        struct DebugInfo {
            std::string m_variableName;    // Variable name
            std::string m_value;           // Variable value
            std::string m_type;            // Variable type
            int m_line;                    // Line number
            
            DebugInfo() : m_line(0) {}
        };
        
        // Theme enumeration
        enum class Theme {
            Light,
            Dark,
            System,
            Custom
        };
        
        // Editor callback types
        using ExecutionCallback = std::function<void(const ExecutionResult&)>;
        using SaveCallback = std::function<void(const Script&)>;
        using ScriptChangedCallback = std::function<void(const std::string&)>;
        using DebugCallback = std::function<void(const std::vector<DebugInfo>&)>;
        
    private:
        // Member variables with consistent m_ prefix
        void* m_viewController;            // Opaque pointer to UIViewController
        void* m_textView;                  // Opaque pointer to UITextView
        void* m_debugView;                 // Opaque pointer to debug view
        void* m_buttonBar;                 // Opaque pointer to button bar
        void* m_tabBar;                    // Opaque pointer to tab bar
        void* m_contextMenu;               // Opaque pointer to context menu
        void* m_syntaxHighlighter;         // Opaque pointer to syntax highlighter
        void* m_autoCompleteEngine;        // Opaque pointer to autocomplete engine
        void* m_animator;                  // Opaque pointer to UI animator
        std::shared_ptr<AIFeatures::ScriptAssistant> m_scriptAssistant; // Script assistant
        std::unordered_map<std::string, void*> m_effectLayers; // LED effect layers
        Script m_currentScript;            // Current script being edited
        std::vector<Script> m_recentScripts; // Recently edited scripts
        ExecutionCallback m_executionCallback; // Script execution callback
        SaveCallback m_saveCallback;       // Script save callback
        ScriptChangedCallback m_scriptChangedCallback; // Script changed callback
        DebugCallback m_debugCallback;     // Debug callback
        Theme m_currentTheme;              // Current UI theme
        float m_fontSize;                  // Font size
        bool m_showLineNumbers;            // Show line numbers
        bool m_autoComplete;               // Auto-complete enabled
        bool m_syntaxHighlighting;         // Syntax highlighting enabled
        bool m_wordWrap;                   // Word wrap enabled
        bool m_isEditing;                  // Is currently editing
        bool m_isDebugging;                // Is currently debugging
        
        // Private methods
        void InitializeUI();
        void SetupSyntaxHighlighter();
        void SetupAutoComplete();
        void SetupAnimations();
        void SetupLEDEffects();
        void SetupDebugView();
        void SetupButtonBar();
        void SetupTabBar();
        void SetupContextMenu();
        void UpdateButtonStates();
        void ApplyTheme(Theme theme);
        void UpdateSyntaxHighlighting();
        void HighlightLineWithError(int line);
        void ShowDebugInfo(const std::vector<DebugInfo>& debugInfo);
        void SaveScriptState();
        void RestoreScriptState();
        void RegisterForKeyboardNotifications();
        void HandleKeyboardAppearance(float keyboardHeight, double duration);
        void HandleKeyboardDisappearance(double duration);
        void AddLEDEffectToButton(void* button, UIColor* color);
        void PulseLEDEffect(void* effectLayer, float duration, float intensity);
        bool ValidateScript(const std::string& script, std::string& error);
        std::vector<DebugInfo> DebugScript(const std::string& script);
        std::string FormatScriptForDebugging(const std::string& script);
        std::vector<std::string> GetAutoCompleteSuggestions(const std::string& prefix);
        
    public:
        /**
         * @brief Constructor
         */
        ScriptEditorViewController();
        
        /**
         * @brief Destructor
         */
        ~ScriptEditorViewController();
        
        /**
         * @brief Initialize the view controller
         * @return True if initialization succeeded, false otherwise
         */
        bool Initialize();
        
        /**
         * @brief Get the native view controller
         * @return Opaque pointer to UIViewController
         */
        void* GetViewController() const;
        
        /**
         * @brief Set the current script
         * @param script Script to edit
         */
        void SetScript(const Script& script);
        
        /**
         * @brief Get the current script
         * @return Current script
         */
        Script GetScript() const;
        
        /**
         * @brief Execute the current script
         * @return Execution result
         */
        ExecutionResult ExecuteScript();
        
        /**
         * @brief Debug the current script
         * @return Vector of debug information
         */
        std::vector<DebugInfo> DebugCurrentScript();
        
        /**
         * @brief Save the current script
         * @return True if save succeeded, false otherwise
         */
        bool SaveScript();
        
        /**
         * @brief Create a new script
         * @param name Script name
         * @param content Script content
         * @return Created script
         */
        Script CreateNewScript(const std::string& name, const std::string& content = "");
        
        /**
         * @brief Set the script execution callback
         * @param callback Function to call for script execution
         */
        void SetExecutionCallback(const ExecutionCallback& callback);
        
        /**
         * @brief Set the script save callback
         * @param callback Function to call for script saving
         */
        void SetSaveCallback(const SaveCallback& callback);
        
        /**
         * @brief Set the script changed callback
         * @param callback Function to call when script content changes
         */
        void SetScriptChangedCallback(const ScriptChangedCallback& callback);
        
        /**
         * @brief Set the debug callback
         * @param callback Function to call when debug info is available
         */
        void SetDebugCallback(const DebugCallback& callback);
        
        /**
         * @brief Set the UI theme
         * @param theme Theme to use
         */
        void SetTheme(Theme theme);
        
        /**
         * @brief Get the current UI theme
         * @return Current theme
         */
        Theme GetTheme() const;
        
        /**
         * @brief Set font size
         * @param size Font size
         */
        void SetFontSize(float size);
        
        /**
         * @brief Get font size
         * @return Font size
         */
        float GetFontSize() const;
        
        /**
         * @brief Enable or disable line numbers
         * @param enable Whether to show line numbers
         */
        void SetShowLineNumbers(bool enable);
        
        /**
         * @brief Check if line numbers are enabled
         * @return True if line numbers are enabled, false otherwise
         */
        bool GetShowLineNumbers() const;
        
        /**
         * @brief Enable or disable auto-complete
         * @param enable Whether to enable auto-complete
         */
        void SetAutoComplete(bool enable);
        
        /**
         * @brief Check if auto-complete is enabled
         * @return True if auto-complete is enabled, false otherwise
         */
        bool GetAutoComplete() const;
        
        /**
         * @brief Enable or disable syntax highlighting
         * @param enable Whether to enable syntax highlighting
         */
        void SetSyntaxHighlighting(bool enable);
        
        /**
         * @brief Check if syntax highlighting is enabled
         * @return True if syntax highlighting is enabled, false otherwise
         */
        bool GetSyntaxHighlighting() const;
        
        /**
         * @brief Enable or disable word wrap
         * @param enable Whether to enable word wrap
         */
        void SetWordWrap(bool enable);
        
        /**
         * @brief Check if word wrap is enabled
         * @return True if word wrap is enabled, false otherwise
         */
        bool GetWordWrap() const;
        
        /**
         * @brief Reset editor settings to defaults
         */
        void ResetSettings();
        
        /**
         * @brief Set the script assistant
         * @param assistant Script assistant
         */
        void SetScriptAssistant(std::shared_ptr<AIFeatures::ScriptAssistant> assistant);
        
        /**
         * @brief Show AI assistant view
         */
        void ShowAIAssistant();
        
        /**
         * @brief Hide AI assistant view
         */
        void HideAIAssistant();
        
        /**
         * @brief Ask AI assistant for help with current script
         * @return Assistant's response
         */
        std::string AskAssistantForHelp();
        
        /**
         * @brief Get memory usage
         * @return Memory usage in bytes
         */
        uint64_t GetMemoryUsage() const;
    };

} // namespace UI
} // namespace iOS
