// Fixed implementation for MainViewController matching the header interface
#include "../../ios_compat.h"
#include "MainViewController.h"
#include <iostream>

namespace iOS {
namespace UI {

    // Constructor
    MainViewController::MainViewController() {
        std::cout << "MainViewController constructor called" << std::endl;
        m_viewController = nullptr;
    }

    // Destructor
    MainViewController::~MainViewController() {
        std::cout << "MainViewController destructor called" << std::endl;
        
        if (m_viewController) {
            // Release the retained native view controller
            CFRelease(m_viewController);
            m_viewController = nullptr;
        }
    }

    // Execute a script
    bool MainViewController::ExecuteScript(const std::string& script) {
        std::cout << "MainViewController ExecuteScript called: " << script << std::endl;
        return true; // Stub implementation
    }

    // Display AI response
    void MainViewController::DisplayAIResponse(const std::string& response) {
        std::cout << "MainViewController DisplayAIResponse called: " << response << std::endl;
    }

    // Set script execution callback
    void MainViewController::SetExecutionCallback(ExecutionCallback callback) {
        m_executionCallback = callback;
    }

    // Set save script callback
    void MainViewController::SetSaveScriptCallback(SaveScriptCallback callback) {
        m_saveScriptCallback = callback;
    }

    // Set load scripts callback
    void MainViewController::SetLoadScriptsCallback(LoadScriptsCallback callback) {
        m_loadScriptsCallback = callback;
    }

    // Set AI query callback
    void MainViewController::SetAIQueryCallback(AIQueryCallback callback) {
        m_aiQueryCallback = callback;
    }

    // Set AI response callback
    void MainViewController::SetAIResponseCallback(AIResponseCallback callback) {
        m_aiResponseCallback = callback;
    }

    // Get native view controller
    void* MainViewController::GetNativeViewController() const {
        return m_viewController;
    }

    // Set native view controller
    void MainViewController::SetNativeViewController(void* viewController) {
        // Release any existing view controller
        if (m_viewController) {
            CFRelease(m_viewController);
        }
        
        // Retain the new view controller if it's not null
        if (viewController) {
            // Use const_cast to safely assign const void* to void*
            m_viewController = const_cast<void*>(CFRetain(viewController));
        } else {
            m_viewController = nullptr;
        }
    }

} // namespace UI
} // namespace iOS
