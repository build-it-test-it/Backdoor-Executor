#include <lua.hpp>         // Lua core
#include <lauxlib.h>      // Lua auxiliary functions
#include <lualib.h>       // Lua standard libraries
#include <lfs.h>          // LuaFileSystem for file handling
#include <iostream>
#include <string>
#include <fstream>

// Multi-line Lua script as a string
const char* mainLuauScript = R"(
print("Hello from main.luau!")
function main(playerName)
    print("Executing main function for player: " .. playerName)
    -- Add your main script logic here
end
)";

// Universal file functions
int isfile(lua_State* L) {
    const char* path = luaL_checkstring(L, 1);
    std::ifstream file(path);
    lua_pushboolean(L, file.good());
    return 1; // Number of return values
}

int writefile(lua_State* L) {
    const char* path = luaL_checkstring(L, 1);
    const char* content = luaL_checkstring(L, 2);
    
    // Ensure the path is within the workspace directory
    std::string workspacePath = "workspace/";
    if (std::string(path).find(workspacePath) != 0) {
        luaL_error(L, "Path must be within the workspace directory.");
        return 0;
    }

    std::ofstream file(path);
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
    const char* path = luaL_checkstring(L, 1);
    const char* content = luaL_checkstring(L, 2);
    
    // Ensure the path is within the workspace directory
    std::string workspacePath = "workspace/";
    if (std::string(path).find(workspacePath) != 0) {
        luaL_error(L, "Path must be within the workspace directory.");
        return 0;
    }

    std::ofstream file(path, std::ios_base::app);
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
    
    // Ensure the path is within the workspace directory
    std::string workspacePath = "workspace/";
    if (std::string(path).find(workspacePath) != 0) {
        luaL_error(L, "Path must be within the workspace directory.");
        return 0;
    }

    std::ifstream file(path);
    if (!file) {
        lua_pushnil(L);
        return 1; // Return nil if the file does not exist
    }

    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    lua_pushstring(L, content.c_str());
    return 1; // Return content of the file
}

// Function to execute the main Luau script for a specific player
void executeMainLuau(lua_State* L, const std::string& playerName) {
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

// Main function to initialize Lua and set up event listener
extern "C" int luaopen_mylibrary(lua_State *L) {
    // Load LuaFileSystem
    luaL_requiref(L, "lfs", luaopen_lfs, 1);
    lua_pop(L, 1); // Remove the library from the stack

    // Register universal file functions
    lua_register(L, "isfile", isfile);
    lua_register(L, "writefile", writefile);
    lua_register(L, "append_file", append_file);
    lua_register(L, "readfile", readfile);

    // Hook into the PlayerAdded event
    hookPlayerAddedEvent(L);

    return 0;
}

// Entry point for the dynamic library
extern "C" void luaopen_executor(lua_State* L) {
    luaopen_mylibrary(L);
}
