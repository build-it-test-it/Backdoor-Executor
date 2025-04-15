#include "ScriptAssistant.h"
#include <iostream>

namespace iOS {
namespace AIFeatures {

// Implementation of ReleaseUnusedResources
void ScriptAssistant::ReleaseUnusedResources() {
    std::cout << "ScriptAssistant: Releasing unused resources" << std::endl;
    
    // Clear conversation history beyond a certain limit
    TrimConversationHistory();
    
    // Release templates that haven't been used recently
    if (m_scriptTemplates.size() > 20) {
        m_scriptTemplates.resize(20);
    }
}

// Implementation of GetMemoryUsage
uint64_t ScriptAssistant::GetMemoryUsage() const {
    // Estimate memory usage based on stored data
    uint64_t memoryUsage = 0;
    
    // Conversation history
    for (const auto& message : m_conversationHistory) {
        memoryUsage += message.m_content.size();
    }
    
    // Script templates
    for (const auto& tmpl : m_scriptTemplates) {
        memoryUsage += tmpl.m_name.size() + tmpl.m_description.size() + tmpl.m_code.size();
    }
    
    // Add base memory usage
    memoryUsage += 1024 * 1024; // 1MB base usage
    
    return memoryUsage;
}

} // namespace AIFeatures
} // namespace iOS
