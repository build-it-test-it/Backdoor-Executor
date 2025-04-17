#include "AIIntegration.h"
#include "AIConfig.h"
#include "AISystemInitializer.h"

// For CI build
#include "../ui/VulnerabilityViewController.h"
#include "../ui/MainViewController.h"
#include "ScriptAssistant.h"

#ifdef CI_BUILD
#define CI_SAFE_BUILD
#endif

// Simplified implementation for CI build
namespace iOS {
namespace AIFeatures {

// Private implementation class
class AIIntegrationImpl {
private:
    // Configuration
    AIConfig m_config;
    
    // Main view controller
    std::shared_ptr<UI::MainViewController> m_mainViewController;
    
    // Vulnerability view controller
    std::shared_ptr<UI::VulnerabilityViewController> m_vulnerabilityViewController;
    
    // Script assistant
    std::shared_ptr<ScriptAssistant> m_scriptAssistant;
    
    // System initialization state
    bool m_isInitialized;

public:
    AIIntegrationImpl() 
        : m_isInitialized(false) {
        
        // Simplified implementation for CI build
    }
    
    ~AIIntegrationImpl() {
        Cleanup();
    }
    
    bool Initialize() {
        if (m_isInitialized) {
            return true;
        }
        
        // Initialize system components
        if (!InitializeSystemComponents()) {
            return false;
        }
        
        // Initialize UI components
        if (!InitializeUIComponents()) {
            return false;
        }
        
        m_isInitialized = true;
        return true;
    }
    
    bool InitializeSystemComponents() {
        // For CI build, just succeed
        return true;
    }
    
    bool InitializeUIComponents() {
        // For CI build, just create stub components
        m_mainViewController = std::make_shared<UI::MainViewController>();
        m_vulnerabilityViewController = std::make_shared<UI::VulnerabilityViewController>();
        m_scriptAssistant = std::make_shared<ScriptAssistant>();
        
        // Just return success for CI
        return true;
    }
    
    bool IsInitialized() const {
        return m_isInitialized;
    }
    
    void Cleanup() {
        m_mainViewController = nullptr;
        m_vulnerabilityViewController = nullptr;
        m_scriptAssistant = nullptr;
        m_isInitialized = false;
    }
    
    void GenerateScript(const std::string& prompt, std::function<void(const std::string&)> callback) {
        if (!m_isInitialized) {
            callback("Error: AI system not initialized");
            return;
        }
        
        // For CI build, just return a stub script
        std::string stubScript = "-- Generated stub script\nprint('Hello, world!')";
        callback(stubScript);
    }
    
    std::shared_ptr<UI::MainViewController> GetMainViewController() const {
        return m_mainViewController;
    }
    
    std::shared_ptr<UI::VulnerabilityViewController> GetVulnerabilityViewController() const {
        return m_vulnerabilityViewController;
    }
    
    std::shared_ptr<ScriptAssistant> GetScriptAssistant() const {
        return m_scriptAssistant;
    }
};

// Actual AIIntegration implementation
class AIIntegration {
private:
    std::unique_ptr<AIIntegrationImpl> m_impl;
    
public:
    AIIntegration() 
        : m_impl(new AIIntegrationImpl()) {
    }
    
    ~AIIntegration() {
        // Implementation handled by unique_ptr
    }
    
    bool Initialize() {
        return m_impl->Initialize();
    }
    
    bool IsInitialized() const {
        return m_impl->IsInitialized();
    }
    
    void Cleanup() {
        m_impl->Cleanup();
    }
    
    void GenerateScript(const std::string& prompt, std::function<void(const std::string&)> callback) {
        m_impl->GenerateScript(prompt, callback);
    }
    
    std::shared_ptr<ScriptAssistant> GetScriptAssistant() const {
        return m_impl->GetScriptAssistant();
    }
    
    std::shared_ptr<UI::MainViewController> GetMainViewController() const {
        return m_impl->GetMainViewController();
    }
    
    std::shared_ptr<UI::VulnerabilityViewController> GetVulnerabilityViewController() const {
        return m_impl->GetVulnerabilityViewController();
    }
};

} // namespace AIFeatures
} // namespace iOS

// C interface for native code integration
extern "C" {

void* CreateAIIntegration() {
    return new iOS::AIFeatures::AIIntegration();
}

void DestroyAIIntegration(void* integration) {
    delete static_cast<iOS::AIFeatures::AIIntegration*>(integration);
}

bool InitializeAIIntegration(void* integration) {
    return static_cast<iOS::AIFeatures::AIIntegration*>(integration)->Initialize();
}

bool IsAIIntegrationInitialized(void* integration) {
    return static_cast<iOS::AIFeatures::AIIntegration*>(integration)->IsInitialized();
}

void CleanupAIIntegration(void* integration) {
    static_cast<iOS::AIFeatures::AIIntegration*>(integration)->Cleanup();
}

void GenerateScript(void* integration, const char* prompt, void (*callback)(const char*)) {
    static_cast<iOS::AIFeatures::AIIntegration*>(integration)->GenerateScript(
        prompt,
        [callback](const std::string& script) {
            callback(script.c_str());
        }
    );
}

void* GetMainViewController(void* integration) {
    auto aiIntegration = static_cast<iOS::AIFeatures::AIIntegration*>(integration);
    return aiIntegration->GetMainViewController().get();
}

void* GetVulnerabilityViewController(void* integration) {
    // For CI build, just return something non-null
    auto aiIntegration = static_cast<iOS::AIFeatures::AIIntegration*>(integration);
    return aiIntegration->GetVulnerabilityViewController().get();
}

}
