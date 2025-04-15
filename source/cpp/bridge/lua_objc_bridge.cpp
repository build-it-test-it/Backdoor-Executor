// Bridge implementation for safely communicating between Lua and Objective-C
#include "lua_isolation.h"
#include "objc_isolation.h"
#include <string>
#include <vector>

// Implementation of LuaBridge functions
namespace LuaBridge {
    bool ExecuteScript(lua_State* L, const char* script, const char* chunkname) {
        // Directly use real Lua API since we're in a Lua-enabled compilation unit
        int status = luaL_loadbuffer(L, script, strlen(script), chunkname);
        if (status != 0) {
            return false;
        }
        status = lua_pcall(L, 0, 0, 0);
        return status == 0;
    }
    
    const char* GetLastError(lua_State* L) {
        if (lua_gettop(L) > 0 && lua_isstring(L, -1)) {
            return lua_tostring(L, -1);
        }
        return "Unknown error";
    }
    
    void CollectGarbage(lua_State* L) {
        lua_gc(L, LUA_GCCOLLECT, 0);
    }
    
    void RegisterFunction(lua_State* L, const char* name, int (*func)(lua_State*)) {
        lua_pushcfunction(L, func, name);
        lua_setglobal(L, name);
    }
}

// Implementation of ObjCBridge functions
namespace ObjCBridge {
    // These would normally be implemented in Objective-C++ files
    // Here we provide stub implementations for testing
    bool ShowAlert(const char* title, const char* message) {
        // In a real implementation, this would create a UIAlertController
        printf("ALERT: %s - %s\n", title, message);
        return true;
    }
    
    bool SaveScript(const char* name, const char* script) {
        // In a real implementation, this would use NSFileManager
        printf("SAVE SCRIPT: %s\n", name);
        return true;
    }
    
    const char* LoadScript(const char* name) {
        // In a real implementation, this would use NSFileManager
        static std::string script = "-- Loaded script content";
        return script.c_str();
    }
    
    bool InjectFloatingButton() {
        // In a real implementation, this would create and add a UIButton
        printf("INJECT FLOATING BUTTON\n");
        return true;
    }
    
    void ShowScriptEditor() {
        // In a real implementation, this would present a UIViewController
        printf("SHOW SCRIPT EDITOR\n");
    }
}
