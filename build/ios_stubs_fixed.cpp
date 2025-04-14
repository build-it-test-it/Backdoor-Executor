#include <string>
#include <vector>
#include <functional>
#include <memory>

// Attributes to ensure symbols are exported and not optimized away
#define EXPORT __attribute__((visibility("default"), used, externally_visible))

// Add SystemConfiguration stubs
extern "C" {
    __attribute__((visibility("default"), used, weak))
    void* SCNetworkReachabilityCreateWithAddress_STUB(void* allocator, const struct sockaddr* address) {
        return NULL;
    }
    
    __attribute__((visibility("default"), used, weak))
    int SCNetworkReachabilityGetFlags_STUB(void* target, unsigned int* flags) {
        if (flags) *flags = 0;
        return 1;
    }
    
    __attribute__((visibility("default"), used, weak))
    int SCNetworkReachabilitySetCallback_STUB(void* target, void (*callback)(void*, int, void*), void* context) {
        return 1;
    }
    
    __attribute__((visibility("default"), used, weak))
    int SCNetworkReachabilityScheduleWithRunLoop_STUB(void* target, void* runLoop, void* runLoopMode) {
        return 1;
    }
    
    __attribute__((visibility("default"), used, weak))
    int SCNetworkReachabilityUnscheduleFromRunLoop_STUB(void* target, void* runLoop, void* runLoopMode) {
        return 1;
    }
}

// Define full classes with implementations to satisfy the linker
namespace iOS {
    // Add dummy symbols
    EXPORT void* dummy_symbol_to_force_linking = (void*)0xdeadbeef;
    
    namespace AIFeatures {
        EXPORT void* dummy_aifeatures_symbol = (void*)0xdeadbeef;
        
        namespace VulnerabilityDetection {
            EXPORT void* dummy_vulndetect_symbol = (void*)0xdeadbeef;
            
            // Define VulnerabilityDetector with nested type first since it's referenced in UI callbacks
            class VulnerabilityDetector {
            public:
                // This struct has to be fully defined here because it's used in function signatures
                struct Vulnerability {
                    std::string name;
                };
                
                EXPORT VulnerabilityDetector() {}
                EXPORT ~VulnerabilityDetector() {}
            };
        }
        
        // Define ScriptAssistant (needed to satisfy std::shared_ptr<ScriptAssistant>)
        class ScriptAssistant {
        public:
            EXPORT ScriptAssistant() {}
            EXPORT ~ScriptAssistant() {}
        };
        
        namespace LocalModels {
            EXPORT void* dummy_localmodels_symbol = (void*)0xdeadbeef;
            
            class ScriptGenerationModel {
            public:
                EXPORT ScriptGenerationModel() {}
                EXPORT ~ScriptGenerationModel() {}
                EXPORT std::string AnalyzeScript(const std::string& script) { return ""; }
                EXPORT std::string GenerateResponse(const std::string& input, const std::string& context) { return ""; }
            };
        }
        
        namespace SignatureAdaptation {
            EXPORT void* dummy_sigadapt_symbol = (void*)0xdeadbeef;
            
            struct DetectionEvent {
                std::string name;
                std::vector<unsigned char> bytes;
            };
            
            class SignatureAdaptation {
            public:
                EXPORT SignatureAdaptation() {}
                EXPORT ~SignatureAdaptation() {}
                EXPORT static void Initialize() {}
                EXPORT static void ReportDetection(const DetectionEvent& event) {}
                EXPORT static void PruneDetectionHistory() {}
                EXPORT static void ReleaseUnusedResources() {}
            };
        }
    }
    
    // Define UI classes after VulnerabilityDetector is defined
    namespace UI {
        EXPORT void* dummy_ui_symbol = (void*)0xdeadbeef;
        
        class MainViewController {
        public:
            EXPORT void SetScriptAssistant(std::shared_ptr<AIFeatures::ScriptAssistant> assistant) {}
        };
        
        class VulnerabilityViewController {
        public:
            EXPORT VulnerabilityViewController() {}
            EXPORT ~VulnerabilityViewController() {}
            
            EXPORT void Initialize() {}
            EXPORT void SetScanButtonCallback(std::function<void()> callback) {}
            // Now safe to use Vulnerability since VulnerabilityDetector is fully defined above
            EXPORT void SetExploitButtonCallback(std::function<void(AIFeatures::VulnerabilityDetection::VulnerabilityDetector::Vulnerability const&)> callback) {}
            EXPORT void SetVulnerabilityDetector(std::shared_ptr<AIFeatures::VulnerabilityDetection::VulnerabilityDetector> detector) {}
            EXPORT void StartScan(const std::string& path1, const std::string& path2) {}
            EXPORT void* GetViewController() const { return nullptr; }
        };
    }
    
    class UIController {
    public:
        EXPORT static void SetButtonVisible(bool visible) {}
        EXPORT static void Hide() {}
    };
    
    namespace AdvancedBypass {
        EXPORT void* dummy_advbypass_symbol = (void*)0xdeadbeef;
        
        class ExecutionIntegration {
        public:
            EXPORT bool Execute(const std::string& script) { return true; }
        };
        
        EXPORT bool IntegrateHttpFunctions(std::shared_ptr<ExecutionIntegration> engine) { return true; }
    }
}
