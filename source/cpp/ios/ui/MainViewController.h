#pragma once

#include "../../objc_isolation.h"
#include <memory>
#include <string>
#include <vector>
#include <functional>

namespace iOS {
namespace UI {

/**
 * @class MainViewController
 * @brief Main view controller for the iOS UI
 *
 * This class serves as the primary interface between the AI features and the UI system.
 * It provides callbacks and methods for interacting with and manipulating the UI.
 */
class MainViewController {
public:
    // Callback typedefs
    using ScriptExecutionCallback = std::function<bool(const std::string&)>;
    using AIResponseCallback = std::function<void(const std::string&)>;
    
private:
    // Implementation details
    void* m_viewController = nullptr;
    ScriptExecutionCallback m_scriptExecutionCallback;
    AIResponseCallback m_aiResponseCallback;
    
public:
    /**
     * @brief Constructor
     */
    MainViewController();
    
    /**
     * @brief Destructor
     */
    ~MainViewController();
    
    /**
     * @brief Execute a script
     * @param script Script to execute
     * @return True if execution succeeded
     */
    bool ExecuteScript(const std::string& script);
    
    /**
     * @brief Display AI response in UI
     * @param response AI response to display
     */
    void DisplayAIResponse(const std::string& response);
    
    /**
     * @brief Set script execution callback
     * @param callback Function to call when executing scripts
     */
    void SetScriptExecutionCallback(ScriptExecutionCallback callback);
    
    /**
     * @brief Set AI response callback
     * @param callback Function to call when receiving AI responses
     */
    void SetAIResponseCallback(AIResponseCallback callback);
    
    /**
     * @brief Get native view controller
     * @return Opaque pointer to native view controller
     */
    void* GetNativeViewController() const;
    
    /**
     * @brief Set native view controller
     * @param viewController Opaque pointer to native view controller
     */
    void SetNativeViewController(void* viewController);
};

} // namespace UI
} // namespace iOS
