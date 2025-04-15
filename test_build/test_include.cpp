// Test including both C++ and Objective-C headers
#include "../source/cpp/ios_compat.h"
#include <string>
#include <vector>

int main() {
    // Some C++ code that uses the forward declarations
    CGRect rect;
    rect.origin.x = 10;
    rect.origin.y = 20;
    rect.size.width = 100;
    rect.size.height = 200;
    
    // Create a NSString pointer (but we can't call methods on it in C++ mode)
    NSString str = nullptr;
    
    return 0;
}
