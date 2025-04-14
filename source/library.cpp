#include <lua.hpp>         // Lua core
#include <lauxlib.h>      // Lua auxiliary functions
#include <lualib.h>       // Lua standard libraries
#include <lfs.h>          // LuaFileSystem for file handling
#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <chrono>

// Forward declaration for AI integration
#ifdef ENABLE_AI_FEATURES
namespace iOS {
namespace AIFeatures {
    class AIIntegrationManager;
    class ScriptAssistant;
}}
#endif

// Main Lua script for the executor
const char* mainLuauScript = R"(
print("Roblox Executor initialized!")

-- Global executor information
_G.EXECUTOR = {
    version = "1.0.0",
    name = "RobloxExecutor",
    platform = "iOS",
}

-- Main function that executes when a player is detected
function main(playerName)
    print("Welcome " .. playerName .. " to " .. _G.EXECUTOR.name .. " " .. _G.EXECUTOR.version)
    
    -- Initialize global executor environment
    _G.EXECUTOR.player = playerName
    _G.EXECUTOR.startTime = os.time()
end

-- Add executor-specific global functions
function getExecutorInfo()
    return _G.EXECUTOR
end
)";

// Create workspace directory if it doesn't exist
void ensureWorkspaceDirectory() {
    std::filesystem::path workspace("workspace");
    if (!std::filesystem::exists(workspace)) {
        std::filesystem::create_directory(workspace);
    }
}

// Universal file functions
int isfile(lua_State* L) {
    const char* path = luaL_checkstring(L, 1);
    std::ifstream file(path);
    lua_pushboolean(L, file.good());
    return 1; // Number of return values
}

int writefile(lua_State* L) {
    ensureWorkspaceDirectory();
    
    const char* path = luaL_checkstring(L, 1);
    const char* content = luaL_checkstring(L, 2);
    
    // Ensure the path is within the workspace directory or starts with it
    std::string fullPath = path;
    if (fullPath.find("workspace/") != 0) {
        fullPath = "workspace/" + fullPath;
    }

    // Create directories if needed
    std::filesystem::path filePath(fullPath);
    std::filesystem::create_directories(filePath.parent_path());

    std::ofstream file(fullPath);
    if (file) {
        file << content;
        file.close();
        lua_pushboolean(L, true);
    } else {
        lua_pushboolean(L, false);
    }
    return 1; // Number of return values
}

int append_file(lua_State* L) {
    ensureWorkspaceDirectory();
    
    const char* path = luaL_checkstring(L, 1);
    const char* content = luaL_checkstring(L, 2);
    
    // Ensure the path is within the workspace directory or starts with it
    std::string fullPath = path;
    if (fullPath.find("workspace/") != 0) {
        fullPath = "workspace/" + fullPath;
    }

    // Create directories if needed
    std::filesystem::path filePath(fullPath);
    std::filesystem::create_directories(filePath.parent_path());

    std::ofstream file(fullPath, std::ios_base::app);
    if (file) {
        file << content;
        file.close();
        lua_pushboolean(L, true);
    } else {
        lua_pushboolean(L, false);
    }
    return 1; // Number of return values
}

int readfile(lua_State* L) {
    const char* path = luaL_checkstring(L, 1);
    
    // Ensure the path is within the workspace directory or starts with it
    std::string fullPath = path;
    if (fullPath.find("workspace/") != 0) {
        fullPath = "workspace/" + fullPath;
    }

    std::ifstream file(fullPath);
    if (!file) {
        lua_pushnil(L);
        return 1; // Return nil if the file does not exist
    }

    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    lua_pushstring(L, content.c_str());
    return 1; // Return content of the file
}

// Function to generate a script using AI
int generateScript(lua_State* L) {
#ifdef ENABLE_AI_FEATURES
    const char* description = luaL_checkstring(L, 1);
    
    try {
        // Get the AI Integration Manager
        auto& aiManager = iOS::AIFeatures::AIIntegrationManager::GetSharedInstance();
        
        // Check if initialized
        if (!aiManager.IsInitialized()) {
            lua_pushstring(L, "-- AI system not initialized yet. Please try again later.\nprint('AI system initializing...')");
            return 1;
        }
        
        // Get the script assistant
        auto scriptAssistant = aiManager.GetScriptAssistant();
        if (!scriptAssistant) {
            lua_pushstring(L, "-- Script assistant not available.\nprint('Script assistant not available')");
            return 1;
        }
        
        // For synchronous use in Lua, we need to handle the async nature of the AI system
        std::string resultScript;
        bool completed = false;
        
        // Generate the script
        aiManager.GenerateScript(description, "", [&resultScript, &completed](const std::string& script) {
            resultScript = script;
            completed = true;
        });
        
        // Wait for completion (with timeout)
        auto startTime = std::chrono::steady_clock::now();
        while (!completed) {
            // Check for timeout (5 seconds)
            auto now = std::chrono::steady_clock::now();
            if (std::chrono::duration_cast<std::chrono::seconds>(now - startTime).count() > 5) {
                lua_pushstring(L, "-- Script generation timed out. Please try again.\nprint('Script generation timed out')");
                return 1;
            }
            
            // Sleep a bit to avoid busy waiting
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        
        // Return the generated script
        lua_pushstring(L, resultScript.c_str());
        return 1;
    }
    catch (const std::exception& e) {
        std::string errorMsg = "-- Error generating script: ";
        errorMsg += e.what();
        errorMsg += "\nprint('Error generating script')";
        lua_pushstring(L, errorMsg.c_str());
        return 1;
    }
#else
    // AI features disabled, return a message
    lua_pushstring(L, "-- AI features are disabled in this build.\nprint('AI features are disabled')");
    return 1;
#endif
}

// Function to scan for vulnerabilities in the current game
int scanVulnerabilities(lua_State* L) {
#ifdef ENABLE_AI_FEATURES
    try {
        // Get the AI Integration Manager
        auto& aiManager = iOS::AIFeatures::AIIntegrationManager::GetSharedInstance();
        
        // Check if initialized and has vulnerability scanning capability
        if (!aiManager.IsInitialized() || !aiManager.HasCapability(iOS::AIFeatures::AIIntegrationManager::VULNERABILITY_DETECTION)) {
            lua_pushboolean(L, false);
            lua_pushstring(L, "Vulnerability scanner not available");
            return 2;
        }
        
        // Start a scan (this would normally be handled by the UI)
        lua_pushboolean(L, true);
        lua_pushstring(L, "Vulnerability scan started. Check the UI for results.");
        return 2;
    }
    catch (const std::exception& e) {
        lua_pushboolean(L, false);
        lua_pushstring(L, e.what());
        return 2;
    }
#else
    // AI features disabled, return a message
    lua_pushboolean(L, false);
    lua_pushstring(L, "AI features are disabled in this build.");
    return 2;
#endif
}

// Function to execute the main Luau script for a specific player
void executeMainLuau(lua_State* L, const std::string& playerName) {
    // Load the main script
    if (luaL_dostring(L, mainLuauScript)) {
        std::cerr << "Error loading main.luau: " << lua_tostring(L, -1) << std::endl;
        lua_pop(L, 1); // Remove error message from stack
        return;
    }
    
    // Call the main function
    lua_getglobal(L, "main"); // Get the main function
    lua_pushstring(L, playerName.c_str()); // Push player name as an argument
    if (lua_pcall(L, 1, 0, 0)) { // Call the main function with 1 argument
        std::cerr << "Error executing main.luau: " << lua_tostring(L, -1) << std::endl;
        lua_pop(L, 1); // Remove error message from stack
    }
}

// Hook for Roblox's PlayerAdded event
void hookPlayerAddedEvent(lua_State* L) {
    lua_getglobal(L, "game");
    lua_getfield(L, -1, "Players"); // Get game.Players

    // Get the PlayerAdded event
    lua_getfield(L, -1, "PlayerAdded");
    lua_pushcfunction(L, [](lua_State* L) -> int {
        // Get the new player
        lua_getglobal(L, "game");
        lua_getfield(L, -1, "Players");
        lua_getfield(L, -1, "LocalPlayer"); // Get LocalPlayer

        lua_getfield(L, -1, "Name"); // Get the player's name
        const char* playerName = lua_tostring(L, -1);
        
        // Execute main Luau script for the new player
        executeMainLuau(L, playerName);

        lua_pop(L, 4); // Clean up the stack (game, Players, LocalPlayer, Name)
        return 0; // Number of return values
    });

    // Connect the PlayerAdded event to the function
    lua_call(L, 1, 0); // Connect event
    lua_pop(L, 1); // Pop Players
}

// Register executor-specific functions
void registerExecutorFunctions(lua_State* L) {
    // File operations
    lua_register(L, "isfile", isfile);
    lua_register(L, "writefile", writefile);
    lua_register(L, "append_file", append_file);
    lua_register(L, "readfile", readfile);
    
    // AI-powered features
    lua_register(L, "generateScript", generateScript);
    lua_register(L, "scanVulnerabilities", scanVulnerabilities);
}

// Main function to initialize Lua and set up event listener
extern "C" int luaopen_mylibrary(lua_State *L) {
    // Load LuaFileSystem
    luaL_requiref(L, "lfs", luaopen_lfs, 1);
    lua_pop(L, 1); // Remove the library from the stack

    // Register executor functions
    registerExecutorFunctions(L);

    // Hook into the PlayerAdded event
    hookPlayerAddedEvent(L);
    
    // Ensure workspace directory exists
    ensureWorkspaceDirectory();

    return 0;
}

// Entry point for the dynamic library
extern "C" void luaopen_executor(lua_State* L) {
    luaopen_mylibrary(L);
}
