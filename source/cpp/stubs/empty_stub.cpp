// Empty stub source file to ensure the library has at least one source file
#include <iostream>

extern "C" {
    // Some dummy function exports
    void roblox_execution_dummy_function() {
        std::cout << "Dummy function called" << std::endl;
    }
    
    // Add any other function exports needed for linking
    void* roblox_execution_get_dummy_ptr() {
        return nullptr;
    }
}
