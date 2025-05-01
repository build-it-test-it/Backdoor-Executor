--[[
  Roblox Executor Test Script
  This script tests the core functionality of your executor
  and reports any issues found.
]]

local results = {
  passed = 0,
  failed = 0,
  tests = {}
}

-- Utility function to add test result
local function addResult(name, passed, details)
  results.tests[#results.tests + 1] = {
    name = name,
    passed = passed,
    details = details or ""
  }
  
  if passed then
    results.passed = results.passed + 1
  else
    results.failed = results.failed + 1
  end
  
  print(passed and "✅ PASS:" or "❌ FAIL:", name, details or "")
end

-- Utility function to safely check if a function exists
local function functionExists(funcPath)
  local parts = {}
  for part in string.gmatch(funcPath, "[^.]+") do
    table.insert(parts, part)
  end
  
  local current = _G
  for i, part in ipairs(parts) do
    if type(current) ~= "table" then
      return false
    end
    current = current[part]
    if current == nil then
      return false
    end
  end
  
  return type(current) == "function"
end

print("====== Roblox Executor Test Suite ======")
print("Starting tests...")

-- Test 1: Basic environment check
do
  local environment = getfenv and getfenv() or _ENV
  local isLuau = type(script) == "userdata" and pcall(function() return script.Name end)
  local isSandboxed = not io or not io.open
  
  addResult("Environment Detection", true, 
    "Running in " .. (isLuau and "Luau" or "Lua") .. 
    (isSandboxed and " (sandboxed)" or " (full access)"))
end

-- Test 2: Executor identity check
do
  local identitySuccess, identity = pcall(function() 
    -- Check for Synapse X, KRNL, etc.
    if syn and syn.request then return "Synapse X" end
    if KRNL_LOADED then return "KRNL" end
    if is_sirhurt_closure then return "Sirhurt" end
    if isourclosure then return "Script-Ware" end
    if SENTINEL_V2 then return "Sentinel" end
    if executor then return executor.name or "Custom (with executor global)" end
    
    -- Check for custom globals specific to your executor
    if ExecuteScript and GetScriptSuggestions then return "iOS Executor" end
    
    return "Unknown Executor"
  end)
  
  addResult("Executor Identification", identitySuccess, identity)
end

-- Test 3: Lua VM functionality
do
  local vmSuccess = pcall(function()
    -- Test basic Lua functionality
    local a = 5
    local b = 10
    local c = a + b
    assert(c == 15)
    
    -- Test table manipulation
    local t = {}
    t.x = 1
    t[2] = "test"
    assert(t.x == 1 and t[2] == "test")
    
    -- Test function creation
    local f = function(x) return x * 2 end
    assert(f(10) == 20)
    
    -- Test error handling
    local status, err = pcall(function() error("test error") end)
    assert(not status and err:find("test error"))
  end)
  
  addResult("Lua VM Functionality", vmSuccess)
end

-- Test 4: Function accessibility
do
  -- Core Roblox functions to check
  local functions = {
    "game.GetService",
    "game.Players.LocalPlayer",
    "workspace.CurrentCamera",
    "require",
    "getgenv",
    "getrawmetatable",
    "getrenv",
    "getnamecallmethod",
    "setreadonly",
    "isreadonly"
  }
  
  local accessibleCount = 0
  local details = ""
  
  for _, func in ipairs(functions) do
    if functionExists(func) then
      accessibleCount = accessibleCount + 1
    else
      details = details .. func .. ", "
    end
  end
  
  if details ~= "" then
    details = "Missing functions: " .. details:sub(1, -3)
  else
    details = "All core functions accessible"
  end
  
  addResult("Function Accessibility", accessibleCount >= 0.7 * #functions, 
    string.format("%d/%d functions accessible. %s", accessibleCount, #functions, details))
end

-- Test 5: Memory read/write
do
  local memorySuccess = pcall(function()
    -- Try to access memory if your executor has this capability
    if not functionExists("readfile") and not functionExists("writefile") then
      -- No filesystem access, so we'll test memory manipulation
      local memoryTest = "test"
      local success = false
      
      -- Check if we have memory functions
      if rawget(_G, "WriteMemory") and rawget(_G, "WriteMemory") then
        -- We have direct memory access functions
        success = true
      elseif getgenv and setrawmetatable then
        -- We can manipulate metatables and environments
        success = true
      end
      
      assert(success, "No memory manipulation capability found")
    else
      -- Test file system access
      writefile("executor_test.txt", "Test file content")
      local content = readfile("executor_test.txt")
      assert(content == "Test file content")
      delfile("executor_test.txt")
    end
  end)
  
  addResult("Memory/File Manipulation", memorySuccess)
end

-- Test 6: Executor-specific features
do
  local featuresSuccess, featuresDetails = pcall(function()
    local features = {}
    
    -- Check for specific iOS executor functions
    if type(ExecuteScript) == "function" then
      table.insert(features, "Script Execution")
    end
    
    if type(WriteMemory) == "function" then
      table.insert(features, "Memory Writing")
    end
    
    if type(HookRobloxMethod) == "function" then
      table.insert(features, "Method Hooking")
    end
    
    if type(InjectRobloxUI) == "function" then
      table.insert(features, "UI Injection")
    end
    
    if type(GetScriptSuggestions) == "function" then
      table.insert(features, "AI Script Suggestions")
    end
    
    return table.concat(features, ", ")
  end)
  
  addResult("Executor Features", featuresSuccess and featuresDetails ~= "", 
    featuresSuccess and "Detected: " .. featuresDetails or "No executor-specific features detected")
end

-- Test 7: Hook functionality
do
  local hookSuccess = pcall(function()
    -- Try to hook a simple function if supported
    local originalFunc = type
    local hookCalled = false
    
    local function hookType(value)
      hookCalled = true
      return originalFunc(value)
    end
    
    -- Different executor hook methods
    if hookfunction then
      hookfunction(type, hookType)
    elseif replace_closure then
      replace_closure(type, hookType)
    elseif HookRobloxMethod then
      -- Your custom hook function
      HookRobloxMethod(type, hookType)
    else
      error("No hook function available")
    end
    
    -- Test if hook works
    local testType = type(123)
    assert(hookCalled, "Hook was not called")
    assert(testType == "number", "Hook returned incorrect value")
    
    -- Restore original
    if hookfunction then
      hookfunction(type, originalFunc)
    elseif replace_closure then
      replace_closure(type, originalFunc)
    elseif HookRobloxMethod then
      -- Your custom restore method
    end
  end)
  
  addResult("Function Hooking", hookSuccess)
end

-- Generate final report
print("\n====== Test Results ======")
print(string.format("Passed: %d, Failed: %d, Total: %d", 
  results.passed, results.failed, results.passed + results.failed))
print("==========================")

if results.failed > 0 then
  print("\nFailed tests:")
  for _, test in ipairs(results.tests) do
    if not test.passed then
      print("- " .. test.name .. ": " .. test.details)
    end
  end
end

-- Return results table for programmatic use
return results
