#pragma once

#include "../../objc_isolation.h"
#include <string>
#include <memory>

namespace iOS {
namespace AIFeatures {
    class ScriptAssistant; // Forward declaration
}

namespace UI {

/**
 * @class MainViewController
 * @brief Main view controller for the application
 * 
 * This is a stub implementation for CI builds.
 */
class MainViewController {
public:
    /**
     * @brief Constructor
     */
    MainViewController() {}
    
    /**
     * @brief Destructor
     */
    virtual ~MainViewController() {}
    
    /**
     * @brief Initialize the view controller
     * @return True if initialization succeeded
     */
    bool Initialize() { return true; }
    
    /**
     * @brief Set the script assistant
     * @param assistant Shared pointer to script assistant
     */
    void SetScriptAssistant(std::shared_ptr<AIFeatures::ScriptAssistant> assistant) {}
    
    /**
     * @brief Show the view controller
     */
    void Show() {}
    
    /**
     * @brief Hide the view controller
     */
    void Hide() {}
};

} // namespace UI
} // namespace iOS
