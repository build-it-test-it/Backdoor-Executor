#include "AIIntegration.h"
#include "AISystemInitializer.h"
#include "local_models/GeneralAssistantModel.h"
#include <iostream>

namespace iOS {
namespace AIFeatures {

// Class implementation for AIIntegration 
// This class is needed for internal implementation while AIIntegrationInterface is the public API
class AIIntegration {
private:
    void* m_integration;
    
public:
    AIIntegration() : m_integration(nullptr) {}
    
    void SetIntegration(void* integration) {
        m_integration = integration;
    }
    
    // Get general assistant model
    std::shared_ptr<LocalModels::GeneralAssistantModel> GetGeneralAssistantModel() const {
        if (m_integration != nullptr) {
            // Cast to AISystemInitializer
            AISystemInitializer* initializer = static_cast<AISystemInitializer*>(m_integration);
            if (initializer) {
                return initializer->GetGeneralAssistantModel();
            }
        }
        return nullptr;
    }
};

} // namespace AIFeatures
} // namespace iOS
