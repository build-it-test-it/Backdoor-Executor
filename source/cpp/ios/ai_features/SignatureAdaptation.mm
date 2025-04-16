#include "SignatureAdaptation.h"
#include <iostream>

namespace iOS {
namespace AIFeatures {

// Implementation of ReportDetection
void SignatureAdaptation::ReportDetection(const DetectionEvent& event) {
    std::unique_lock<std::mutex> lock(m_mutex);
    
    // Store detection information
    std::string signatureStr;
    if (!event.m_signature.empty()) {
        signatureStr.reserve(event.m_signature.size() * 2);
        for (auto byte : event.m_signature) {
            char buf[3];
            snprintf(buf, sizeof(buf), "%02x", byte);
            signatureStr += buf;
        }
    }
    
    // Add to detection history
    m_detectionHistory.push_back(std::make_pair(signatureStr, true));
    
    // Update risk scores
    UpdateRiskScores();
    
    std::cout << "SignatureAdaptation: Detected " << event.m_detectionType 
              << " at " << event.m_timestamp << std::endl;
}

// Implementation of ReleaseUnusedResources
void SignatureAdaptation::ReleaseUnusedResources() {
    std::unique_lock<std::mutex> lock(m_mutex);
    
    // Prune old detection history
    PruneDetectionHistory();
    
    // Clear unused adaptations
    std::vector<std::string> keysToRemove;
    for (const auto& entry : m_signatureAdaptations) {
        if (m_signatureRiskScores.find(entry.first) == m_signatureRiskScores.end() ||
            m_signatureRiskScores[entry.first] < 0.1) {
            keysToRemove.push_back(entry.first);
        }
    }
    
    for (const auto& key : keysToRemove) {
        m_signatureAdaptations.erase(key);
    }
    
    std::cout << "SignatureAdaptation: Released " << keysToRemove.size() 
              << " unused resources" << std::endl;
}

// Implementation of GetMemoryUsage
uint64_t SignatureAdaptation::GetMemoryUsage() const {
    std::unique_lock<std::mutex> lock(m_mutex);
    
    // Estimate memory usage based on stored data
    uint64_t memoryUsage = 0;
    
    // Detection history
    memoryUsage += m_detectionHistory.size() * sizeof(std::pair<std::string, bool>);
    for (const auto& entry : m_detectionHistory) {
        memoryUsage += entry.first.size();
    }
    
    // Signature adaptations
    for (const auto& entry : m_signatureAdaptations) {
        memoryUsage += entry.first.size() + entry.second.size();
    }
    
    // Risk scores
    memoryUsage += m_signatureRiskScores.size() * sizeof(std::pair<std::string, double>);
    for (const auto& entry : m_signatureRiskScores) {
        memoryUsage += entry.first.size();
    }
    
    // Add base memory usage
    memoryUsage += 512 * 1024; // 512KB base usage
    
    return memoryUsage;
}

} // namespace AIFeatures
} // namespace iOS
