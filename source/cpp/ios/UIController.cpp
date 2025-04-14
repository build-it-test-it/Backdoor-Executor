// Define CI_BUILD for CI builds
#define CI_BUILD

#include "UIController.h"
#include <algorithm>
#include <chrono>
#include <iostream>
#include <thread>

// Only include iOS-specific headers when not in CI build
#ifndef CI_BUILD
#import <UIKit/UIKit.h>
#import <Foundation/Foundation.h>
#import <objc/runtime.h>
#endif

namespace iOS {
    
    // Constructor
    UIController::UIController()
        : m_uiView(nullptr),
          m_floatingButton(std::make_unique<FloatingButtonController>()),
          m_isVisible(false),
          m_currentTab(TabType::Editor),
          m_opacity(0.9f),
          m_isDraggable(true),
          m_currentScript("") {
        // Initialize with empty callbacks
        m_executeCallback = [](const std::string&) { return false; };
        m_saveScriptCallback = [](const ScriptInfo&) { return false; };
        m_loadScriptsCallback = []() { return std::vector<ScriptInfo>(); };
    }
    
    // Destructor
    UIController::~UIController() {
        // Save UI state before destroying
        SaveUIState();
        
        // Release the UI view
        if (m_uiView) {
            m_uiView = nullptr;
        }
    }
    
    // Initialize the UI
    bool UIController::Initialize() {
        // Create the UI elements
        CreateUI();
        
        // Load saved UI state
        LoadUIState();
        
        // Set up the floating button
        if (m_floatingButton) {
            m_floatingButton->SetTapCallback([this]() {
                Toggle();
            });
        }
        
        // Initial refresh of scripts list
        RefreshScriptsList();
        
        return true;
    }
    
    // Show the UI
    void UIController::Show() {
        if (m_isVisible) return;
        
        // In CI build, just set the flag
        m_isVisible = true;
        
        // Log for debugging
        std::cout << "UIController::Show - UI visibility set to true" << std::endl;
    }
    
    // Hide the UI
    void UIController::Hide() {
        if (!m_isVisible) return;
        
        // In CI build, just set the flag
        m_isVisible = false;
        
        // Log for debugging
        std::cout << "UIController::Hide - UI visibility set to false" << std::endl;
    }
    
    // Toggle UI visibility
    bool UIController::Toggle() {
        if (m_isVisible) {
            Hide();
        } else {
            Show();
        }
        return m_isVisible;
    }
    
    // Check if UI is visible
    bool UIController::IsVisible() const {
        return m_isVisible;
    }
    
    // Switch to a specific tab
    void UIController::SwitchTab(TabType tab) {
        if (tab == m_currentTab) return;
        
        m_currentTab = tab;
        
        // Log for debugging
        std::cout << "UIController::SwitchTab - Tab switched to " << static_cast<int>(tab) << std::endl;
        
        UpdateLayout();
    }
    
    // Get current tab
    UIController::TabType UIController::GetCurrentTab() const {
        return m_currentTab;
    }
    
    // Set UI opacity
    void UIController::SetOpacity(float opacity) {
        // Clamp opacity to valid range
        m_opacity = std::max(0.0f, std::min(1.0f, opacity));
        
        // Log for debugging
        std::cout << "UIController::SetOpacity - Opacity set to " << m_opacity << std::endl;
    }
    
    // Get UI opacity
    float UIController::GetOpacity() const {
        return m_opacity;
    }
    
    // Enable/disable UI dragging
    void UIController::SetDraggable(bool enabled) {
        m_isDraggable = enabled;
        
        // Log for debugging
        std::cout << "UIController::SetDraggable - Draggable set to " << (m_isDraggable ? "true" : "false") << std::endl;
    }
    
    // Check if UI is draggable
    bool UIController::IsDraggable() const {
        return m_isDraggable;
    }
    
    // Set script content in editor
    void UIController::SetScriptContent(const std::string& script) {
        m_currentScript = script;
        
        // Log for debugging
        std::cout << "UIController::SetScriptContent - Script content set (" << script.length() << " chars)" << std::endl;
    }
    
    // Get script content from editor
    std::string UIController::GetScriptContent() const {
        return m_currentScript;
    }
    
    // Execute current script in editor
    bool UIController::ExecuteCurrentScript() {
        // Get the current script content
        std::string script = GetScriptContent();
        
        // Call the execute callback
        bool success = m_executeCallback(script);
        
        // Log to console
        if (success) {
            AppendToConsole("Script executed successfully.");
        } else {
            AppendToConsole("Script execution failed.");
        }
        
        return success;
    }
    
    // Save current script in editor
    bool UIController::SaveCurrentScript(const std::string& name) {
        // Get the current script content
        std::string script = GetScriptContent();
        
        // Generate a name if not provided
        std::string scriptName = name;
        if (scriptName.empty()) {
            // Generate name based on current timestamp
            auto now = std::chrono::system_clock::now();
            auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(
                now.time_since_epoch()).count();
            scriptName = "Script_" + std::to_string(timestamp);
        }
        
        // Create script info
        ScriptInfo scriptInfo(scriptName, script, std::chrono::system_clock::now().time_since_epoch().count());
        
        // Call the save callback
        bool success = m_saveScriptCallback(scriptInfo);
        
        if (success) {
            // Refresh the scripts list
            RefreshScriptsList();
            AppendToConsole("Script saved: " + scriptName);
        } else {
            AppendToConsole("Failed to save script: " + scriptName);
        }
        
        return success;
    }
    
    // Load a script into the editor
    bool UIController::LoadScript(const UIController::ScriptInfo& scriptInfo) {
        // Set the script content
        SetScriptContent(scriptInfo.m_content);
        
        // Ensure editor tab is active
        SwitchTab(TabType::Editor);
        
        AppendToConsole("Loaded script: " + scriptInfo.m_name);
        
        return true;
    }
    
    // Delete a saved script
    bool UIController::DeleteScript(const std::string& name) {
        bool success = false;
        
        // Find and remove the script from the saved scripts list
        auto it = std::find_if(m_savedScripts.begin(), m_savedScripts.end(),
                             [&name](const ScriptInfo& info) {
                                 return info.m_name == name;
                             });
        
        if (it != m_savedScripts.end()) {
            m_savedScripts.erase(it);
            success = true;
            
            // Update the UI list
            RefreshScriptsList();
            AppendToConsole("Deleted script: " + name);
        } else {
            AppendToConsole("Script not found: " + name);
        }
        
        return success;
    }
    
    // Clear the console
    void UIController::ClearConsole() {
        m_consoleText.clear();
        std::cout << "UIController::ClearConsole - Console cleared" << std::endl;
    }
    
    // Get console text
    std::string UIController::GetConsoleText() const {
        return m_consoleText;
    }
    
    // Set execute callback
    void UIController::SetExecuteCallback(ExecuteCallback callback) {
        if (callback) {
            m_executeCallback = callback;
        }
    }
    
    // Set save script callback
    void UIController::SetSaveScriptCallback(SaveScriptCallback callback) {
        if (callback) {
            m_saveScriptCallback = callback;
        }
    }
    
    // Set load scripts callback
    void UIController::SetLoadScriptsCallback(LoadScriptsCallback callback) {
        if (callback) {
            m_loadScriptsCallback = callback;
        }
    }
    
    // Check if button is visible
    bool UIController::IsButtonVisible() const {
        return m_floatingButton && m_floatingButton->IsVisible();
    }
    
    // Show/hide floating button
    void UIController::SetButtonVisible(bool visible) {
        if (m_floatingButton) {
            if (visible) {
                m_floatingButton->Show();
            } else {
                m_floatingButton->Hide();
            }
        }
    }
    
    // Private method implementations
    
    void UIController::CreateUI() {
        // Stub implementation for CI builds
        std::cout << "UIController::CreateUI - Stub implementation for CI build" << std::endl;
    }
    
    void UIController::UpdateLayout() {
        // Stub implementation for CI builds
        std::cout << "UIController::UpdateLayout - Stub implementation for CI build" << std::endl;
    }
    
    void UIController::SaveUIState() {
        // Stub implementation for CI builds
        std::cout << "UIController::SaveUIState - Stub implementation for CI build" << std::endl;
    }
    
    void UIController::LoadUIState() {
        // Stub implementation for CI builds
        std::cout << "UIController::LoadUIState - Stub implementation for CI build" << std::endl;
    }
    
    void UIController::RefreshScriptsList() {
        // Load scripts using the callback
        m_savedScripts = m_loadScriptsCallback();
        std::cout << "UIController::RefreshScriptsList - Loaded " << m_savedScripts.size() << " scripts" << std::endl;
    }
    
    void UIController::AppendToConsole(const std::string& text) {
        // Add the text to the console with a timestamp
        auto now = std::chrono::system_clock::now();
        auto nowTime = std::chrono::system_clock::to_time_t(now);
        std::string timestamp = std::ctime(&nowTime);
        if (!timestamp.empty() && timestamp.back() == '\n') {
            timestamp.pop_back(); // Remove trailing newline
        }
        
        std::string logEntry = "[" + timestamp + "] " + text + "\n";
        m_consoleText += logEntry;
        
        // Log to stdout for CI builds
        std::cout << "CONSOLE: " << logEntry;
    }
    
} // namespace iOS
