#pragma once

#include <string>
#include <chrono>
#include <functional>
#include <memory>
#include <thread>
#include <random>
#include <stdexcept>
#include <vector>
#include <map>
#include <mutex>
#include <atomic>

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

// Enhanced execution status tracking with more detailed information
struct ExecutionStatus {
    bool success;
    std::string error;
    int64_t executionTime; // in milliseconds
    std::string output;    // Captured script output
    size_t memoryUsed;     // Memory used by the script in bytes
    std::vector<std::string> warnings; // Warnings during execution
    
    ExecutionStatus() 
        : success(false), error(""), executionTime(0), memoryUsed(0) {}
    
    // Add a warning message
    void AddWarning(const std::string& warning) {
        warnings.push_back(warning);
    }
    
    // Get all warnings as a single string
    std::string GetWarningsAsString() const {
        std::string result;
        for (const auto& warning : warnings) {
            result += "WARNING: " + warning + "\n";
        }
        return result;
    }
    
    // Check if there were any warnings
    bool HasWarnings() const {
        return !warnings.empty();
    }
};

// Script execution options for customized execution behavior
struct ExecutionOptions {
    std::string chunkName;               // Name for the chunk (empty for auto-generated)
    bool enableObfuscation;              // Whether to obfuscate the script
    bool enableAntiDetection;            // Whether to use anti-detection measures
    int timeout;                         // Timeout in milliseconds (0 for no timeout)
    bool captureOutput;                  // Whether to capture script output
    bool autoRetry;                      // Whether to automatically retry on failure
    int maxRetries;                      // Maximum number of retries
    std::map<std::string, std::string> environment; // Environment variables for the script
    
    ExecutionOptions()
        : enableObfuscation(true),
          enableAntiDetection(true),
          timeout(ExecutorConfig::ScriptExecutionTimeout),
          captureOutput(true),
          autoRetry(ExecutorConfig::AutoRetryFailedExecution),
          maxRetries(ExecutorConfig::MaxAutoRetries) {}
};

// Callback types for execution events
using BeforeExecuteCallback = std::function<void(const std::string&, const ExecutionOptions&)>;
using AfterExecuteCallback = std::function<void(const std::string&, const ExecutionStatus&)>;
using OutputCallback = std::function<void(const std::string&)>;

// Global execution state
struct ExecutionState {
    static BeforeExecuteCallback beforeExecuteCallback;
    static AfterExecuteCallback afterExecuteCallback;
    static OutputCallback outputCallback;
    static std::mutex executionMutex;
    static std::atomic<bool> isExecuting;
    static std::atomic<size_t> memoryUsage;
};

// Define static members
BeforeExecuteCallback ExecutionState::beforeExecuteCallback = nullptr;
AfterExecuteCallback ExecutionState::afterExecuteCallback = nullptr;
OutputCallback ExecutionState::outputCallback = nullptr;
std::mutex ExecutionState::executionMutex;
std::atomic<bool> ExecutionState::isExecuting(false);
std::atomic<size_t> ExecutionState::memoryUsage(0);

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

// Apply environment variables to script
std::string applyEnvironment(const std::string& script, const std::map<std::string, std::string>& environment) {
    if (environment.empty()) {
        return script;
    }
    
    // Create environment setup code
    std::string envSetup = "-- Environment setup\nlocal env = {}\n";
    
    for (const auto& [key, value] : environment) {
        // Safely create environment variables
        envSetup += "env[\"" + key + "\"] = \"" + value + "\"\n";
    }
    
    // Create a function to access environment
    envSetup += "function getenv(name) return env[name] end\n\n";
    
    // Combine with the original script
    return envSetup + script;
}

// Enhanced script execution with error handling, timeouts, and anti-detection
ExecutionStatus executescript(lua_State* ls, const std::string& script, const ExecutionOptions& options = ExecutionOptions()) {
    // Set execution flag
    std::lock_guard<std::mutex> lock(ExecutionState::executionMutex);
    ExecutionState::isExecuting = true;
    
    // Create execution status
    ExecutionStatus status;
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Call before-execute callback if set
    if (ExecutionState::beforeExecuteCallback) {
        ExecutionState::beforeExecuteCallback(script, options);
    }
    
    try {
        // 1. Apply environment variables
        std::string processedScript = applyEnvironment(script, options.environment);
        
        // 2. Apply script obfuscation if enabled
        if (options.enableObfuscation) {
            // Add random dead code to confuse analysis
            processedScript = AntiDetection::Obfuscator::AddDeadCode(processedScript);
            
            // Apply more advanced obfuscation techniques
            processedScript = AntiDetection::Obfuscator::ObfuscateIdentifiers(processedScript);
            processedScript = AntiDetection::Obfuscator::AddDeadCode(processedScript);
        }
        
        // 3. Add output capture if needed
        if (options.captureOutput) {
            // Wrap output functions to capture their output
            std::string outputCapture = 
                "-- Output capture setup\n"
                "local old_print = print\n"
                "print = function(...)\n"
                "  local args = {...}\n"
                "  local result = \"\"\n"
                "  for i, v in ipairs(args) do\n"
                "    if i > 1 then result = result .. \"\\t\" end\n"
                "    result = result .. tostring(v)\n"
                "  end\n"
                "  old_print(result)\n"
                "  -- Custom output handling would go here\n"
                "end\n\n";
            
            processedScript = outputCapture + processedScript;
        }
        
        // 4. Compile the script with our enhanced encoder
        EnhancedBytecodeEncoder encoder;
        auto bc = Luau::compile(processedScript, {}, {}, &encoder);
        
        // 5. Create a random chunk name if none provided (helps avoid detection)
        std::string effectiveChunkname = options.chunkName;
        if (effectiveChunkname.empty()) {
            effectiveChunkname = generateRandomChunkName();
        }
        
        // 6. Execute with timeout if configured
        bool executionComplete = false;
        std::thread timeoutThread;
        
        if (options.timeout > 0) {
            // Create a timeout thread
            timeoutThread = std::thread([&executionComplete, timeout = options.timeout]() {
                std::this_thread::sleep_for(std::chrono::milliseconds(timeout));
                if (!executionComplete) {
                    // In a real implementation, you might need a way to interrupt the script
                    fprintf(stderr, "Script execution timed out after %d ms\n", timeout);
                }
            });
        }
        
        // 7. Execute the script with memory tracking
        size_t memBefore = lua_gc(ls, LUA_GCCOUNT, 0) * 1024;
        
        int loadResult = rluau_load(ls, effectiveChunkname.c_str(), bc.c_str(), bc.size(), 0);
        if (loadResult == 0) {
            int spawnResult = rspawn(ls);
            status.success = (spawnResult == 0);
            
            if (!status.success) {
                status.error = "Script spawning failed";
                if (lua_isstring(ls, -1)) {
                    status.error += ": ";
                    status.error += lua_tostring(ls, -1);
                    lua_pop(ls, 1);
                }
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
        
        // 8. Calculate memory usage
        size_t memAfter = lua_gc(ls, LUA_GCCOUNT, 0) * 1024;
        status.memoryUsed = (memAfter > memBefore) ? (memAfter - memBefore) : 0;
        ExecutionState::memoryUsage += status.memoryUsed;
        
        // 9. Cleanup
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
    if (!status.success && options.autoRetry) {
        static int retryCount = 0;
        if (retryCount < options.maxRetries) {
            retryCount++;
            
            // Add a warning about retry
            status.AddWarning("Execution failed, retrying (attempt " + std::to_string(retryCount) + 
                             " of " + std::to_string(options.maxRetries) + ")");
            
            // Wait a bit before retrying
            std::this_thread::sleep_for(std::chrono::milliseconds(500 * retryCount));
            
            // Create a copy of options for retry
            ExecutionOptions retryOptions = options;
            
            // Recursive retry
            status = executescript(ls, script, retryOptions);
            
            // Reset retry count on success
            if (status.success) {
                retryCount = 0;
            }
        } else {
            // Reset retry count for next script
            retryCount = 0;
            status.AddWarning("Reached maximum retry attempts");
        }
    }
    
    // Call after-execute callback if set
    if (ExecutionState::afterExecuteCallback) {
        ExecutionState::afterExecuteCallback(script, status);
    }
    
    // Reset execution flag
    ExecutionState::isExecuting = false;
    
    return status;
}

// Execute a script with default options
ExecutionStatus executescript(lua_State* ls, const std::string& script, const std::string& chunkname = "") {
    ExecutionOptions options;
    options.chunkName = chunkname;
    return executescript(ls, script, options);
}

// Set callback for before script execution
void SetBeforeExecuteCallback(BeforeExecuteCallback callback) {
    ExecutionState::beforeExecuteCallback = callback;
}

// Set callback for after script execution
void SetAfterExecuteCallback(AfterExecuteCallback callback) {
    ExecutionState::afterExecuteCallback = callback;
}

// Set callback for script output
void SetOutputCallback(OutputCallback callback) {
    ExecutionState::outputCallback = callback;
}

// Check if currently executing a script
bool IsExecuting() {
    return ExecutionState::isExecuting;
}

// Get the current memory usage
size_t GetMemoryUsage() {
    return ExecutionState::memoryUsage;
}

// Run garbage collection
size_t CollectGarbage(bool full = false) {
    // Estimate current memory
    size_t before = ExecutionState::memoryUsage;
    
    // Run GC
    if (full) {
        lua_gc(nullptr, LUA_GCCOLLECT, 0);
    } else {
        lua_gc(nullptr, LUA_GCSTEP, 100);
    }
    
    // Update and return freed memory
    size_t after = lua_gc(nullptr, LUA_GCCOUNT, 0) * 1024;
    ExecutionState::memoryUsage = after;
    
    return (before > after) ? (before - after) : 0;
}

// Reset memory usage tracking
void ResetMemoryTracking() {
    ExecutionState::memoryUsage = 0;
}

// Apply AI optimization to a script (placeholder)
std::string OptimizeScript(const std::string& script) {
    // In a real implementation, this would use AI to optimize the script
    // For now, just return the original script
    return script;
}

// Format a script according to standard style (placeholder)
std::string FormatScript(const std::string& script) {
    // In a real implementation, this would format the script
    // For now, just return the original script
    return script;
}