-- source/main.lua
-- Enhanced Roblox Executor Main Script with LED effects and AI integration

-- Import the Files module
local Files = require("Files")

-- Global configuration
local Config = {
    version = "1.2.0",
    enableLEDEffects = enableLEDEffects or true,  -- Set in C++
    enableAIFeatures = enableAIFeatures or true,  -- Set in C++
    memoryManagement = {
        autoGarbageCollection = true,
        gcInterval = 30,  -- seconds
        memoryWarningThreshold = 50 * 1024 * 1024  -- 50MB
    },
    ui = {
        defaultColorScheme = "cyberpunk",
        animationSpeed = 1.0,
        hapticFeedback = true
    }
}

-- Get the app name from the global variable set in C++
local appName = appName or "Enhanced Executor"  -- Fallback if not set

-- Print startup banner with version
print("\n===================================")
print("  " .. appName .. " v" .. Config.version)
print("  LED Effects: " .. (Config.enableLEDEffects and "Enabled" or "Disabled"))
print("  AI Features: " .. (Config.enableAIFeatures and "Enabled" or "Disabled"))
print("===================================\n")

-- Initialize the Workspace directory
if not Files.initializeWorkspace(appName) then
    print("Failed to initialize the Workspace directory.")
end

-- Apply initial LED effect to floating button
if Config.enableLEDEffects and applyLEDEffect then
    applyLEDEffect("default", 0.8)
    triggerPulseEffect()
end

-- Utility Functions
local function formatMemorySize(bytes)
    if bytes < 1024 then
        return bytes .. " B"
    elseif bytes < 1024 * 1024 then
        return string.format("%.2f KB", bytes / 1024)
    else
        return string.format("%.2f MB", bytes / (1024 * 1024))
    end
end

-- Memory Management
local MemoryManager = {
    lastGcTime = os.time(),
    
    checkMemory = function(self)
        -- Check if we have the getScriptMemoryUsage function
        if not getScriptMemoryUsage then return false end
        
        local memUsage = getScriptMemoryUsage()
        if memUsage > Config.memoryManagement.memoryWarningThreshold then
            print("WARNING: High memory usage: " .. formatMemorySize(memUsage))
            self:runGarbageCollection(true)
            return true
        end
        
        -- Run periodic GC if enabled
        if Config.memoryManagement.autoGarbageCollection then
            local currentTime = os.time()
            if currentTime - self.lastGcTime > Config.memoryManagement.gcInterval then
                self:runGarbageCollection(false)
                self.lastGcTime = currentTime
                return true
            end
        end
        
        return false
    end,
    
    runGarbageCollection = function(self, full)
        -- Check if we have the collectGarbage function
        if not collectGarbage then 
            collectgarbage(full and "collect" or "step")
            return 0
        end
        
        local before = getScriptMemoryUsage and getScriptMemoryUsage() or 0
        local freed = collectGarbage(full)
        print("Memory cleaned: " .. formatMemorySize(freed))
        return freed
    end
}

-- Script Execution
local Executor = {
    lastScript = "",
    executionHistory = {},
    
    execute = function(self, script)
        if not script or script == "" then
            print("Error: Empty script")
            return false, "Script is empty"
        end
        
        -- Save script for history
        self.lastScript = script
        table.insert(self.executionHistory, {
            script = script,
            timestamp = os.time()
        })
        
        -- Limit history size
        if #self.executionHistory > 20 then
            table.remove(self.executionHistory, 1)
        end
        
        -- Check memory before execution
        MemoryManager:checkMemory()
        
        -- Use the ExecuteScript function from C++
        local success = ExecuteScript(script)
        
        -- Report execution status
        if success then
            print("Script executed successfully!")
            
            -- Apply success LED effect if enabled
            if Config.enableLEDEffects and applyLEDEffect then
                applyLEDEffect("success", 0.8)
                triggerPulseEffect()
            end
        else
            print("Script execution failed")
            
            -- Apply error LED effect if enabled
            if Config.enableLEDEffects and applyLEDEffect then
                applyLEDEffect("error", 1.0)
                triggerPulseEffect()
            end
        end
        
        return success
    end,
    
    getLastScript = function(self)
        return self.lastScript
    end,
    
    getHistory = function(self)
        return self.executionHistory
    end
}

-- AI Integration
local AIAssistant = {
    isAvailable = Config.enableAIFeatures,
    
    generateScript = function(self, description)
        if not self.isAvailable or not generateScript then
            print("AI script generation not available")
            return "-- AI script generation not available\nprint(\"" .. description .. "\")"
        end
        
        print("Generating script from description: " .. description)
        
        -- Pulse LED effect during generation if enabled
        if Config.enableLEDEffects and triggerPulseEffect then
            triggerPulseEffect()
        end
        
        -- Call the C++ function to generate a script
        local generatedScript = generateScript(description)
        
        -- Flash success LED effect if enabled
        if Config.enableLEDEffects and applyLEDEffect then
            applyLEDEffect("success", 0.6)
            triggerPulseEffect()
        end
        
        return generatedScript
    end,
    
    optimizeScript = function(self, script)
        if not self.isAvailable or not optimizeScript then
            print("AI script optimization not available")
            return script
        end
        
        print("Optimizing script...")
        
        -- Call the C++ function to optimize the script
        local optimized = optimizeScript(script)
        
        -- Flash success LED effect if enabled
        if Config.enableLEDEffects and applyLEDEffect then
            applyLEDEffect("info", 0.5)
            triggerPulseEffect()
        end
        
        return optimized
    end,
    
    getSuggestions = function(self, script)
        if not self.isAvailable or not GetScriptSuggestions then
            return "AI suggestions not available"
        end
        
        -- Call the C++ function to get suggestions
        return GetScriptSuggestions(script)
    end
}

-- Register command-line interface functions
local function processUserCommand(command)
    if command:sub(1, 7) == "execute" then
        local script = command:sub(9)
        return Executor:execute(script)
    elseif command:sub(1, 8) == "generate" then
        local description = command:sub(10)
        local script = AIAssistant:generateScript(description)
        print("Generated script:")
        print(script)
        return true
    elseif command:sub(1, 8) == "optimize" then
        local script = Executor:getLastScript()
        if script == "" then
            print("No script to optimize")
            return false
        end
        local optimized = AIAssistant:optimizeScript(script)
        Executor.lastScript = optimized
        print("Script optimized")
        return true
    elseif command == "memory" then
        MemoryManager:checkMemory()
        return true
    elseif command == "gc" then
        local freed = MemoryManager:runGarbageCollection(true)
        print("Garbage collection completed. Freed: " .. formatMemorySize(freed))
        return true
    elseif command == "help" then
        print("Available commands:")
        print("  execute <script> - Execute a Lua script")
        print("  generate <description> - Generate script from description using AI")
        print("  optimize - Optimize the last executed script")
        print("  memory - Check memory usage")
        print("  gc - Run garbage collection")
        print("  help - Show this help message")
        return true
    else
        print("Unknown command. Type 'help' for available commands.")
        return false
    end
end

-- Export public API
return {
    -- Core functionality
    execute = function(script) return Executor:execute(script) end,
    getLastScript = function() return Executor:getLastScript() end,
    getExecutionHistory = function() return Executor:getHistory() end,
    
    -- Memory management
    checkMemory = function() return MemoryManager:checkMemory() end,
    runGarbageCollection = function(full) return MemoryManager:runGarbageCollection(full) end,
    
    -- AI features
    generateScript = function(description) return AIAssistant:generateScript(description) end,
    optimizeScript = function(script) return AIAssistant:optimizeScript(script) end,
    getScriptSuggestions = function(script) return AIAssistant:getSuggestions(script) end,
    
    -- Command processing
    processCommand = processUserCommand,
    
    -- Configuration
    config = Config
}
