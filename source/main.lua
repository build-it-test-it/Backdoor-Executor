-- source/main.lua

-- Import the Files module
local Files = require("Files")

-- Get the app name from the global variable set in C++
-- The app name is available because we set it in library.cpp
local appName = "Test"  -- Fallback if not set

-- Initialize the Workspace directory
if not Files.initializeWorkspace(appName) then
    print("Failed to initialize the Workspace directory.")
end

-- Define a function to greet a user
function greet(name)
    return "Hello, " .. name .. "!"
end

-- Call the greet function and print the result
print(greet("World"))  -- Output: Hello, World!
