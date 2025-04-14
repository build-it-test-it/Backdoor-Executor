#include "LoadstringSupport.h"
#include <iostream>
#include <sstream>
#include <random>
#include <ctime>
#include <iomanip>  // For std::setw and std::setfill
#import <Foundation/Foundation.h>

namespace iOS {
namespace AdvancedBypass {

    // Constructor
    LoadstringSupport::LoadstringSupport(Mode mode)
        : m_mode(mode) {
    }
    
    // Destructor
    LoadstringSupport::~LoadstringSupport() {
        Clear();
    }
    
    // Generate loadstring code for a script
    LoadstringSupport::LoadResult LoadstringSupport::GenerateLoadstring(
        const std::string& code, const std::string& chunkName) {
        
        try {
            // Generate a function ID for this loadstring
            std::string functionId = GenerateFunctionId();
            
            // Generate the loadstring code based on the current mode
            std::string loadstringCode;
            
            switch (m_mode) {
                case Mode::Direct:
                    loadstringCode = GenerateDirectLoadstring(code, chunkName);
                    break;
                    
                case Mode::StringEncoding:
                    loadstringCode = GenerateEncodedLoadstring(code, chunkName);
                    break;
                    
                case Mode::BytecodeCompile:
                    loadstringCode = GenerateBytecodeLoadstring(code, chunkName);
                    break;
                    
                case Mode::ProtectedCall:
                default:
                    loadstringCode = GenerateProtectedLoadstring(code, chunkName);
                    break;
            }
            
            // Store the function ID
            m_funcs.push_back(functionId);
            
            // Return success
            return LoadResult(true, "", "", functionId);
        } catch (const std::exception& e) {
            // Return failure
            return LoadResult(false, e.what());
        }
    }
    
    // Generate code to execute a previously loaded function
    std::string LoadstringSupport::GenerateExecuteCode(const std::string& functionId) {
        // Check if function ID exists
        auto it = std::find(m_funcs.begin(), m_funcs.end(), functionId);
        
        if (it == m_funcs.end()) {
            // Function ID not found
            return "error('Function ID not found: " + functionId + "')";
        }
        
        // Generate execution code
        std::stringstream code;
        
        code << "-- Execute loaded function\n";
        code << "if " << functionId << " then\n";
        code << "    local success, result = pcall(" << functionId << ")\n";
        code << "    if not success then\n";
        code << "        print('Error executing function: ' .. tostring(result))\n";
        code << "    end\n";
        code << "else\n";
        code << "    print('Function not found: " << functionId << "')\n";
        code << "end\n";
        
        return code.str();
    }
    
    // Generate complete loadstring and execute code
    std::string LoadstringSupport::GenerateLoadAndExecute(
        const std::string& code, const std::string& chunkName) {
        
        // Generate the loadstring
        LoadResult result = GenerateLoadstring(code, chunkName);
        
        if (!result.m_success) {
            // Return error code
            return "print('Error generating loadstring: " + result.m_error + "')";
        }
        
        // Generate execution code
        std::string executeCode = GenerateExecuteCode(result.m_functionId);
        
        // Combine into a single string
        return executeCode;
    }
    
    // Set the loadstring mode
    void LoadstringSupport::SetMode(Mode mode) {
        m_mode = mode;
    }
    
    // Get the current loadstring mode
    LoadstringSupport::Mode LoadstringSupport::GetMode() const {
        return m_mode;
    }
    
    // Clear stored function IDs and bytecode cache
    void LoadstringSupport::Clear() {
        m_funcs.clear();
        m_bytecodeCache.clear();
    }
    
    // Generate a direct loadstring implementation
    std::string LoadstringSupport::GenerateDirectLoadstring(
        const std::string& code, const std::string& chunkName) {
        
        // Create a function ID
        std::string functionId = GenerateFunctionId();
        
        // Prepare the chunk name
        std::string effectiveChunkName = chunkName.empty() ? "chunk" : chunkName;
        
        // Escape any quotes in the code
        std::string escapedCode = code;
        size_t pos = 0;
        while ((pos = escapedCode.find('"', pos)) != std::string::npos) {
            escapedCode.replace(pos, 1, "\\\"");
            pos += 2;
        }
        
        // Generate the loadstring code
        std::stringstream loadCode;
        
        loadCode << "-- Direct loadstring implementation\n";
        loadCode << "local " << functionId << " = loadstring(\"" << escapedCode << "\", \"" << effectiveChunkName << "\")\n";
        
        return loadCode.str();
    }
    
    // Generate an encoded loadstring implementation
    std::string LoadstringSupport::GenerateEncodedLoadstring(
        const std::string& code, const std::string& chunkName) {
        
        // Create a function ID
        std::string functionId = GenerateFunctionId();
        
        // Prepare the chunk name
        std::string effectiveChunkName = chunkName.empty() ? "chunk" : chunkName;
        
        // Obfuscate the code
        std::string obfuscatedCode = ObfuscateString(code);
        
        // Generate the loadstring code
        std::stringstream loadCode;
        
        loadCode << "-- Encoded loadstring implementation\n";
        loadCode << "local function _decode(str)\n";
        loadCode << "    local result = \"\"\n";
        loadCode << "    for i = 1, #str, 2 do\n";
        loadCode << "        local hex = str:sub(i, i+1)\n";
        loadCode << "        result = result .. string.char(tonumber(hex, 16))\n";
        loadCode << "    end\n";
        loadCode << "    return result\n";
        loadCode << "end\n\n";
        
        loadCode << "local encoded = \"" << obfuscatedCode << "\"\n";
        loadCode << "local decoded = _decode(encoded)\n";
        loadCode << "local " << functionId << " = loadstring(decoded, \"" << effectiveChunkName << "\")\n";
        
        return loadCode.str();
    }
    
    // Generate a bytecode loadstring implementation
    std::string LoadstringSupport::GenerateBytecodeLoadstring(
        const std::string& code, const std::string& chunkName) {
        
        // Create a function ID
        std::string functionId = GenerateFunctionId();
        
        // Prepare the chunk name
        std::string effectiveChunkName = chunkName.empty() ? "chunk" : chunkName;
        
        // In a real implementation, we would compile the code to bytecode here
        // For this implementation, we'll just use an encoded string as a placeholder
        std::string obfuscatedCode = ObfuscateString(code);
        
        // Store in bytecode cache
        m_bytecodeCache[functionId] = obfuscatedCode;
        
        // Generate the loadstring code
        std::stringstream loadCode;
        
        loadCode << "-- Bytecode loadstring implementation\n";
        loadCode << "local function _decode(str)\n";
        loadCode << "    local result = \"\"\n";
        loadCode << "    for i = 1, #str, 2 do\n";
        loadCode << "        local hex = str:sub(i, i+1)\n";
        loadCode << "        result = result .. string.char(tonumber(hex, 16))\n";
        loadCode << "    end\n";
        loadCode << "    return result\n";
        loadCode << "end\n\n";
        
        loadCode << "local encoded = \"" << obfuscatedCode << "\"\n";
        loadCode << "local decoded = _decode(encoded)\n";
        loadCode << "local " << functionId << " = loadstring(decoded, \"" << effectiveChunkName << "\")\n";
        
        return loadCode.str();
    }
    
    // Generate a protected loadstring implementation
    std::string LoadstringSupport::GenerateProtectedLoadstring(
        const std::string& code, const std::string& chunkName) {
        
        // Create a function ID
        std::string functionId = GenerateFunctionId();
        
        // Prepare the chunk name
        std::string effectiveChunkName = chunkName.empty() ? "chunk" : chunkName;
        
        // Obfuscate the code
        std::string obfuscatedCode = ObfuscateString(code);
        
        // Generate the loadstring code
        std::stringstream loadCode;
        
        loadCode << "-- Protected loadstring implementation\n";
        loadCode << "local function _decode(str)\n";
        loadCode << "    local result = \"\"\n";
        loadCode << "    for i = 1, #str, 2 do\n";
        loadCode << "        local hex = str:sub(i, i+1)\n";
        loadCode << "        result = result .. string.char(tonumber(hex, 16))\n";
        loadCode << "    end\n";
        loadCode << "    return result\n";
        loadCode << "end\n\n";
        
        loadCode << "local encoded = \"" << obfuscatedCode << "\"\n";
        loadCode << "local decoded = _decode(encoded)\n";
        loadCode << "local success, result = pcall(loadstring, decoded, \"" << effectiveChunkName << "\")\n";
        loadCode << "if success then\n";
        loadCode << "    local " << functionId << " = result\n";
        loadCode << "else\n";
        loadCode << "    print('Error loading string: ' .. tostring(result))\n";
        loadCode << "    local " << functionId << " = nil\n";
        loadCode << "end\n";
        
        return loadCode.str();
    }
    
    // Obfuscate a string
    std::string LoadstringSupport::ObfuscateString(const std::string& str) {
        // Convert string to hex representation
        std::stringstream hexStream;
        
        for (char c : str) {
            hexStream << std::hex << std::setw(2) << std::setfill('0') << (int)(unsigned char)c;
        }
        
        return hexStream.str();
    }
    
    // Generate a unique function ID
    std::string LoadstringSupport::GenerateFunctionId() {
        // Create a random ID with timestamp for uniqueness
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_int_distribution<> dis(1000, 9999);
        
        // Use time and random number for uniqueness
        std::time_t t = std::time(nullptr);
        int random = dis(gen);
        
        // Create the ID
        std::stringstream idStream;
        idStream << "_F" << t << "_" << random;
        
        return idStream.str();
    }
    
    // Get code for a loadstring implementation
    std::string LoadstringSupport::GetLoadstringImplementation() {
        // This provides a full implementation of loadstring that can be used in the Lua environment
        
        return R"(
-- Comprehensive loadstring implementation
local _G_loadstring = loadstring  -- Store original loadstring if available

-- Create custom loadstring implementation
function loadstring(code, chunkname)
    -- Use original loadstring if available
    if _G_loadstring then
        return _G_loadstring(code, chunkname)
    end
    
    -- Fallback implementation for environments where loadstring is not available
    
    -- For WebKit/JavaScript environments
    if window and window.LuaJSBridge and window.LuaJSBridge.compileString then
        return window.LuaJSBridge.compileString(code, chunkname or "loadstring")
    end
    
    -- Check for other compatible environments
    if js and js.loadstring then
        return js.loadstring(code, chunkname)
    end
    
    -- Attempt to use load function if available
    if load then
        return load(code, chunkname)
    end
    
    -- Last resort - create a function that returns an error
    return function()
        error("loadstring is not available in this environment", 2)
    end
end

-- Enhanced loadstring with error handling
function safeLoadstring(code, chunkname)
    local success, func = pcall(loadstring, code, chunkname)
    if success then
        return func
    else
        return nil, func  -- func contains the error message
    end
end

-- Execute string with error handling
function execstring(code, chunkname)
    local func, err = safeLoadstring(code, chunkname)
    if not func then
        return false, "Compilation error: " .. tostring(err)
    end
    
    local success, result = pcall(func)
    if not success then
        return false, "Runtime error: " .. tostring(result)
    end
    
    return true, result
end

-- Return the implementations
return {
    loadstring = loadstring,
    safeLoadstring = safeLoadstring,
    execstring = execstring
}
)";
    }
    
    // Convert mode enum to string
    std::string LoadstringSupport::ModeToString(Mode mode) {
        switch (mode) {
            case Mode::Direct:
                return "Direct";
                
            case Mode::StringEncoding:
                return "StringEncoding";
                
            case Mode::BytecodeCompile:
                return "BytecodeCompile";
                
            case Mode::ProtectedCall:
                return "ProtectedCall";
                
            default:
                return "Unknown";
        }
    }
    
    // Get a description of a mode
    std::string LoadstringSupport::GetModeDescription(Mode mode) {
        switch (mode) {
            case Mode::Direct:
                return "Direct loadstring execution without additional protection";
                
            case Mode::StringEncoding:
                return "Encodes the string before loading to avoid detection";
                
            case Mode::BytecodeCompile:
                return "Compiles to bytecode for execution to avoid string scanning";
                
            case Mode::ProtectedCall:
                return "Uses protected call (pcall) for error handling";
                
            default:
                return "Unknown mode";
        }
    }

} // namespace AdvancedBypass
} // namespace iOS
