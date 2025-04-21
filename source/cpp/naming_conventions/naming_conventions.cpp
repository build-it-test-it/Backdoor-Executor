#include "naming_conventions.h"
#include <iostream>
#include <algorithm>

namespace RobloxExecutor {
namespace NamingConventions {

// Singleton instance implementation
NamingConventionManager& NamingConventionManager::GetInstance() {
    static NamingConventionManager instance;
    return instance;
}

// Constructor
NamingConventionManager::NamingConventionManager()
    : m_enableUNC(true),
      m_enableSNC(true),
      m_enableCustom(true),
      m_initialized(false) {
}

// Initialize the naming convention manager
bool NamingConventionManager::Initialize(bool enableUNC, bool enableSNC) {
    if (m_initialized) {
        std::cout << "NamingConventionManager: Already initialized" << std::endl;
        return true;
    }
    
    m_enableUNC = enableUNC;
    m_enableSNC = enableSNC;
    
    // Initialize naming conventions
    if (m_enableUNC) {
        InitializeUNC();
    }
    
    if (m_enableSNC) {
        InitializeSNC();
    }
    
    m_initialized = true;
    std::cout << "NamingConventionManager: Initialized with UNC=" 
              << (m_enableUNC ? "enabled" : "disabled")
              << ", SNC=" << (m_enableSNC ? "enabled" : "disabled") << std::endl;
    
    return true;
}

// Register a function alias
bool NamingConventionManager::RegisterAlias(const std::string& originalName, 
                                          const std::string& aliasName,
                                          ConventionType convention, 
                                          const std::string& description) {
    // Check if the alias already exists
    auto it = m_aliasMap.find(aliasName);
    if (it != m_aliasMap.end()) {
        // Alias already exists, check if it points to the same original function
        if (it->second != originalName) {
            std::cerr << "NamingConventionManager: Alias '" << aliasName 
                      << "' already registered for function '" << it->second 
                      << "', cannot register for '" << originalName << "'" << std::endl;
            return false;
        }
        
        // Alias already registered for this function, nothing to do
        return true;
    }
    
    // Register the alias
    m_aliasMap[aliasName] = originalName;
    
    // Add to original-to-aliases map
    FunctionAlias alias(originalName, aliasName, convention, description);
    m_originalToAliases[originalName].push_back(alias);
    
    return true;
}

// Register multiple aliases for a function
bool NamingConventionManager::RegisterAliases(const std::string& originalName, 
                                            const std::vector<std::string>& aliases,
                                            ConventionType convention,
                                            const std::string& description) {
    bool success = true;
    
    for (const auto& alias : aliases) {
        if (!RegisterAlias(originalName, alias, convention, description)) {
            success = false;
        }
    }
    
    return success;
}

// Resolve a function name to its original name
std::string NamingConventionManager::ResolveFunction(const std::string& functionName) const {
    // Check if the function name is an alias
    auto it = m_aliasMap.find(functionName);
    if (it != m_aliasMap.end()) {
        return it->second;
    }
    
    // Not an alias, return the original name
    return functionName;
}

// Check if a function name is an alias
bool NamingConventionManager::IsAlias(const std::string& functionName) const {
    return m_aliasMap.find(functionName) != m_aliasMap.end();
}

// Get all aliases for a function
std::vector<FunctionAlias> NamingConventionManager::GetAliases(const std::string& originalName) const {
    auto it = m_originalToAliases.find(originalName);
    if (it != m_originalToAliases.end()) {
        return it->second;
    }
    
    return std::vector<FunctionAlias>();
}

// Get all function aliases
std::vector<FunctionAlias> NamingConventionManager::GetAllAliases() const {
    std::vector<FunctionAlias> allAliases;
    
    for (const auto& pair : m_originalToAliases) {
        allAliases.insert(allAliases.end(), pair.second.begin(), pair.second.end());
    }
    
    return allAliases;
}

// Enable or disable a naming convention
void NamingConventionManager::EnableConvention(ConventionType convention, bool enable) {
    switch (convention) {
        case ConventionType::UNC:
            m_enableUNC = enable;
            break;
        case ConventionType::SNC:
            m_enableSNC = enable;
            break;
        case ConventionType::Custom:
            m_enableCustom = enable;
            break;
    }
}

// Check if a naming convention is enabled
bool NamingConventionManager::IsConventionEnabled(ConventionType convention) const {
    switch (convention) {
        case ConventionType::UNC:
            return m_enableUNC;
        case ConventionType::SNC:
            return m_enableSNC;
        case ConventionType::Custom:
            return m_enableCustom;
        default:
            return false;
    }
}

// Initialize UNC naming convention
void NamingConventionManager::InitializeUNC() {
    std::cout << "NamingConventionManager: Initializing UNC naming convention" << std::endl;
    
    // Cache functions
    RegisterAlias("cloneref", "cache.replace", ConventionType::UNC, "Replace an instance reference with another");
    RegisterAlias("cache.invalidate", "cache.invalidate", ConventionType::UNC, "Invalidate an instance in the cache");
    RegisterAlias("cache.iscached", "cache.iscached", ConventionType::UNC, "Check if an instance is cached");
    RegisterAlias("cloneref", "cloneref", ConventionType::UNC, "Clone an instance reference");
    RegisterAlias("compareinstances", "compareinstances", ConventionType::UNC, "Compare two instances for equality");
    
    // Closure functions
    RegisterAlias("checkcaller", "checkcaller", ConventionType::UNC, "Check if the caller is from the executor");
    RegisterAlias("clonefunction", "clonefunction", ConventionType::UNC, "Clone a function");
    RegisterAlias("getcallingscript", "getcallingscript", ConventionType::UNC, "Get the script that called the current function");
    RegisterAlias("getscriptclosure", "getscriptclosure", ConventionType::UNC, "Get the closure of a script");
    RegisterAliases("getscriptclosure", {"getscriptfunction"}, ConventionType::UNC, "Get the closure of a script");
    RegisterAlias("hookfunction", "hookfunction", ConventionType::UNC, "Hook a function");
    RegisterAliases("hookfunction", {"replaceclosure"}, ConventionType::UNC, "Hook a function");
    RegisterAlias("iscclosure", "iscclosure", ConventionType::UNC, "Check if a function is a C closure");
    RegisterAlias("islclosure", "islclosure", ConventionType::UNC, "Check if a function is a Lua closure");
    RegisterAlias("isexecutorclosure", "isexecutorclosure", ConventionType::UNC, "Check if a function is an executor closure");
    RegisterAliases("isexecutorclosure", {"checkclosure", "isourclosure"}, ConventionType::UNC, "Check if a function is an executor closure");
    RegisterAlias("loadstring", "loadstring", ConventionType::UNC, "Load a string as a function");
    
    // Metatable functions
    RegisterAlias("getrawmetatable", "getrawmetatable", ConventionType::UNC, "Get the raw metatable of an object");
    RegisterAlias("hookmetamethod", "hookmetamethod", ConventionType::UNC, "Hook a metamethod");
    RegisterAlias("getnamecallmethod", "getnamecallmethod", ConventionType::UNC, "Get the name of the method being called");
    RegisterAlias("isreadonly", "isreadonly", ConventionType::UNC, "Check if a table is read-only");
    RegisterAlias("setrawmetatable", "setrawmetatable", ConventionType::UNC, "Set the raw metatable of an object");
    RegisterAlias("setreadonly", "setreadonly", ConventionType::UNC, "Set whether a table is read-only");
    
    // Miscellaneous functions
    RegisterAlias("identifyexecutor", "identifyexecutor", ConventionType::UNC, "Identify the executor");
    RegisterAliases("identifyexecutor", {"getexecutorname"}, ConventionType::UNC, "Identify the executor");
    RegisterAlias("lz4compress", "lz4compress", ConventionType::UNC, "Compress data using LZ4");
    RegisterAlias("lz4decompress", "lz4decompress", ConventionType::UNC, "Decompress data using LZ4");
    RegisterAlias("messagebox", "messagebox", ConventionType::UNC, "Display a message box");
    RegisterAlias("queue_on_teleport", "queue_on_teleport", ConventionType::UNC, "Queue a script to run after teleporting");
    RegisterAliases("queue_on_teleport", {"queueonteleport"}, ConventionType::UNC, "Queue a script to run after teleporting");
    RegisterAlias("request", "request", ConventionType::UNC, "Send an HTTP request");
    RegisterAliases("request", {"http.request", "http_request"}, ConventionType::UNC, "Send an HTTP request");
    RegisterAlias("setclipboard", "setclipboard", ConventionType::UNC, "Set the clipboard content");
    RegisterAliases("setclipboard", {"toclipboard"}, ConventionType::UNC, "Set the clipboard content");
    RegisterAlias("setfpscap", "setfpscap", ConventionType::UNC, "Set the FPS cap");
    RegisterAlias("join", "join", ConventionType::UNC, "Join a game");
    RegisterAliases("join", {"joingame", "joinplace", "joinserver"}, ConventionType::UNC, "Join a game");
    RegisterAlias("gethwid", "gethwid", ConventionType::UNC, "Get the hardware ID");
    
    // Script functions
    RegisterAlias("getgc", "getgc", ConventionType::UNC, "Get the garbage collector");
    RegisterAlias("getgenv", "getgenv", ConventionType::UNC, "Get the global environment");
    RegisterAlias("getloadedmodules", "getloadedmodules", ConventionType::UNC, "Get loaded modules");
    RegisterAlias("getrenv", "getrenv", ConventionType::UNC, "Get the Roblox environment");
    RegisterAlias("getrunningscripts", "getrunningscripts", ConventionType::UNC, "Get running scripts");
    RegisterAlias("getscriptbytecode", "getscriptbytecode", ConventionType::UNC, "Get the bytecode of a script");
    RegisterAliases("getscriptbytecode", {"dumpstring"}, ConventionType::UNC, "Get the bytecode of a script");
    RegisterAlias("getscripthash", "getscripthash", ConventionType::UNC, "Get the hash of a script");
    RegisterAlias("getscripts", "getscripts", ConventionType::UNC, "Get all scripts");
    RegisterAlias("getsenv", "getsenv", ConventionType::UNC, "Get the environment of a script");
    RegisterAlias("getthreadidentity", "getthreadidentity", ConventionType::UNC, "Get the identity of the current thread");
    RegisterAliases("getthreadidentity", {"getidentity", "getthreadcontext"}, ConventionType::UNC, "Get the identity of the current thread");
    RegisterAlias("setthreadidentity", "setthreadidentity", ConventionType::UNC, "Set the identity of the current thread");
    RegisterAliases("setthreadidentity", {"setidentity", "setthreadcontext"}, ConventionType::UNC, "Set the identity of the current thread");
    
    // Drawing functions
    RegisterAlias("Drawing", "Drawing", ConventionType::UNC, "Drawing library");
    RegisterAlias("Drawing.new", "Drawing.new", ConventionType::UNC, "Create a new drawing object");
    RegisterAlias("Drawing.Fonts", "Drawing.Fonts", ConventionType::UNC, "Drawing fonts");
    RegisterAlias("isrenderobj", "isrenderobj", ConventionType::UNC, "Check if an object is a render object");
    RegisterAlias("cleardrawcache", "cleardrawcache", ConventionType::UNC, "Clear the drawing cache");
    
    // WebSocket functions
    RegisterAlias("WebSocket", "WebSocket", ConventionType::UNC, "WebSocket library");
    RegisterAlias("WebSocket.connect", "WebSocket.connect", ConventionType::UNC, "Connect to a WebSocket server");
    
    // Player functions
    RegisterAlias("getplayer", "getplayer", ConventionType::UNC, "Get a player");
    RegisterAlias("getlocalplayer", "getlocalplayer", ConventionType::UNC, "Get the local player");
    RegisterAlias("getplayers", "getplayers", ConventionType::UNC, "Get all players");
    RegisterAlias("runanimation", "runanimation", ConventionType::UNC, "Run an animation");
    RegisterAliases("runanimation", {"playanimation"}, ConventionType::UNC, "Run an animation");
}

// Initialize SNC naming convention
void NamingConventionManager::InitializeSNC() {
    std::cout << "NamingConventionManager: Initializing SNC naming convention" << std::endl;
    
    // Cache functions
    RegisterAlias("cloneref", "cache.replace", ConventionType::SNC, "Replace an instance reference with another");
    RegisterAlias("cache.invalidate", "cache.invalidate", ConventionType::SNC, "Invalidate an instance in the cache");
    RegisterAlias("cache.iscached", "cache.iscached", ConventionType::SNC, "Check if an instance is cached");
    RegisterAlias("cloneref", "cloneref", ConventionType::SNC, "Clone an instance reference");
    RegisterAlias("compareinstances", "compareinstances", ConventionType::SNC, "Compare two instances for equality");
    
    // Closure functions
    RegisterAlias("checkcaller", "checkcaller", ConventionType::SNC, "Check if the caller is from the executor");
    RegisterAlias("clonefunction", "clonefunction", ConventionType::SNC, "Clone a function");
    RegisterAlias("getcallingscript", "getcallingscript", ConventionType::SNC, "Get the script that called the current function");
    RegisterAlias("getscriptclosure", "getscriptclosure", ConventionType::SNC, "Get the closure of a script");
    RegisterAliases("getscriptclosure", {"getscriptfunction"}, ConventionType::SNC, "Get the closure of a script");
    RegisterAlias("hookfunction", "hookfunction", ConventionType::SNC, "Hook a function");
    RegisterAliases("hookfunction", {"replaceclosure"}, ConventionType::SNC, "Hook a function");
    RegisterAlias("closuretype", "closuretype", ConventionType::SNC, "Get the type of a closure");
    RegisterAlias("iscclosure", "iscclosure", ConventionType::SNC, "Check if a function is a C closure");
    RegisterAlias("islclosure", "islclosure", ConventionType::SNC, "Check if a function is a Lua closure");
    RegisterAlias("isexecutorclosure", "isexecutorclosure", ConventionType::SNC, "Check if a function is an executor closure");
    RegisterAliases("isexecutorclosure", {"checkclosure", "isourclosure"}, ConventionType::SNC, "Check if a function is an executor closure");
    RegisterAlias("loadstring", "loadstring", ConventionType::SNC, "Load a string as a function");
    
    // Metatable functions
    RegisterAlias("getrawmetatable", "getrawmetatable", ConventionType::SNC, "Get the raw metatable of an object");
    RegisterAlias("hookmetamethod", "hookmetamethod", ConventionType::SNC, "Hook a metamethod");
    RegisterAlias("getnamecallmethod", "getnamecallmethod", ConventionType::SNC, "Get the name of the method being called");
    RegisterAlias("isreadonly", "isreadonly", ConventionType::SNC, "Check if a table is read-only");
    RegisterAlias("setrawmetatable", "setrawmetatable", ConventionType::SNC, "Set the raw metatable of an object");
    RegisterAlias("setreadonly", "setreadonly", ConventionType::SNC, "Set whether a table is read-only");
    
    // Miscellaneous functions
    RegisterAlias("identifyexecutor", "identifyexecutor", ConventionType::SNC, "Identify the executor");
    RegisterAliases("identifyexecutor", {"getexecutorname"}, ConventionType::SNC, "Identify the executor");
    RegisterAlias("lz4compress", "lz4compress", ConventionType::SNC, "Compress data using LZ4");
    RegisterAlias("lz4decompress", "lz4decompress", ConventionType::SNC, "Decompress data using LZ4");
    RegisterAlias("messagebox", "messagebox", ConventionType::SNC, "Display a message box");
    RegisterAlias("queue_on_teleport", "queue_on_teleport", ConventionType::SNC, "Queue a script to run after teleporting");
    RegisterAliases("queue_on_teleport", {"queueonteleport"}, ConventionType::SNC, "Queue a script to run after teleporting");
    RegisterAlias("request", "request", ConventionType::SNC, "Send an HTTP request");
    RegisterAliases("request", {"http.request", "http_request"}, ConventionType::SNC, "Send an HTTP request");
    RegisterAlias("setclipboard", "setclipboard", ConventionType::SNC, "Set the clipboard content");
    RegisterAliases("setclipboard", {"toclipboard"}, ConventionType::SNC, "Set the clipboard content");
    RegisterAlias("setfpscap", "setfpscap", ConventionType::SNC, "Set the FPS cap");
    RegisterAlias("join", "join", ConventionType::SNC, "Join a game");
    RegisterAliases("join", {"joingame", "joinplace", "joinserver"}, ConventionType::SNC, "Join a game");
    RegisterAlias("gethwid", "gethwid", ConventionType::SNC, "Get the hardware ID");
    
    // Script functions
    RegisterAlias("getgc", "getgc", ConventionType::SNC, "Get the garbage collector");
    RegisterAlias("getgenv", "getgenv", ConventionType::SNC, "Get the global environment");
    RegisterAlias("getloadedmodules", "getloadedmodules", ConventionType::SNC, "Get loaded modules");
    RegisterAlias("getrenv", "getrenv", ConventionType::SNC, "Get the Roblox environment");
    RegisterAlias("getrunningscripts", "getrunningscripts", ConventionType::SNC, "Get running scripts");
    RegisterAlias("getscriptbytecode", "getscriptbytecode", ConventionType::SNC, "Get the bytecode of a script");
    RegisterAliases("getscriptbytecode", {"dumpstring"}, ConventionType::SNC, "Get the bytecode of a script");
    RegisterAlias("getscripthash", "getscripthash", ConventionType::SNC, "Get the hash of a script");
    RegisterAlias("getscripts", "getscripts", ConventionType::SNC, "Get all scripts");
    RegisterAlias("getsenv", "getsenv", ConventionType::SNC, "Get the environment of a script");
    RegisterAlias("getthreadidentity", "getthreadidentity", ConventionType::SNC, "Get the identity of the current thread");
    RegisterAliases("getthreadidentity", {"getidentity", "getthreadcontext"}, ConventionType::SNC, "Get the identity of the current thread");
    RegisterAlias("setthreadidentity", "setthreadidentity", ConventionType::SNC, "Set the identity of the current thread");
    RegisterAliases("setthreadidentity", {"setidentity", "setthreadcontext"}, ConventionType::SNC, "Set the identity of the current thread");
    
    // Drawing functions
    RegisterAlias("Drawing", "Drawing", ConventionType::SNC, "Drawing library");
    RegisterAlias("Drawing.new", "Drawing.new", ConventionType::SNC, "Create a new drawing object");
    RegisterAlias("Drawing.Fonts", "Drawing.Fonts", ConventionType::SNC, "Drawing fonts");
    RegisterAlias("isrenderobj", "isrenderobj", ConventionType::SNC, "Check if an object is a render object");
    RegisterAlias("cleardrawcache", "cleardrawcache", ConventionType::SNC, "Clear the drawing cache");
    
    // WebSocket functions
    RegisterAlias("WebSocket", "WebSocket", ConventionType::SNC, "WebSocket library");
    RegisterAlias("WebSocket.connect", "WebSocket.connect", ConventionType::SNC, "Connect to a WebSocket server");
    
    // Player functions
    RegisterAlias("getplayer", "getplayer", ConventionType::SNC, "Get a player");
    RegisterAlias("getlocalplayer", "getlocalplayer", ConventionType::SNC, "Get the local player");
    RegisterAlias("getplayers", "getplayers", ConventionType::SNC, "Get all players");
    RegisterAlias("runanimation", "runanimation", ConventionType::SNC, "Run an animation");
    RegisterAliases("runanimation", {"playanimation"}, ConventionType::SNC, "Run an animation");
    
    // SNC-specific aliases
    RegisterAlias("is_salad_closure", "isexecutorclosure", ConventionType::SNC, "Check if a function is a Salad closure");
    RegisterAlias("is_essence_closure", "isexecutorclosure", ConventionType::SNC, "Check if a function is an Essence closure");
    RegisterAlias("is_ronix_closure", "isexecutorclosure", ConventionType::SNC, "Check if a function is a Ronix closure");
    RegisterAlias("is_awp_closure", "isexecutorclosure", ConventionType::SNC, "Check if a function is an AWP closure");
    RegisterAlias("is_wave_closure", "isexecutorclosure", ConventionType::SNC, "Check if a function is a Wave closure");
}

} // namespace NamingConventions
} // namespace RobloxExecutor
