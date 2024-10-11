-- source/Files.lua

local lfs = require("lfs")  -- Ensure LuaFileSystem is available

-- Function to create a directory if it doesn't exist
local function createDirectory(path)
    -- Check if the directory already exists
    if lfs.attributes(path) then
        print("Directory already exists: " .. path)
        return true
    else
        -- Try to create the directory
        local success, err = lfs.mkdir(path)
        if success then
            print("Directory created: " .. path)
            return true
        else
            print("Error creating directory: " .. err)
            return false
        end
    end
end

-- Function to initialize the Workspace directory
local function initializeWorkspace(appName)
    -- Get the app’s sandboxed Documents directory
    local path = os.getenv("HOME") .. "/Documents/Workspace"
    return createDirectory(path)
end


-- Return the functions to be used by other modules
return {
    initializeWorkspace = initializeWorkspace
}
