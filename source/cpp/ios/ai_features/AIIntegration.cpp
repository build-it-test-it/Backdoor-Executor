#include "AIIntegration.h"
#include "AISystemInitializer.h"
#include "local_models/GeneralAssistantModel.h"
#include <iostream>

namespace iOS {
namespace AIFeatures {

// Get general assistant model
std::shared_ptr<LocalModels::GeneralAssistantModel> AIIntegration::GetGeneralAssistantModel() const {
    if (m_integration != nullptr) {
        // Cast to AISystemInitializer
        AISystemInitializer* initializer = static_cast<AISystemInitializer*>(m_integration);
        if (initializer) {
            return initializer->GetGeneralAssistantModel();
        }
    }
    return nullptr;
}

} // namespace AIFeatures
} // namespace iOS
