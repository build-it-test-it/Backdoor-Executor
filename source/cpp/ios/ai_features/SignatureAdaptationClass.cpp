// Define CI_BUILD to use stub implementations in CI environment
#define CI_BUILD

#include <string>
#include <vector>
#include <functional>
#include <memory>

// Define the EXPORT macro to ensure proper symbol visibility
#define EXPORT __attribute__((visibility("default"), used, externally_visible))

// Stub implementations of mangled name functions
extern "C" {
#ifdef CI_BUILD
    // Use the mangled name functions for CI build
    EXPORT void* _ZN3iOS10AIFeatures19SignatureAdaptationC1Ev() {
        return nullptr;  // Constructor stub
    }
    
    EXPORT void* _ZN3iOS10AIFeatures19SignatureAdaptationD1Ev() {
        return nullptr;  // Destructor stub
    }
#endif
}

// Actual class implementation
namespace iOS {
    namespace AIFeatures {
        // Forward declaration
        class SignatureAdaptation;
        
        // Class definition
        class SignatureAdaptation {
        public:
#ifndef CI_BUILD
            // Only define these in non-CI builds to avoid symbol conflicts
            SignatureAdaptation();
            ~SignatureAdaptation();
#endif
        };
        
#ifndef CI_BUILD
        // Only include actual implementations in non-CI builds
        SignatureAdaptation::SignatureAdaptation() {
            // Real constructor implementation would initialize:
            // - Detection patterns
            // - Signature database
            // - Memory scanning parameters
        }
        
        SignatureAdaptation::~SignatureAdaptation() {
            // Real destructor implementation would:
            // - Release any resources
            // - Clear signature caches
            // - Clean up detection history
        }
#endif
    }
}
