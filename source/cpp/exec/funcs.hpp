#pragma once

#include <string>
#include <chrono>
#include <functional>
#include <memory>
#include <thread>
#include <random>
#include <stdexcept>

#include "../globals.hpp"
#include "../memory/mem.hpp"
#include "../anti_detection/obfuscator.hpp"
#include "../anti_detection/vm_detect.hpp"

#include "../luau/lua.h"
#include "../luau/lstate.h"
#include "../luau/Luau/Compiler.h"
#include "../luau/Luau/BytecodeBuilder.h"

// Enhanced bytecode encoder with randomization for anti-detection
class EnhancedBytecodeEncoder : public Luau::BytecodeEncoder {
private:
    // Random multiplier for better obfuscation
    uint8_t multiplier;
    
    // Random generator
    static std::mt19937& GetRNG() {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        return gen;
    }
    
public:
    EnhancedBytecodeEncoder() {
        // Generate a random odd multiplier between 1 and 255
        std::uniform_int_distribution<> dist(1, 127);
        multiplier = dist(GetRNG()) * 2 + 1; // Ensures it's odd for proper decoding
    }
    
    std::uint8_t encodeOp(const std::uint8_t Opcode) override {
        // Use a different multiplication factor each time
        return Opcode * multiplier;
    }
    
    // Getter for the multiplier (used for decoding)
    uint8_t getMultiplier() const {
        return multiplier;
    }
};

// Function pointers to Roblox functions
lua_State* (*rlua_getmainstate)(std::uintptr_t scriptcontext, std::uintptr_t identity, std::uintptr_t script);
lua_State* (*rlua_newthread)(lua_State* rL);
int (*rluau_load)(lua_State* rL, const char* chunkname, const char* code, size_t codesize, int env);
int (*rspawn)(lua_State* rL);

// Execution status tracking
struct ExecutionStatus {
    bool success;
    std::string error;
    int64_t executionTime; // in milliseconds
    
    ExecutionStatus() : success(false), error(""), executionTime(0) {}
};

// Initialize Roblox function pointers with fallback and retry mechanisms
void initfuncs() {
    // Apply anti-debug measures before accessing sensitive functions
    if (ExecutorConfig::EnableAntiDetection) {
        AntiDetection::AntiDebug::ApplyAntiTamperingMeasures();
    }
    
    // Check for VM environment
    if (ExecutorConfig::EnableVMDetection) {
        AntiDetection::VMDetection::HandleVMDetection();
    }

    // Get function addresses with retry logic
    int maxRetries = 3;
    bool success = false;
    
    for (int attempt = 0; attempt < maxRetries && !success; attempt++) {
        try {
            // Get the addresses using our new dynamic address resolution
            rlua_getmainstate = reinterpret_cast<lua_State*(*)(std::uintptr_t,std::uintptr_t,std::uintptr_t)>(getAddress(getstate_addy));
            rlua_newthread = reinterpret_cast<lua_State*(*)(lua_State*)>(getAddress(newthread_addy));
            rluau_load = reinterpret_cast<int(*)(lua_State*,const char*,const char*,size_t,int)>(getAddress(luauload_addy));
            rspawn = reinterpret_cast<int(*)(lua_State*)>(getAddress(spawn_addy));
            
            // Validate function pointers
            if (!rlua_getmainstate || !rlua_newthread || !rluau_load || !rspawn) {
                throw std::runtime_error("Failed to resolve function addresses");
            }
            
            success = true;
        }
        catch (const std::exception& e) {
            // If we failed, wait a bit and retry
            if (attempt < maxRetries - 1) {
                std::this_thread::sleep_for(std::chrono::milliseconds(500 * (attempt + 1)));
                
                // Reset the cache to force re-scanning
                AddressCache::ResetCache();
            }
        }
    }
    
    if (!success) {
        // Log the error but don't throw - we'll handle failures during execution
        fprintf(stderr, "Failed to initialize function pointers after multiple attempts\n");
    }
}

// Generate a random chunk name to avoid detection
std::string generateRandomChunkName() {
    static const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, sizeof(charset) - 2);
    
    std::string randomName = "Script_";
    for (int i = 0; i < 8; i++) {
        randomName += charset[dist(gen)];
    }
    
    return randomName;
}

// Enhanced script execution with error handling, timeouts, and anti-detection
ExecutionStatus executescript(lua_State* ls, const std::string& script, const std::string& chunkname = "") {
    ExecutionStatus status;
    auto startTime = std::chrono::high_resolution_clock::now();
    
    try {
        // 1. Apply script obfuscation if enabled
        std::string processedScript = script;
        if (ExecutorConfig::EnableScriptObfuscation) {
            // Add random dead code to confuse analysis
            processedScript = AntiDetection::Obfuscator::AddDeadCode(processedScript);
        }
        
        // 2. Compile the script with our enhanced encoder
        EnhancedBytecodeEncoder encoder;
        auto bc = Luau::compile(processedScript, {}, {}, &encoder);
        
        // 3. Create a random chunk name if none provided (helps avoid detection)
        std::string effectiveChunkname = chunkname;
        if (effectiveChunkname.empty()) {
            effectiveChunkname = generateRandomChunkName();
        }
        
        // 4. Execute with timeout if configured
        bool executionComplete = false;
        std::thread timeoutThread;
        
        if (ExecutorConfig::ScriptExecutionTimeout > 0) {
            // Create a timeout thread
            timeoutThread = std::thread([&executionComplete, timeout = ExecutorConfig::ScriptExecutionTimeout]() {
                std::this_thread::sleep_for(std::chrono::milliseconds(timeout));
                if (!executionComplete) {
                    // In a real implementation, you might need a way to interrupt the script
                    // This is complex and often requires injecting a yield point
                    fprintf(stderr, "Script execution timed out after %d ms\n", timeout);
                }
            });
        }
        
        // 5. Execute the script
        int loadResult = rluau_load(ls, effectiveChunkname.c_str(), bc.c_str(), bc.size(), 0);
        if (loadResult == 0) {
            int spawnResult = rspawn(ls);
            status.success = (spawnResult == 0);
            
            if (!status.success) {
                status.error = "Script spawning failed";
            }
        } else {
            // Get the error message from the stack
            status.error = "Script loading failed: ";
            if (lua_isstring(ls, -1)) {
                status.error += lua_tostring(ls, -1);
            } else {
                status.error += "Unknown error";
            }
            lua_pop(ls, 1); // Remove error message from stack
        }
        
        // 6. Cleanup
        executionComplete = true;
        if (timeoutThread.joinable()) {
            timeoutThread.join();
        }
    }
    catch (const std::exception& e) {
        status.success = false;
        status.error = "Exception during execution: ";
        status.error += e.what();
    }
    
    // Calculate execution time
    auto endTime = std::chrono::high_resolution_clock::now();
    status.executionTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    
    // Auto-retry on failure if enabled
    if (!status.success && ExecutorConfig::AutoRetryFailedExecution) {
        static int retryCount = 0;
        if (retryCount < ExecutorConfig::MaxAutoRetries) {
            retryCount++;
            
            // Wait a bit before retrying
            std::this_thread::sleep_for(std::chrono::milliseconds(500 * retryCount));
            
            // Recursive retry
            status = executescript(ls, script, chunkname);
            
            // Reset retry count on success
            if (status.success) {
                retryCount = 0;
            }
        } else {
            // Reset retry count for next script
            retryCount = 0;
        }
    }
    
    return status;
}