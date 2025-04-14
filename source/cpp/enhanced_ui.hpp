#pragma once

#include <string>

namespace EnhancedUI {

// Return the first part of the enhanced UI Lua code
std::string GetUIBase() {
    return R"(
-- Enhanced Executor UI
-- Modern, feature-rich UI with script management

-- Services
local GuiService = game:GetService("GuiService")
local TweenService = game:GetService("TweenService")
local UserInputService = game:GetService("UserInputService")
local RunService = game:GetService("RunService")
local HttpService = game:GetService("HttpService")
local Players = game:GetService("Players")

-- Constants
local EDITOR_FONT_SIZE = 16
local SAVED_SCRIPTS_KEY = "ExecutorSavedScripts"
local UI_SETTINGS_KEY = "ExecutorUISettings"

-- Create the main ScreenGui
local ExecutorGui = Instance.new("ScreenGui")
ExecutorGui.Name = "ByfronBypassExecutor"
ExecutorGui.DisplayOrder = 100
ExecutorGui.ResetOnSpawn = false
ExecutorGui.ZIndexBehavior = Enum.ZIndexBehavior.Sibling

-- Try to parent to CoreGui for better persistence
local success, err = pcall(function()
    ExecutorGui.Parent = game:GetService("CoreGui")
end)

if not success then
    -- Fallback to PlayerGui if CoreGui fails
    ExecutorGui.Parent = Players.LocalPlayer:WaitForChild("PlayerGui")
end

-- Main frame
local MainFrame = Instance.new("Frame")
MainFrame.Name = "MainFrame"
MainFrame.Size = UDim2.new(0, 700, 0, 400)
MainFrame.Position = UDim2.new(0.5, -350, 0.5, -200)
MainFrame.BackgroundColor3 = Color3.fromRGB(30, 30, 30)
MainFrame.BorderSizePixel = 0
MainFrame.ClipsDescendants = true
MainFrame.Parent = ExecutorGui

-- Add rounded corners
local UICorner = Instance.new("UICorner")
UICorner.CornerRadius = UDim.new(0, 8)
UICorner.Parent = MainFrame

-- Top bar (for dragging and title)
local TopBar = Instance.new("Frame")
TopBar.Name = "TopBar"
TopBar.Size = UDim2.new(1, 0, 0, 36)
TopBar.BackgroundColor3 = Color3.fromRGB(40, 40, 40)
TopBar.BorderSizePixel = 0
TopBar.Parent = MainFrame

local TopBarCorner = Instance.new("UICorner")
TopBarCorner.CornerRadius = UDim.new(0, 8)
TopBarCorner.Parent = TopBar

-- Fix the top bar corners
local TopBarFix = Instance.new("Frame")
TopBarFix.Name = "TopBarFix"
TopBarFix.Size = UDim2.new(1, 0, 0.5, 0)
TopBarFix.Position = UDim2.new(0, 0, 0.5, 0)
TopBarFix.BackgroundColor3 = Color3.fromRGB(40, 40, 40)
TopBarFix.BorderSizePixel = 0
TopBarFix.Parent = TopBar

-- Title
local Title = Instance.new("TextLabel")
Title.Name = "Title"
Title.Size = UDim2.new(0, 200, 1, 0)
Title.Position = UDim2.new(0, 10, 0, 0)
Title.BackgroundTransparency = 1
Title.Font = Enum.Font.GothamBold
Title.Text = "Executor Pro"
Title.TextColor3 = Color3.fromRGB(220, 220, 220)
Title.TextSize = 16
Title.TextXAlignment = Enum.TextXAlignment.Left
Title.Parent = TopBar

-- Close button
local CloseButton = Instance.new("TextButton")
CloseButton.Name = "CloseButton"
CloseButton.Size = UDim2.new(0, 36, 0, 36)
CloseButton.Position = UDim2.new(1, -36, 0, 0)
CloseButton.BackgroundTransparency = 1
CloseButton.Font = Enum.Font.GothamBold
CloseButton.Text = "×"
CloseButton.TextColor3 = Color3.fromRGB(220, 220, 220)
CloseButton.TextSize = 24
CloseButton.Parent = TopBar

-- Minimize button
local MinimizeButton = Instance.new("TextButton")
MinimizeButton.Name = "MinimizeButton"
MinimizeButton.Size = UDim2.new(0, 36, 0, 36)
MinimizeButton.Position = UDim2.new(1, -72, 0, 0)
MinimizeButton.BackgroundTransparency = 1
MinimizeButton.Font = Enum.Font.GothamBold
MinimizeButton.Text = "−"
MinimizeButton.TextColor3 = Color3.fromRGB(220, 220, 220)
MinimizeButton.TextSize = 24
MinimizeButton.Parent = TopBar

-- Tab buttons container
local TabButtons = Instance.new("Frame")
TabButtons.Name = "TabButtons"
TabButtons.Size = UDim2.new(1, -160, 1, 0)
TabButtons.Position = UDim2.new(0, 120, 0, 0)
TabButtons.BackgroundTransparency = 1
TabButtons.Parent = TopBar
)";
}

// Return the tab components of the UI
std::string GetUITabs() {
    return R"(
-- Create tab buttons
local function CreateTabButton(name, position)
    local TabButton = Instance.new("TextButton")
    TabButton.Name = name .. "Tab"
    TabButton.Size = UDim2.new(0, 90, 1, 0)
    TabButton.Position = UDim2.new(0, position, 0, 0)
    TabButton.BackgroundTransparency = 1
    TabButton.Font = Enum.Font.Gotham
    TabButton.Text = name
    TabButton.TextColor3 = Color3.fromRGB(180, 180, 180)
    TabButton.TextSize = 14
    TabButton.Parent = TabButtons
    return TabButton
end

local EditorTab = CreateTabButton("Editor", 0)
local ScriptsTab = CreateTabButton("Scripts", 90)
local ConsoleTab = CreateTabButton("Console", 180)
local SettingsTab = CreateTabButton("Settings", 270)

-- Content container for tabs
local TabContent = Instance.new("Frame")
TabContent.Name = "TabContent"
TabContent.Size = UDim2.new(1, 0, 1, -36)
TabContent.Position = UDim2.new(0, 0, 0, 36)
TabContent.BackgroundTransparency = 1
TabContent.Parent = MainFrame

-- Create the Editor tab content
local EditorFrame = Instance.new("Frame")
EditorFrame.Name = "EditorFrame"
EditorFrame.Size = UDim2.new(1, 0, 1, 0)
EditorFrame.BackgroundTransparency = 1
EditorFrame.Parent = TabContent

-- Script editor
local Editor = Instance.new("ScrollingFrame")
Editor.Name = "Editor"
Editor.Size = UDim2.new(1, -20, 1, -50)
Editor.Position = UDim2.new(0, 10, 0, 10)
Editor.BackgroundColor3 = Color3.fromRGB(20, 20, 20)
Editor.BorderSizePixel = 0
Editor.ScrollBarThickness = 6
Editor.ScrollingDirection = Enum.ScrollingDirection.Y
Editor.CanvasSize = UDim2.new(0, 0, 0, 0)
Editor.Parent = EditorFrame

local EditorCorner = Instance.new("UICorner")
EditorCorner.CornerRadius = UDim.new(0, 6)
EditorCorner.Parent = Editor

-- The actual text box for code editing
local EditorTextBox = Instance.new("TextBox")
EditorTextBox.Name = "EditorTextBox"
EditorTextBox.Size = UDim2.new(1, -10, 1, 0)
EditorTextBox.Position = UDim2.new(0, 5, 0, 0)
EditorTextBox.BackgroundTransparency = 1
EditorTextBox.Font = Enum.Font.Code
EditorTextBox.TextSize = EDITOR_FONT_SIZE
EditorTextBox.TextColor3 = Color3.fromRGB(220, 220, 220)
EditorTextBox.TextXAlignment = Enum.TextXAlignment.Left
EditorTextBox.TextYAlignment = Enum.TextYAlignment.Top
EditorTextBox.ClearTextOnFocus = false
EditorTextBox.MultiLine = true
EditorTextBox.Text = "-- Welcome to Executor Pro\n-- Enter your script here"
EditorTextBox.Parent = Editor

-- Button container
local ButtonContainer = Instance.new("Frame")
ButtonContainer.Name = "ButtonContainer"
ButtonContainer.Size = UDim2.new(1, -20, 0, 40)
ButtonContainer.Position = UDim2.new(0, 10, 1, -40)
ButtonContainer.BackgroundTransparency = 1
ButtonContainer.Parent = EditorFrame

-- Execute button
local ExecuteButton = Instance.new("TextButton")
ExecuteButton.Name = "ExecuteButton"
ExecuteButton.Size = UDim2.new(0, 100, 0, 36)
ExecuteButton.Position = UDim2.new(0, 0, 0, 0)
ExecuteButton.BackgroundColor3 = Color3.fromRGB(45, 180, 45)
ExecuteButton.Font = Enum.Font.Gotham
ExecuteButton.Text = "Execute"
ExecuteButton.TextColor3 = Color3.fromRGB(255, 255, 255)
ExecuteButton.TextSize = 14
ExecuteButton.Parent = ButtonContainer

local ExecuteCorner = Instance.new("UICorner")
ExecuteCorner.CornerRadius = UDim.new(0, 6)
ExecuteCorner.Parent = ExecuteButton

-- Clear button
local ClearButton = Instance.new("TextButton")
ClearButton.Name = "ClearButton"
ClearButton.Size = UDim2.new(0, 100, 0, 36)
ClearButton.Position = UDim2.new(0, 110, 0, 0)
ClearButton.BackgroundColor3 = Color3.fromRGB(180, 45, 45)
ClearButton.Font = Enum.Font.Gotham
ClearButton.Text = "Clear"
ClearButton.TextColor3 = Color3.fromRGB(255, 255, 255)
ClearButton.TextSize = 14
ClearButton.Parent = ButtonContainer

local ClearCorner = Instance.new("UICorner")
ClearCorner.CornerRadius = UDim.new(0, 6)
ClearCorner.Parent = ClearButton

-- Save button
local SaveButton = Instance.new("TextButton")
SaveButton.Name = "SaveButton"
SaveButton.Size = UDim2.new(0, 100, 0, 36)
SaveButton.Position = UDim2.new(0, 220, 0, 0)
SaveButton.BackgroundColor3 = Color3.fromRGB(45, 45, 180)
SaveButton.Font = Enum.Font.Gotham
SaveButton.Text = "Save Script"
SaveButton.TextColor3 = Color3.fromRGB(255, 255, 255)
SaveButton.TextSize = 14
SaveButton.Parent = ButtonContainer

local SaveCorner = Instance.new("UICorner")
SaveCorner.CornerRadius = UDim.new(0, 6)
SaveCorner.Parent = SaveButton
)";
}

// Return the script management components
std::string GetUIScriptManagement() {
    return R"(
-- Create the Scripts tab content
local ScriptsFrame = Instance.new("Frame")
ScriptsFrame.Name = "ScriptsFrame"
ScriptsFrame.Size = UDim2.new(1, 0, 1, 0)
ScriptsFrame.BackgroundTransparency = 1
ScriptsFrame.Visible = false
ScriptsFrame.Parent = TabContent

-- Scripts tab header
local ScriptsHeader = Instance.new("Frame")
ScriptsHeader.Name = "ScriptsHeader"
ScriptsHeader.Size = UDim2.new(1, -20, 0, 40)
ScriptsHeader.Position = UDim2.new(0, 10, 0, 10)
ScriptsHeader.BackgroundTransparency = 1
ScriptsHeader.Parent = ScriptsFrame

-- Title for the scripts tab
local ScriptsTitle = Instance.new("TextLabel")
ScriptsTitle.Name = "ScriptsTitle"
ScriptsTitle.Size = UDim2.new(0, 200, 1, 0)
ScriptsTitle.BackgroundTransparency = 1
ScriptsTitle.Font = Enum.Font.GothamBold
ScriptsTitle.Text = "Saved Scripts"
ScriptsTitle.TextColor3 = Color3.fromRGB(220, 220, 220)
ScriptsTitle.TextSize = 16
ScriptsTitle.TextXAlignment = Enum.TextXAlignment.Left
ScriptsTitle.Parent = ScriptsHeader

-- Search bar for scripts
local SearchBar = Instance.new("TextBox")
SearchBar.Name = "SearchBar"
SearchBar.Size = UDim2.new(0, 200, 0, 30)
SearchBar.Position = UDim2.new(1, -200, 0.5, -15)
SearchBar.BackgroundColor3 = Color3.fromRGB(40, 40, 40)
SearchBar.Font = Enum.Font.Gotham
SearchBar.PlaceholderText = "Search scripts..."
SearchBar.Text = ""
SearchBar.TextColor3 = Color3.fromRGB(220, 220, 220)
SearchBar.TextSize = 14
SearchBar.Parent = ScriptsHeader

local SearchBarCorner = Instance.new("UICorner")
SearchBarCorner.CornerRadius = UDim.new(0, 6)
SearchBarCorner.Parent = SearchBar

-- Script list container
local ScriptList = Instance.new("ScrollingFrame")
ScriptList.Name = "ScriptList"
ScriptList.Size = UDim2.new(1, -20, 1, -60)
ScriptList.Position = UDim2.new(0, 10, 0, 60)
ScriptList.BackgroundColor3 = Color3.fromRGB(20, 20, 20)
ScriptList.BorderSizePixel = 0
ScriptList.ScrollBarThickness = 6
ScriptList.ScrollingDirection = Enum.ScrollingDirection.Y
ScriptList.CanvasSize = UDim2.new(0, 0, 0, 0)
ScriptList.Parent = ScriptsFrame

local ScriptListCorner = Instance.new("UICorner")
ScriptListCorner.CornerRadius = UDim.new(0, 6)
ScriptListCorner.Parent = ScriptList

-- Script list layout
local ScriptListLayout = Instance.new("UIListLayout")
ScriptListLayout.Padding = UDim.new(0, 5)
ScriptListLayout.SortOrder = Enum.SortOrder.Name
ScriptListLayout.Parent = ScriptList

-- Function to create a script item in the list
local function CreateScriptItem(name, scriptContent, timestamp)
    local item = Instance.new("Frame")
    item.Name = name
    item.Size = UDim2.new(1, -10, 0, 60)
    item.BackgroundColor3 = Color3.fromRGB(30, 30, 30)
    item.BorderSizePixel = 0
    item.Parent = ScriptList
    
    local itemCorner = Instance.new("UICorner")
    itemCorner.CornerRadius = UDim.new(0, 6)
    itemCorner.Parent = item
    
    local itemTitle = Instance.new("TextLabel")
    itemTitle.Name = "Title"
    itemTitle.Size = UDim2.new(1, -140, 0, 30)
    itemTitle.Position = UDim2.new(0, 10, 0, 5)
    itemTitle.BackgroundTransparency = 1
    itemTitle.Font = Enum.Font.GothamBold
    itemTitle.Text = name
    itemTitle.TextColor3 = Color3.fromRGB(220, 220, 220)
    itemTitle.TextSize = 14
    itemTitle.TextXAlignment = Enum.TextXAlignment.Left
    itemTitle.Parent = item
    
    local dateText = Instance.new("TextLabel")
    dateText.Name = "Date"
    dateText.Size = UDim2.new(1, -20, 0, 20)
    dateText.Position = UDim2.new(0, 10, 0, 35)
    dateText.BackgroundTransparency = 1
    dateText.Font = Enum.Font.Gotham
    dateText.Text = "Saved: " .. os.date("%Y-%m-%d %H:%M", timestamp or os.time())
    dateText.TextColor3 = Color3.fromRGB(150, 150, 150)
    dateText.TextSize = 12
    dateText.TextXAlignment = Enum.TextXAlignment.Left
    dateText.Parent = item
    
    -- Load button
    local loadBtn = Instance.new("TextButton")
    loadBtn.Name = "LoadButton"
    loadBtn.Size = UDim2.new(0, 60, 0, 30)
    loadBtn.Position = UDim2.new(1, -130, 0.5, -15)
    loadBtn.BackgroundColor3 = Color3.fromRGB(45, 45, 180)
    loadBtn.Font = Enum.Font.Gotham
    loadBtn.Text = "Load"
    loadBtn.TextColor3 = Color3.fromRGB(255, 255, 255)
    loadBtn.TextSize = 14
    loadBtn.Parent = item
    
    local loadBtnCorner = Instance.new("UICorner")
    loadBtnCorner.CornerRadius = UDim.new(0, 6)
    loadBtnCorner.Parent = loadBtn
    
    -- Execute button
    local execBtn = Instance.new("TextButton")
    execBtn.Name = "ExecuteButton"
    execBtn.Size = UDim2.new(0, 60, 0, 30)
    execBtn.Position = UDim2.new(1, -70, 0.5, -15)
    execBtn.BackgroundColor3 = Color3.fromRGB(45, 180, 45)
    execBtn.Font = Enum.Font.Gotham
    execBtn.Text = "Run"
    execBtn.TextColor3 = Color3.fromRGB(255, 255, 255)
    execBtn.TextSize = 14
    execBtn.Parent = item
    
    local execBtnCorner = Instance.new("UICorner")
    execBtnCorner.CornerRadius = UDim.new(0, 6)
    execBtnCorner.Parent = execBtn
    
    -- Hook up the buttons
    loadBtn.MouseButton1Click:Connect(function()
        EditorTextBox.Text = scriptContent
        switchTab("Editor")
    end)
    
    execBtn.MouseButton1Click:Connect(function()
        if scriptContent and scriptContent ~= "" then
            -- Execute the script using loadstring
            local func, err = loadstring(scriptContent)
            if func then
                -- Log to console
                print("Executing script: " .. name)
                -- Execute in protected mode to catch errors
                local success, result = pcall(func)
                if not success then
                    warn("Execution error: " .. tostring(result))
                else
                    print("Script executed successfully")
                end
            else
                warn("Compilation error: " .. tostring(err))
            end
        end
    end)
    
    return item
end
)";
}

// Return the console and settings components
std::string GetUIConsoleAndSettings() {
    return R"(
-- Create the Console tab content
local ConsoleFrame = Instance.new("Frame")
ConsoleFrame.Name = "ConsoleFrame"
ConsoleFrame.Size = UDim2.new(1, 0, 1, 0)
ConsoleFrame.BackgroundTransparency = 1
ConsoleFrame.Visible = false
ConsoleFrame.Parent = TabContent

-- Console header
local ConsoleHeader = Instance.new("Frame")
ConsoleHeader.Name = "ConsoleHeader"
ConsoleHeader.Size = UDim2.new(1, -20, 0, 40)
ConsoleHeader.Position = UDim2.new(0, 10, 0, 10)
ConsoleHeader.BackgroundTransparency = 1
ConsoleHeader.Parent = ConsoleFrame

-- Console title
local ConsoleTitle = Instance.new("TextLabel")
ConsoleTitle.Name = "ConsoleTitle"
ConsoleTitle.Size = UDim2.new(0, 200, 1, 0)
ConsoleTitle.BackgroundTransparency = 1
ConsoleTitle.Font = Enum.Font.GothamBold
ConsoleTitle.Text = "Execution Console"
ConsoleTitle.TextColor3 = Color3.fromRGB(220, 220, 220)
ConsoleTitle.TextSize = 16
ConsoleTitle.TextXAlignment = Enum.TextXAlignment.Left
ConsoleTitle.Parent = ConsoleHeader

-- Clear console button
local ClearConsoleButton = Instance.new("TextButton")
ClearConsoleButton.Name = "ClearConsoleButton"
ClearConsoleButton.Size = UDim2.new(0, 100, 0, 30)
ClearConsoleButton.Position = UDim2.new(1, -100, 0.5, -15)
ClearConsoleButton.BackgroundColor3 = Color3.fromRGB(180, 45, 45)
ClearConsoleButton.Font = Enum.Font.Gotham
ClearConsoleButton.Text = "Clear Log"
ClearConsoleButton.TextColor3 = Color3.fromRGB(255, 255, 255)
ClearConsoleButton.TextSize = 14
ClearConsoleButton.Parent = ConsoleHeader

local ClearConsoleCorner = Instance.new("UICorner")
ClearConsoleCorner.CornerRadius = UDim.new(0, 6)
ClearConsoleCorner.Parent = ClearConsoleButton

-- Console output
local ConsoleOutput = Instance.new("ScrollingFrame")
ConsoleOutput.Name = "ConsoleOutput"
ConsoleOutput.Size = UDim2.new(1, -20, 1, -60)
ConsoleOutput.Position = UDim2.new(0, 10, 0, 60)
ConsoleOutput.BackgroundColor3 = Color3.fromRGB(20, 20, 20)
ConsoleOutput.BorderSizePixel = 0
ConsoleOutput.ScrollBarThickness = 6
ConsoleOutput.ScrollingDirection = Enum.ScrollingDirection.Y
ConsoleOutput.CanvasSize = UDim2.new(0, 0, 0, 0)
ConsoleOutput.ScrollingEnabled = true
ConsoleOutput.Parent = ConsoleFrame

local ConsoleCorner = Instance.new("UICorner")
ConsoleCorner.CornerRadius = UDim.new(0, 6)
ConsoleCorner.Parent = ConsoleOutput

-- Console text
local ConsoleText = Instance.new("TextLabel")
ConsoleText.Name = "ConsoleText"
ConsoleText.Size = UDim2.new(1, -10, 1, 0)
ConsoleText.Position = UDim2.new(0, 5, 0, 0)
ConsoleText.BackgroundTransparency = 1
ConsoleText.Font = Enum.Font.Code
ConsoleText.Text = "-- Execution log will appear here\n"
ConsoleText.TextColor3 = Color3.fromRGB(220, 220, 220)
ConsoleText.TextSize = 14
ConsoleText.TextXAlignment = Enum.TextXAlignment.Left
ConsoleText.TextYAlignment = Enum.TextYAlignment.Top
ConsoleText.TextWrapped = true
ConsoleText.Parent = ConsoleOutput

-- Create the Settings tab content
local SettingsFrame = Instance.new("Frame")
SettingsFrame.Name = "SettingsFrame"
SettingsFrame.Size = UDim2.new(1, 0, 1, 0)
SettingsFrame.BackgroundTransparency = 1
SettingsFrame.Visible = false
SettingsFrame.Parent = TabContent

-- Settings header
local SettingsHeader = Instance.new("Frame")
SettingsHeader.Name = "SettingsHeader"
SettingsHeader.Size = UDim2.new(1, -20, 0, 40)
SettingsHeader.Position = UDim2.new(0, 10, 0, 10)
SettingsHeader.BackgroundTransparency = 1
SettingsHeader.Parent = SettingsFrame

-- Settings title
local SettingsTitle = Instance.new("TextLabel")
SettingsTitle.Name = "SettingsTitle"
SettingsTitle.Size = UDim2.new(0, 200, 1, 0)
SettingsTitle.BackgroundTransparency = 1
SettingsTitle.Font = Enum.Font.GothamBold
SettingsTitle.Text = "Executor Settings"
SettingsTitle.TextColor3 = Color3.fromRGB(220, 220, 220)
SettingsTitle.TextSize = 16
SettingsTitle.TextXAlignment = Enum.TextXAlignment.Left
SettingsTitle.Parent = SettingsHeader

-- Settings container
local SettingsContainer = Instance.new("ScrollingFrame")
SettingsContainer.Name = "SettingsContainer"
SettingsContainer.Size = UDim2.new(1, -20, 1, -60)
SettingsContainer.Position = UDim2.new(0, 10, 0, 60)
SettingsContainer.BackgroundColor3 = Color3.fromRGB(20, 20, 20)
SettingsContainer.BorderSizePixel = 0
SettingsContainer.ScrollBarThickness = 6
SettingsContainer.ScrollingDirection = Enum.ScrollingDirection.Y
SettingsContainer.CanvasSize = UDim2.new(0, 0, 0, 300) -- Adjusted for content
SettingsContainer.Parent = SettingsFrame

local SettingsCorner = Instance.new("UICorner")
SettingsCorner.CornerRadius = UDim.new(0, 6)
SettingsCorner.Parent = SettingsContainer

-- Settings layout
local SettingsLayout = Instance.new("UIListLayout")
SettingsLayout.Padding = UDim.new(0, 10)
SettingsLayout.SortOrder = Enum.SortOrder.LayoutOrder
SettingsLayout.Parent = SettingsContainer

-- Function to create a setting toggle
local function CreateToggleSetting(name, description, defaultValue, callback)
    local settingFrame = Instance.new("Frame")
    settingFrame.Name = name .. "Setting"
    settingFrame.Size = UDim2.new(1, -20, 0, 60)
    settingFrame.BackgroundColor3 = Color3.fromRGB(30, 30, 30)
    settingFrame.Position = UDim2.new(0, 10, 0, 0)
    settingFrame.LayoutOrder = #SettingsContainer:GetChildren()
    settingFrame.Parent = SettingsContainer
    
    local settingCorner = Instance.new("UICorner")
    settingCorner.CornerRadius = UDim.new(0, 6)
    settingCorner.Parent = settingFrame
    
    local settingTitle = Instance.new("TextLabel")
    settingTitle.Name = "Title"
    settingTitle.Size = UDim2.new(1, -80, 0, 30)
    settingTitle.Position = UDim2.new(0, 10, 0, 5)
    settingTitle.BackgroundTransparency = 1
    settingTitle.Font = Enum.Font.GothamBold
    settingTitle.Text = name
    settingTitle.TextColor3 = Color3.fromRGB(220, 220, 220)
    settingTitle.TextSize = 14
    settingTitle.TextXAlignment = Enum.TextXAlignment.Left
    settingTitle.Parent = settingFrame
    
    local settingDesc = Instance.new("TextLabel")
    settingDesc.Name = "Description"
    settingDesc.Size = UDim2.new(1, -80, 0, 20)
    settingDesc.Position = UDim2.new(0, 10, 0, 35)
    settingDesc.BackgroundTransparency = 1
    settingDesc.Font = Enum.Font.Gotham
    settingDesc.Text = description
    settingDesc.TextColor3 = Color3.fromRGB(150, 150, 150)
    settingDesc.TextSize = 12
    settingDesc.TextXAlignment = Enum.TextXAlignment.Left
    settingDesc.Parent = settingFrame
    
    -- Toggle button
    local toggleButton = Instance.new("Frame")
    toggleButton.Name = "ToggleButton"
    toggleButton.Size = UDim2.new(0, 50, 0, 24)
    toggleButton.Position = UDim2.new(1, -60, 0.5, -12)
    toggleButton.BackgroundColor3 = defaultValue and Color3.fromRGB(45, 180, 45) or Color3.fromRGB(180, 45, 45)
    toggleButton.Parent = settingFrame
    
    local toggleCorner = Instance.new("UICorner")
    toggleCorner.CornerRadius = UDim.new(1, 0)
    toggleCorner.Parent = toggleButton
    
    local toggleIndicator = Instance.new("Frame")
    toggleIndicator.Name = "Indicator"
    toggleIndicator.Size = UDim2.new(0, 18, 0, 18)
    toggleIndicator.Position = defaultValue and UDim2.new(1, -21, 0.5, -9) or UDim2.new(0, 3, 0.5, -9)
    toggleIndicator.BackgroundColor3 = Color3.fromRGB(255, 255, 255)
    toggleIndicator.Parent = toggleButton
    
    local indicatorCorner = Instance.new("UICorner")
    indicatorCorner.CornerRadius = UDim.new(1, 0)
    indicatorCorner.Parent = toggleIndicator
    
    -- Track the toggle state
    local isToggled = defaultValue
    
    -- Make the toggle interactive
    toggleButton.InputBegan:Connect(function(input)
        if input.UserInputType == Enum.UserInputType.MouseButton1 or input.UserInputType == Enum.UserInputType.Touch then
            isToggled = not isToggled
            
            -- Animate the toggle
            local targetPosition = isToggled and UDim2.new(1, -21, 0.5, -9) or UDim2.new(0, 3, 0.5, -9)
            local targetColor = isToggled and Color3.fromRGB(45, 180, 45) or Color3.fromRGB(180, 45, 45)
            
            TweenService:Create(toggleIndicator, TweenInfo.new(0.2), {Position = targetPosition}):Play()
            TweenService:Create(toggleButton, TweenInfo.new(0.2), {BackgroundColor3 = targetColor}):Play()
            
            -- Call the callback with the new state
            if callback then
                callback(isToggled)
            end
        end
    end)
    
    return settingFrame, isToggled
end

-- Create some example settings
CreateToggleSetting("Anti-Detection", "Enable advanced anti-detection features", true, function(enabled)
    print("Anti-Detection set to: " .. tostring(enabled))
    -- In a real implementation, you'd update the ExecutorConfig here
end)

CreateToggleSetting("Script Obfuscation", "Obfuscate scripts for better protection", true, function(enabled)
    print("Script Obfuscation set to: " .. tostring(enabled))
end)

CreateToggleSetting("Auto-Execute", "Automatically execute saved scripts on join", false, function(enabled)
    print("Auto-Execute set to: " .. tostring(enabled))
end)

CreateToggleSetting("TopMost Window", "Keep executor window on top", true, function(enabled)
    print("TopMost Window set to: " .. tostring(enabled))
end)
)";
}

// Return the event handlers and functionality
std::string GetUIFunctionality() {
    return R"(
-- Event handlers and functionality
ExecuteButton.MouseButton1Click:Connect(function()
    local scriptText = EditorTextBox.Text
    if scriptText and scriptText ~= "" then
        -- Execute the script using loadstring
        local func, err = loadstring(scriptText)
        if func then
            -- Log to console
            local logText = "Executing script...\n"
            ConsoleText.Text = ConsoleText.Text .. logText
            
            -- Auto-scroll console to bottom
            ConsoleOutput.CanvasPosition = Vector2.new(0, 99999)
            
            -- Execute in protected mode to catch errors
            local success, result = pcall(func)
            if not success then
                local errorText = "Execution error: " .. tostring(result) .. "\n"
                ConsoleText.Text = ConsoleText.Text .. errorText
                warn(errorText)
            else
                local successText = "Script executed successfully\n"
                ConsoleText.Text = ConsoleText.Text .. successText
                print(successText)
            end
            
            -- Auto-scroll console to bottom again
            ConsoleOutput.CanvasPosition = Vector2.new(0, 99999)
        else
            local errorText = "Compilation error: " .. tostring(err) .. "\n"
            ConsoleText.Text = ConsoleText.Text .. errorText
            warn(errorText)
            
            -- Auto-scroll console to bottom
            ConsoleOutput.CanvasPosition = Vector2.new(0, 99999)
        end
    end
end)

ClearButton.MouseButton1Click:Connect(function()
    EditorTextBox.Text = ""
end)

ClearConsoleButton.MouseButton1Click:Connect(function()
    ConsoleText.Text = "-- Execution log cleared\n"
end)

-- Save script functionality
SaveButton.MouseButton1Click:Connect(function()
    local scriptText = EditorTextBox.Text
    if scriptText and scriptText ~= "" then
        -- Prompt for script name
        local scriptName = "Script_" .. os.time()
        
        -- In a real implementation, you would show a dialog here
        -- For this example, we'll just use a timestamp
        
        -- Save the script
        local savedScripts = {}
        
        -- Try to load existing scripts
        local success, result = pcall(function()
            return HttpService:JSONDecode(Players.LocalPlayer:GetAttribute(SAVED_SCRIPTS_KEY) or "{}")
        end)
        
        if success and type(result) == "table" then
            savedScripts = result
        end
        
        -- Add the new script
        savedScripts[scriptName] = {
            name = scriptName,
            content = scriptText,
            timestamp = os.time()
        }
        
        -- Save back to attribute (persisted)
        Players.LocalPlayer:SetAttribute(SAVED_SCRIPTS_KEY, HttpService:JSONEncode(savedScripts))
        
        -- Add to UI
        CreateScriptItem(scriptName, scriptText, os.time())
        
        -- Refresh script list UI (would be implemented in a real version)
        print("Script saved as: " .. scriptName)
        
        -- Log to console
        ConsoleText.Text = ConsoleText.Text .. "Script saved as: " .. scriptName .. "\n"
        
        -- Show the script in the list
        RefreshScriptList()
    end
end)

-- Function to refresh the script list UI
function RefreshScriptList()
    -- Clear existing items
    for _, child in pairs(ScriptList:GetChildren()) do
        if child:IsA("Frame") then
            child:Destroy()
        end
    end
    
    -- Load saved scripts
    local savedScripts = {}
    local success, result = pcall(function()
        return HttpService:JSONDecode(Players.LocalPlayer:GetAttribute(SAVED_SCRIPTS_KEY) or "{}")
    end)
    
    if success and type(result) == "table" then
        savedScripts = result
    end
    
    -- Add each script to the UI
    for name, data in pairs(savedScripts) do
        CreateScriptItem(name, data.content, data.timestamp)
    end
    
    -- Update canvas size
    ScriptList.CanvasSize = UDim2.new(0, 0, 0, ScriptListLayout.AbsoluteContentSize.Y + 10)
end

-- Initialize script list
RefreshScriptList()

-- Script search functionality
SearchBar.Changed:Connect(function(property)
    if property == "Text" then
        local searchText = string.lower(SearchBar.Text)
        
        for _, child in pairs(ScriptList:GetChildren()) do
            if child:IsA("Frame") then
                if searchText == "" then
                    child.Visible = true
                else
                    local scriptName = string.lower(child.Name)
                    child.Visible = string.find(scriptName, searchText) ~= nil
                end
            end
        end
    end
end)

-- Dragging functionality for the main frame
local dragging = false
local dragInput = nil
local dragStart = nil
local startPos = nil

local function updateDrag(input)
    local delta = input.Position - dragStart
    MainFrame.Position = UDim2.new(startPos.X.Scale, startPos.X.Offset + delta.X, startPos.Y.Scale, startPos.Y.Offset + delta.Y)
end

TopBar.InputBegan:Connect(function(input)
    if input.UserInputType == Enum.UserInputType.MouseButton1 or input.UserInputType == Enum.UserInputType.Touch then
        dragging = true
        dragStart = input.Position
        startPos = MainFrame.Position
        
        input.Changed:Connect(function()
            if input.UserInputState == Enum.UserInputState.End then
                dragging = false
            end
        end)
    end
end)

TopBar.InputChanged:Connect(function(input)
    if input.UserInputType == Enum.UserInputType.MouseMovement or input.UserInputType == Enum.UserInputType.Touch then
        dragInput = input
    end
end)

UserInputService.InputChanged:Connect(function(input)
    if input == dragInput and dragging then
        updateDrag(input)
    end
end)

-- Close and minimize functionality
CloseButton.MouseButton1Click:Connect(function()
    ExecutorGui.Enabled = false
end)

MinimizeButton.MouseButton1Click:Connect(function()
    MainFrame.Visible = not MainFrame.Visible
end)

-- Tab switching functionality
local tabs = {
    Editor = EditorFrame,
    Scripts = ScriptsFrame,
    Console = ConsoleFrame,
    Settings = SettingsFrame
}

function switchTab(tabName)
    for name, frame in pairs(tabs) do
        frame.Visible = (name == tabName)
    end
    
    -- Update tab button appearance
    for _, button in pairs(TabButtons:GetChildren()) do
        if button:IsA("TextButton") then
            if button.Name == tabName .. "Tab" then
                button.TextColor3 = Color3.fromRGB(255, 255, 255)
                button.Font = Enum.Font.GothamBold
            else
                button.TextColor3 = Color3.fromRGB(180, 180, 180)
                button.Font = Enum.Font.Gotham
            end
        end
    end
end

EditorTab.MouseButton1Click:Connect(function()
    switchTab("Editor")
end)

ScriptsTab.MouseButton1Click:Connect(function()
    switchTab("Scripts")
    RefreshScriptList() -- Refresh when switching to Scripts tab
end)

ConsoleTab.MouseButton1Click:Connect(function()
    switchTab("Console")
end)

SettingsTab.MouseButton1Click:Connect(function()
    switchTab("Settings")
end)

-- Start with Editor tab selected
switchTab("Editor")

-- Keybinds
UserInputService.InputBegan:Connect(function(input, gameProcessed)
    if gameProcessed then return end
    
    -- Ctrl+E to execute
    if input.KeyCode == Enum.KeyCode.E and UserInputService:IsKeyDown(Enum.KeyCode.LeftControl) then
        ExecuteButton.MouseButton1Click:Fire()
    end
    
    -- Ctrl+S to save
    if input.KeyCode == Enum.KeyCode.S and UserInputService:IsKeyDown(Enum.KeyCode.LeftControl) then
        SaveButton.MouseButton1Click:Fire()
    end
    
    -- Ctrl+Space to show/hide the executor
    if input.KeyCode == Enum.KeyCode.Space and UserInputService:IsKeyDown(Enum.KeyCode.LeftControl) then
        ExecutorGui.Enabled = not ExecutorGui.Enabled
    end
end)

-- Intercept print and warn functions to capture output
local originalPrint = print
local originalWarn = warn

print = function(...)
    local args = {...}
    local message = ""
    for i, v in ipairs(args) do
        message = message .. tostring(v) .. (i < #args and "\t" or "")
    end
    
    ConsoleText.Text = ConsoleText.Text .. message .. "\n"
    ConsoleOutput.CanvasPosition = Vector2.new(0, 99999)
    
    return originalPrint(...)
end

warn = function(...)
    local args = {...}
    local message = ""
    for i, v in ipairs(args) do
        message = message .. tostring(v) .. (i < #args and "\t" or "")
    end
    
    ConsoleText.Text = ConsoleText.Text .. "WARNING: " .. message .. "\n"
    ConsoleOutput.CanvasPosition = Vector2.new(0, 99999)
    
    return originalWarn(...)
end

-- Return the UI so it can be referenced elsewhere
return ExecutorGui
)";
}

// Get the complete UI code
std::string GetCompleteUI() {
    return GetUIBase() + 
           GetUITabs() + 
           GetUIScriptManagement() + 
           GetUIConsoleAndSettings() + 
           GetUIFunctionality();
}

} // namespace EnhancedUI
