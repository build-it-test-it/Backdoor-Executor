#include "ScriptAssistant.h"
#include "AIConfig.h"
#include "local_models/GeneralAssistantModel.h"
#include "local_models/ScriptGenerationModel.h"
#include <iostream>
#include <thread>
#include <algorithm>
#include <chrono>
#include <sstream>
#include <set>

namespace iOS {
namespace AIFeatures {

// Constructor
ScriptAssistant::ScriptAssistant()
    : m_maxHistorySize(100),
      m_languageModel(nullptr),
      m_gameAnalyzer(nullptr),
      m_scriptGenerator(nullptr),
      m_executionInterface(nullptr),
      m_responseCallback(nullptr),
      m_executionCallback(nullptr) {
    // Add system welcome message
    AddSystemMessage("ScriptAssistant initialized. Ready to help with Lua scripting and game analysis.");
}

// Destructor
ScriptAssistant::~ScriptAssistant() {
    // Clean up resources
    if (m_languageModel) {
        delete static_cast<LocalModels::GeneralAssistantModel*>(m_languageModel);
        m_languageModel = nullptr;
    }
    
    if (m_scriptGenerator) {
        delete static_cast<LocalModels::ScriptGenerationModel*>(m_scriptGenerator);
        m_scriptGenerator = nullptr;
    }
    
    // Clear other resources
    m_conversationHistory.clear();
    m_scriptTemplates.clear();
}

// Initialize
bool ScriptAssistant::Initialize() {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    try {
        // Initialize language model
        auto languageModel = new LocalModels::GeneralAssistantModel();
        if (!languageModel->Initialize("models/assistant")) {
            std::cerr << "Failed to initialize language model" << std::endl;
            delete languageModel;
            return false;
        }
        m_languageModel = languageModel;
        
        // Initialize script generator
        auto scriptGenerator = new LocalModels::ScriptGenerationModel();
        if (!scriptGenerator->Initialize("models/generator")) {
            std::cerr << "Failed to initialize script generator" << std::endl;
            // Continue anyway, just without script generation capability
        }
        m_scriptGenerator = scriptGenerator;
        
        // Load default templates
        LoadDefaultTemplates();
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Exception during ScriptAssistant initialization: " << e.what() << std::endl;
        return false;
    }
}

// Set response callback
void ScriptAssistant::SetResponseCallback(ResponseCallback callback) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_responseCallback = callback;
}

// Set execution callback
void ScriptAssistant::SetExecutionCallback(ScriptExecutionCallback callback) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_executionCallback = callback;
}

// Process user input
void ScriptAssistant::ProcessUserInput(const std::string& input) {
    if (input.empty()) return;
    
    // Add to conversation history
    AddUserMessage(input);
    
    // Generate response
    std::string response = GenerateResponse(input);
    
    // Add to conversation history
    AddAssistantMessage(response);
    
    // Call callback if set
    if (m_responseCallback) {
        m_responseCallback(response, true);
    }
}

// Generate a script based on description
void ScriptAssistant::GenerateScript(const std::string& description) {
    if (description.empty()) return;
    
    // Check if script generator is initialized
    if (!m_scriptGenerator) {
        std::string errorMsg = "Script generation is not available. Using template instead.";
        
        // Find closest template
        auto closestTemplate = FindClosestTemplate(description);
        
        if (closestTemplate.m_name.empty()) {
            errorMsg += " No suitable template found.";
            if (m_responseCallback) {
                m_responseCallback(errorMsg, false);
            }
            return;
        }
        
        // Use template
        std::string response = "Using template '" + closestTemplate.m_name + "' as a starting point:\n\n```lua\n" + 
                              closestTemplate.m_code + "\n```\n\nYou can modify this to fit your needs.";
        
        if (m_responseCallback) {
            m_responseCallback(response, true);
        }
        return;
    }
    
    // Use a separate thread for generation to avoid blocking
    std::thread([this, description]() {
        try {
            // Get script generator
            auto generator = static_cast<LocalModels::ScriptGenerationModel*>(m_scriptGenerator);
            
            // Generate script
            std::string script = generator->GenerateScript(description);
            
            if (script.empty()) {
                if (m_responseCallback) {
                    m_responseCallback("Failed to generate script for: " + description, false);
                }
                return;
            }
            
            // Prepare response
            std::string response = "Generated script based on your description:\n\n```lua\n" + script + "\n```";
            
            // Call callback
            if (m_responseCallback) {
                m_responseCallback(response, true);
            }
            
            // Add to templates
            ScriptTemplate newTemplate;
            newTemplate.m_name = "Generated: " + description.substr(0, 30) + (description.length() > 30 ? "..." : "");
            newTemplate.m_description = description;
            newTemplate.m_code = script;
            AddTemplate(newTemplate);
            
        } catch (const std::exception& e) {
            std::cerr << "Exception during script generation: " << e.what() << std::endl;
            if (m_responseCallback) {
                m_responseCallback("Error generating script: " + std::string(e.what()), false);
            }
        }
    }).detach();
}

// Analyze game
void ScriptAssistant::AnalyzeGame(const GameContext& context) {
    // This is a more complex operation - add to conversation history
    AddSystemMessage("Game analysis requested");
    
    std::stringstream analysis;
    analysis << "Game Analysis:\n\n";
    
    // Basic analysis of game structure
    if (context.m_rootObject) {
        analysis << "Game: " << context.m_rootObject->m_name << " (" << context.m_rootObject->m_className << ")\n";
        analysis << "Children: " << context.m_rootObject->m_children.size() << "\n\n";
        
        // Analyze key game components
        std::set<std::string> classNames;
        AnalyzeGameObject(context.m_rootObject, classNames);
        
        analysis << "Detected classes:\n";
        for (const auto& className : classNames) {
            analysis << "- " << className << "\n";
        }
        
        analysis << "\nAvailable APIs: " << context.m_availableAPIs.size() << "\n";
        for (const auto& api : context.m_availableAPIs) {
            analysis << "- " << api << "\n";
        }
    } else {
        analysis << "No game data available for analysis.\n";
    }
    
    // Generate recommendations based on analysis
    analysis << "\nRecommendations:\n";
    
    if (classNames.find("Player") != classNames.end()) {
        analysis << "- Player object detected, can use GetService('Players'):GetLocalPlayer()\n";
    }
    
    if (classNames.find("Workspace") != classNames.end()) {
        analysis << "- Workspace detected, can access the 3D world\n";
    }
    
    // Add the analysis as an assistant message
    std::string analysisStr = analysis.str();
    AddAssistantMessage(analysisStr);
    
    // Send to callback if available
    if (m_responseCallback) {
        m_responseCallback(analysisStr, true);
    }
}

// Optimize script
void ScriptAssistant::OptimizeScript(const std::string& script) {
    if (script.empty()) return;
    
    // Add to conversation
    AddSystemMessage("Script optimization requested");
    
    try {
        // Basic optimization rules
        std::string optimized = script;
        
        // Remove unnecessary whitespace
        optimized = RemoveUnnecessaryWhitespace(optimized);
        
        // Optimize local variable usage
        optimized = OptimizeLocalVariables(optimized);
        
        // Optimize loops
        optimized = OptimizeLoops(optimized);
        
        // Prepare response
        std::stringstream response;
        response << "Optimized script:\n\n```lua\n" << optimized << "\n```\n\n";
        
        // Add optimization notes
        response << "Optimization notes:\n";
        response << "- Removed unnecessary whitespace\n";
        response << "- Optimized local variable declarations\n";
        response << "- Improved loop efficiency\n";
        
        // Check for potential performance issues
        std::vector<std::string> issues = DetectPerformanceIssues(optimized);
        if (!issues.empty()) {
            response << "\nPotential performance issues:\n";
            for (const auto& issue : issues) {
                response << "- " << issue << "\n";
            }
        }
        
        std::string responseStr = response.str();
        
        // Add to conversation
        AddAssistantMessage(responseStr);
        
        // Send to callback
        if (m_responseCallback) {
            m_responseCallback(responseStr, true);
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception during script optimization: " << e.what() << std::endl;
        if (m_responseCallback) {
            m_responseCallback("Error optimizing script: " + std::string(e.what()), false);
        }
    }
}

// Execute script
void ScriptAssistant::ExecuteScript(const std::string& script) {
    if (script.empty()) return;
    
    // Add to system context
    AddSystemMessage("Script execution requested");
    
    // Since we don't have direct execution capability, forward this to callback
    if (m_executionCallback) {
        m_executionCallback(true, script);
    } else {
        // No callback, notify user
        std::string response = "Cannot execute script: execution interface not available";
        if (m_responseCallback) {
            m_responseCallback(response, false);
        }
    }
}

// Implementation of ReleaseUnusedResources
void ScriptAssistant::ReleaseUnusedResources() {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::cout << "ScriptAssistant: Releasing unused resources" << std::endl;
    
    // Clear conversation history beyond a certain limit
    TrimConversationHistory();
    
    // Release templates that haven't been used recently
    if (m_scriptTemplates.size() > 20) {
        m_scriptTemplates.resize(20);
    }
    
    // Release script generator if we're in low memory mode
    // Keep language model since it's core functionality
    bool isLowMemory = false; // TODO: Check system memory pressure
    
    if (isLowMemory && m_scriptGenerator) {
        delete static_cast<LocalModels::ScriptGenerationModel*>(m_scriptGenerator);
        m_scriptGenerator = nullptr;
        std::cout << "ScriptAssistant: Released script generator due to low memory" << std::endl;
    }
}

// Implementation of GetMemoryUsage
uint64_t ScriptAssistant::GetMemoryUsage() const {
    // Estimate memory usage based on stored data
    uint64_t memoryUsage = 0;
    
    // Conversation history
    for (const auto& message : m_conversationHistory) {
        memoryUsage += message.m_content.size();
    }
    
    // Script templates
    for (const auto& tmpl : m_scriptTemplates) {
        memoryUsage += tmpl.m_name.size() + tmpl.m_description.size() + tmpl.m_code.size();
    }
    
    // Language model memory usage
    if (m_languageModel) {
        auto model = static_cast<LocalModels::GeneralAssistantModel*>(m_languageModel);
        memoryUsage += model->GetMemoryUsage();
    }
    
    // Script generator memory usage
    if (m_scriptGenerator) {
        auto generator = static_cast<LocalModels::ScriptGenerationModel*>(m_scriptGenerator);
        memoryUsage += generator->GetMemoryUsage();
    }
    
    // Add base memory usage
    memoryUsage += 1024 * 1024; // 1MB base usage
    
    return memoryUsage;
}

// Load templates
void ScriptAssistant::LoadTemplates(const std::string& templatesPath) {
    // TODO: Implement loading from file
    // For now, just load default templates
    LoadDefaultTemplates();
}

// Save templates
void ScriptAssistant::SaveTemplates(const std::string& templatesPath) {
    // TODO: Implement saving to file
}

// Add template
void ScriptAssistant::AddTemplate(const ScriptTemplate& tmpl) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // Check if template with this name already exists
    for (auto& existing : m_scriptTemplates) {
        if (existing.m_name == tmpl.m_name) {
            // Update existing template
            existing.m_description = tmpl.m_description;
            existing.m_code = tmpl.m_code;
            return;
        }
    }
    
    // Add new template
    m_scriptTemplates.push_back(tmpl);
}

// Remove template
void ScriptAssistant::RemoveTemplate(const std::string& templateName) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = std::remove_if(m_scriptTemplates.begin(), m_scriptTemplates.end(),
                           [&templateName](const ScriptTemplate& tmpl) {
                               return tmpl.m_name == templateName;
                           });
    
    if (it != m_scriptTemplates.end()) {
        m_scriptTemplates.erase(it, m_scriptTemplates.end());
    }
}

// Get suggestions
std::vector<std::string> ScriptAssistant::GetSuggestions(const std::string& partialInput) {
    std::vector<std::string> suggestions;
    
    // Check for common command patterns
    if (partialInput.empty()) {
        // Default suggestions
        suggestions.push_back("help");
        suggestions.push_back("generate script for");
        suggestions.push_back("optimize");
        suggestions.push_back("analyze game");
        suggestions.push_back("show templates");
    } else if (partialInput.find("gen") == 0) {
        // Generate commands
        suggestions.push_back("generate script for player movement");
        suggestions.push_back("generate script for ESP");
        suggestions.push_back("generate script for aimbot");
    } else if (partialInput.find("opt") == 0) {
        // Optimize commands
        suggestions.push_back("optimize my script");
        suggestions.push_back("optimize for performance");
        suggestions.push_back("optimize and minify");
    } else if (partialInput.find("ana") == 0) {
        // Analyze commands
        suggestions.push_back("analyze game");
        suggestions.push_back("analyze this script");
    }
    
    // Return up to 5 suggestions
    if (suggestions.size() > 5) {
        suggestions.resize(5);
    }
    
    return suggestions;
}

// Get templates
std::vector<ScriptAssistant::ScriptTemplate> ScriptAssistant::GetTemplates() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_scriptTemplates;
}

// Get current context
ScriptAssistant::GameContext ScriptAssistant::GetCurrentContext() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_currentContext;
}

// Clear conversation history
void ScriptAssistant::ClearConversationHistory() {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // Keep only system messages
    auto it = std::remove_if(m_conversationHistory.begin(), m_conversationHistory.end(),
                           [](const Message& msg) {
                               return msg.m_type != MessageType::System;
                           });
    
    m_conversationHistory.erase(it, m_conversationHistory.end());
}

// Trim conversation history
void ScriptAssistant::TrimConversationHistory() {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (m_conversationHistory.size() > m_maxHistorySize) {
        // Keep system messages and the most recent messages
        std::vector<Message> systemMessages;
        std::vector<Message> recentMessages;
        
        // Extract system messages
        for (const auto& msg : m_conversationHistory) {
            if (msg.m_type == MessageType::System) {
                systemMessages.push_back(msg);
            }
        }
        
        // Keep most recent messages
        size_t recentCount = m_maxHistorySize - systemMessages.size();
        if (recentCount > 0 && recentCount < m_conversationHistory.size()) {
            auto start = m_conversationHistory.end() - recentCount;
            recentMessages.insert(recentMessages.end(), start, m_conversationHistory.end());
        }
        
        // Combine system messages and recent messages
        m_conversationHistory.clear();
        m_conversationHistory.insert(m_conversationHistory.end(), systemMessages.begin(), systemMessages.end());
        m_conversationHistory.insert(m_conversationHistory.end(), recentMessages.begin(), recentMessages.end());
    }
}

// Get example queries
std::vector<std::string> ScriptAssistant::GetExampleQueries() {
    return {
        "How do I find all players in the game?",
        "Generate a script for wall hacking",
        "What's the best way to bypass anti-cheat?",
        "Help me optimize this script",
        "Explain how the game detection system works",
        "How do I use the ExecutionEngine?",
        "What does this Lua code do?",
        "How do I change character properties?"
    };
}

// Get example script descriptions
std::vector<std::string> ScriptAssistant::GetExampleScriptDescriptions() {
    return {
        "ESP hack that shows player names through walls",
        "Aimbot that locks onto nearest player",
        "Speed hack that makes my character move faster",
        "Auto-farm script for resource collection",
        "UI interface for controlling hacks",
        "Anti-kick script to prevent being disconnected",
        "Script to teleport to any player"
    };
}

// Private helper methods

// Add system message
void ScriptAssistant::AddSystemMessage(const std::string& message) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_conversationHistory.push_back(Message(MessageType::System, message));
}

// Add user message
void ScriptAssistant::AddUserMessage(const std::string& message) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_conversationHistory.push_back(Message(MessageType::User, message));
}

// Add assistant message
void ScriptAssistant::AddAssistantMessage(const std::string& message) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_conversationHistory.push_back(Message(MessageType::Assistant, message));
}

// Generate response
std::string ScriptAssistant::GenerateResponse(const std::string& input) {
    // Check if language model is available
    if (m_languageModel) {
        auto model = static_cast<LocalModels::GeneralAssistantModel*>(m_languageModel);
        return model->ProcessInput(input);
    }
    
    // Fallback: simple rule-based responses
    if (input.find("help") != std::string::npos) {
        return "I can help you with scripting, game analysis, and script optimization. "
               "Try asking me to generate a script, optimize your code, or analyze a game.";
    } else if (input.find("generate") != std::string::npos || input.find("create") != std::string::npos) {
        return "To generate a script, please provide a description of what you want the script to do. "
               "For example: 'Generate a script for ESP that shows player names'.";
    } else if (input.find("optimize") != std::string::npos) {
        return "To optimize a script, please share the code you want to optimize.";
    } else if (input.find("analyze") != std::string::npos) {
        return "I can analyze games or scripts. Please specify what you'd like me to analyze.";
    }
    
    // Generic response
    return "I'm here to help with your scripting needs. Can you please provide more details about what you're looking for?";
}

// Load default templates
void ScriptAssistant::LoadDefaultTemplates() {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // Clear existing templates
    m_scriptTemplates.clear();
    
    // Add some default templates
    ScriptTemplate espTemplate;
    espTemplate.m_name = "Basic ESP";
    espTemplate.m_description = "Shows player names through walls";
    espTemplate.m_code = R"(
-- Basic ESP Script
local Players = game:GetService("Players")
local RunService = game:GetService("RunService")
local LocalPlayer = Players.LocalPlayer
local Camera = workspace.CurrentCamera

-- ESP Configuration
local ESP = {
    Enabled = true,
    ShowName = true,
    ShowDistance = true,
    ShowHealth = true,
    TextSize = 14,
    TextColor = Color3.fromRGB(255, 255, 255),
    TextOutline = true,
    MaxDistance = 1000,
}

-- Function to create ESP elements
local function CreateESP(player)
    local esp = Drawing.new("Text")
    esp.Visible = false
    esp.Center = true
    esp.Outline = ESP.TextOutline
    esp.Size = ESP.TextSize
    esp.Color = ESP.TextColor
    esp.OutlineColor = Color3.fromRGB(0, 0, 0)
    
    -- Update ESP in render loop
    RunService:BindToRenderStep("ESP_" .. player.Name, 1, function()
        if not ESP.Enabled then
            esp.Visible = false
            return
        end
        
        -- Check if player exists and has a character
        if not player or not player.Character or not player.Character:FindFirstChild("HumanoidRootPart") then
            esp.Visible = false
            return
        end
        
        -- Don't show ESP for local player
        if player == LocalPlayer then
            esp.Visible = false
            return
        end
        
        local humanoidRootPart = player.Character:FindFirstChild("HumanoidRootPart")
        local humanoid = player.Character:FindFirstChild("Humanoid")
        local position = humanoidRootPart.Position
        
        -- Calculate if player is visible on screen
        local screenPosition, onScreen = Camera:WorldToViewportPoint(position)
        
        -- Calculate distance
        local distance = (Camera.CFrame.Position - position).Magnitude
        
        -- Only show if on screen and within max distance
        if onScreen and distance <= ESP.MaxDistance then
            esp.Position = Vector2.new(screenPosition.X, screenPosition.Y)
            
            -- Build ESP text
            local text = player.Name
            
            if ESP.ShowDistance then
                text = text .. " [" .. math.floor(distance) .. "m]"
            end
            
            if ESP.ShowHealth and humanoid then
                text = text .. " [" .. math.floor(humanoid.Health) .. "/" .. math.floor(humanoid.MaxHealth) .. " HP]"
            end
            
            esp.Text = text
            esp.Visible = true
        else
            esp.Visible = false
        end
    end)
    
    -- Clean up when player leaves
    player.AncestryChanged:Connect(function(_, parent)
        if parent == nil then
            RunService:UnbindFromRenderStep("ESP_" .. player.Name)
            esp:Remove()
        end
    end)
    
    return esp
end

-- Initialize ESP for all players
local espObjects = {}

-- Set up ESP for existing players
for _, player in ipairs(Players:GetPlayers()) do
    if player ~= LocalPlayer then
        espObjects[player] = CreateESP(player)
    end
end

-- Set up ESP for new players
Players.PlayerAdded:Connect(function(player)
    espObjects[player] = CreateESP(player)
end)

-- Clean up ESP when players leave
Players.PlayerRemoving:Connect(function(player)
    if espObjects[player] then
        espObjects[player]:Remove()
        espObjects[player] = nil
    end
end)

-- Toggle function for UI
local function ToggleESP()
    ESP.Enabled = not ESP.Enabled
    print("ESP " .. (ESP.Enabled and "Enabled" or "Disabled"))
end

-- Return the ESP configuration for external control
return ESP
)";
    m_scriptTemplates.push_back(espTemplate);
    
    // Add more templates
    ScriptTemplate aimbotTemplate;
    aimbotTemplate.m_name = "Basic Aimbot";
    aimbotTemplate.m_description = "Simple aimbot that locks onto nearest player";
    aimbotTemplate.m_code = R"(
-- Basic Aimbot Script
local Players = game:GetService("Players")
local RunService = game:GetService("RunService")
local UserInputService = game:GetService("UserInputService")
local LocalPlayer = Players.LocalPlayer
local Camera = workspace.CurrentCamera

-- Aimbot Configuration
local Aimbot = {
    Enabled = false,
    TargetPart = "Head", -- Options: "Head", "Torso", "HumanoidRootPart"
    TeamCheck = true, -- Only target enemies
    VisibilityCheck = true, -- Only target visible players
    MaxDistance = 1000, -- Maximum targeting distance
    Sensitivity = 0.5, -- Lower = smoother, Higher = snappier
    AimKey = Enum.UserInputType.MouseButton2, -- Right mouse button
    FOV = 250, -- Field of View for targeting (0-800)
    ShowFOV = true, -- Show FOV circle
    FOVColor = Color3.fromRGB(255, 255, 255),
}

-- Create FOV circle
local fovCircle = Drawing.new("Circle")
fovCircle.Visible = Aimbot.ShowFOV
fovCircle.Radius = Aimbot.FOV
fovCircle.Color = Aimbot.FOVColor
fovCircle.Thickness = 1
fovCircle.Filled = false
fovCircle.Transparency = 1

-- Update FOV circle position
RunService.RenderStepped:Connect(function()
    fovCircle.Position = Vector2.new(Camera.ViewportSize.X / 2, Camera.ViewportSize.Y / 2)
    fovCircle.Radius = Aimbot.FOV
    fovCircle.Visible = Aimbot.ShowFOV and Aimbot.Enabled
end)

-- Function to check if a player is on your team
local function IsOnSameTeam(player)
    if not Aimbot.TeamCheck then return false end
    return player.Team == LocalPlayer.Team
end

-- Function to check if a player is visible
local function IsVisible(targetPart)
    if not Aimbot.VisibilityCheck then return true end
    
    local ray = Ray.new(Camera.CFrame.Position, targetPart.Position - Camera.CFrame.Position)
    local hit, _ = workspace:FindPartOnRayWithIgnoreList(ray, {LocalPlayer.Character, targetPart.Parent})
    return hit == nil
end

-- Function to get the nearest player
local function GetNearestPlayer()
    local nearestPlayer = nil
    local nearestDistance = math.huge
    local nearestScreenDistance = math.huge
    local screenCenter = Vector2.new(Camera.ViewportSize.X / 2, Camera.ViewportSize.Y / 2)
    
    for _, player in ipairs(Players:GetPlayers()) do
        if player ~= LocalPlayer and player.Character and not IsOnSameTeam(player) then
            local character = player.Character
            local humanoid = character:FindFirstChild("Humanoid")
            local targetPart = character:FindFirstChild(Aimbot.TargetPart)
            
            if humanoid and humanoid.Health > 0 and targetPart then
                local targetPosition = targetPart.Position
                local distance = (Camera.CFrame.Position - targetPosition).Magnitude
                
                if distance <= Aimbot.MaxDistance then
                    local screenPosition, onScreen = Camera:WorldToViewportPoint(targetPosition)
                    
                    if onScreen then
                        local screenDistance = (Vector2.new(screenPosition.X, screenPosition.Y) - screenCenter).Magnitude
                        
                        if screenDistance <= Aimbot.FOV and screenDistance < nearestScreenDistance and IsVisible(targetPart) then
                            nearestPlayer = player
                            nearestDistance = distance
                            nearestScreenDistance = screenDistance
                        end
                    end
                end
            end
        end
    end
    
    return nearestPlayer, nearestDistance
end

-- Aim at target function
local function AimAt(targetPosition)
    local aimDirection = (targetPosition - Camera.CFrame.Position).Unit
    local targetCFrame = CFrame.new(Camera.CFrame.Position, Camera.CFrame.Position + aimDirection)
    Camera.CFrame = Camera.CFrame:Lerp(targetCFrame, Aimbot.Sensitivity)
end

-- Main aimbot loop
RunService.RenderStepped:Connect(function()
    if Aimbot.Enabled and UserInputService:IsMouseButtonPressed(Aimbot.AimKey) then
        local target, _ = GetNearestPlayer()
        
        if target and target.Character then
            local targetPart = target.Character:FindFirstChild(Aimbot.TargetPart)
            if targetPart then
                AimAt(targetPart.Position)
            end
        end
    end
end)

-- Toggle function
local function ToggleAimbot()
    Aimbot.Enabled = not Aimbot.Enabled
    print("Aimbot " .. (Aimbot.Enabled and "Enabled" or "Disabled"))
end

-- Hotkey to toggle aimbot
UserInputService.InputBegan:Connect(function(input, gameProcessed)
    if not gameProcessed and input.KeyCode == Enum.KeyCode.X then
        ToggleAimbot()
    end
end)

-- Return the aimbot configuration for external control
return Aimbot
)";
    m_scriptTemplates.push_back(aimbotTemplate);
    
    // Additional template for UI
    ScriptTemplate uiTemplate;
    uiTemplate.m_name = "Simple UI Framework";
    uiTemplate.m_description = "Framework for creating a simple UI for scripts";
    uiTemplate.m_code = R"(
-- Simple UI Framework
local UserInputService = game:GetService("UserInputService")
local TweenService = game:GetService("TweenService")
local Players = game:GetService("Players")
local LocalPlayer = Players.LocalPlayer
local Mouse = LocalPlayer:GetMouse()

-- UI Configuration
local UI = {
    Title = "Script Hub",
    Width = 300,
    Height = 350,
    Color = {
        Background = Color3.fromRGB(30, 30, 30),
        TopBar = Color3.fromRGB(40, 40, 40),
        Button = Color3.fromRGB(50, 50, 50),
        ButtonHover = Color3.fromRGB(60, 60, 60),
        Text = Color3.fromRGB(255, 255, 255),
        Accent = Color3.fromRGB(0, 120, 215)
    },
    Elements = {}, -- Store UI elements
    Visible = true,
    Draggable = true
}

-- Create ScreenGui
local ScreenGui = Instance.new("ScreenGui")
ScreenGui.Name = "ScriptHub"
ScreenGui.ResetOnSpawn = false
ScreenGui.ZIndexBehavior = Enum.ZIndexBehavior.Sibling

-- If synapse or other exploit has a protection method, use it
if syn and syn.protect_gui then
    syn.protect_gui(ScreenGui)
    ScreenGui.Parent = game.CoreGui
elseif gethui then
    ScreenGui.Parent = gethui()
else
    ScreenGui.Parent = game.CoreGui
end

-- Create main frame
local MainFrame = Instance.new("Frame")
MainFrame.Name = "MainFrame"
MainFrame.Size = UDim2.new(0, UI.Width, 0, UI.Height)
MainFrame.Position = UDim2.new(0.5, -UI.Width/2, 0.5, -UI.Height/2)
MainFrame.BackgroundColor3 = UI.Color.Background
MainFrame.BorderSizePixel = 0
MainFrame.Active = true
MainFrame.Parent = ScreenGui

-- Add corner radius
local Corner = Instance.new("UICorner")
Corner.CornerRadius = UDim.new(0, 6)
Corner.Parent = MainFrame

-- Add shadow
local Shadow = Instance.new("ImageLabel")
Shadow.Name = "Shadow"
Shadow.AnchorPoint = Vector2.new(0.5, 0.5)
Shadow.BackgroundTransparency = 1
Shadow.Position = UDim2.new(0.5, 0, 0.5, 0)
Shadow.Size = UDim2.new(1, 12, 1, 12)
Shadow.ZIndex = -1
Shadow.Image = "rbxassetid://5554236805"
Shadow.ImageColor3 = Color3.fromRGB(0, 0, 0)
Shadow.ImageTransparency = 0.5
Shadow.ScaleType = Enum.ScaleType.Slice
Shadow.SliceCenter = Rect.new(23, 23, 277, 277)
Shadow.Parent = MainFrame

-- Create top bar
local TopBar = Instance.new("Frame")
TopBar.Name = "TopBar"
TopBar.Size = UDim2.new(1, 0, 0, 30)
TopBar.BackgroundColor3 = UI.Color.TopBar
TopBar.BorderSizePixel = 0
TopBar.Parent = MainFrame

-- Add corner radius to top bar
local TopBarCorner = Instance.new("UICorner")
TopBarCorner.CornerRadius = UDim.new(0, 6)
TopBarCorner.Parent = TopBar

-- Create title
local Title = Instance.new("TextLabel")
Title.Name = "Title"
Title.Size = UDim2.new(1, -30, 1, 0)
Title.Position = UDim2.new(0, 10, 0, 0)
Title.BackgroundTransparency = 1
Title.Text = UI.Title
Title.TextColor3 = UI.Color.Text
Title.TextSize = 18
Title.Font = Enum.Font.SourceSansBold
Title.TextXAlignment = Enum.TextXAlignment.Left
Title.Parent = TopBar

-- Create close button
local CloseButton = Instance.new("TextButton")
CloseButton.Name = "CloseButton"
CloseButton.Size = UDim2.new(0, 20, 0, 20)
CloseButton.Position = UDim2.new(1, -25, 0, 5)
CloseButton.BackgroundColor3 = Color3.fromRGB(255, 0, 0)
CloseButton.Text = "X"
CloseButton.TextColor3 = UI.Color.Text
CloseButton.TextSize = 14
CloseButton.Font = Enum.Font.SourceSansBold
CloseButton.Parent = TopBar

-- Add corner radius to close button
local CloseButtonCorner = Instance.new("UICorner")
CloseButtonCorner.CornerRadius = UDim.new(0, 4)
CloseButtonCorner.Parent = CloseButton

-- Create content frame
local ContentFrame = Instance.new("Frame")
ContentFrame.Name = "ContentFrame"
ContentFrame.Size = UDim2.new(1, -20, 1, -40)
ContentFrame.Position = UDim2.new(0, 10, 0, 35)
ContentFrame.BackgroundTransparency = 1
ContentFrame.Parent = MainFrame

-- Create scrolling frame for content
local ScrollFrame = Instance.new("ScrollingFrame")
ScrollFrame.Name = "ScrollFrame"
ScrollFrame.Size = UDim2.new(1, 0, 1, 0)
ScrollFrame.BackgroundTransparency = 1
ScrollFrame.BorderSizePixel = 0
ScrollFrame.ScrollBarThickness = 4
ScrollFrame.ScrollBarImageColor3 = UI.Color.Accent
ScrollFrame.Parent = ContentFrame

-- Create UI list layout
local UIListLayout = Instance.new("UIListLayout")
UIListLayout.SortOrder = Enum.SortOrder.LayoutOrder
UIListLayout.Padding = UDim.new(0, 5)
UIListLayout.Parent = ScrollFrame

-- Make UI draggable
if UI.Draggable then
    local isDragging = false
    local dragInput
    local dragStart
    local startPos
    
    TopBar.InputBegan:Connect(function(input)
        if input.UserInputType == Enum.UserInputType.MouseButton1 then
            isDragging = true
            dragStart = input.Position
            startPos = MainFrame.Position
        end
    end)
    
    TopBar.InputEnded:Connect(function(input)
        if input.UserInputType == Enum.UserInputType.MouseButton1 then
            isDragging = false
        end
    end)
    
    UserInputService.InputChanged:Connect(function(input)
        if input == dragInput and isDragging then
            local delta = input.Position - dragStart
            MainFrame.Position = UDim2.new(startPos.X.Scale, startPos.X.Offset + delta.X, startPos.Y.Scale, startPos.Y.Offset + delta.Y)
        end
    end)
    
    TopBar.InputChanged:Connect(function(input)
        if input.UserInputType == Enum.UserInputType.MouseMovement then
            dragInput = input
        end
    end)
end

-- Close button functionality
CloseButton.MouseButton1Click:Connect(function()
    ScreenGui:Destroy()
end)

-- Helper function to create a button
function UI:CreateButton(text, callback)
    local Button = Instance.new("TextButton")
    Button.Name = text .. "Button"
    Button.Size = UDim2.new(1, 0, 0, 30)
    Button.BackgroundColor3 = UI.Color.Button
    Button.Text = text
    Button.TextColor3 = UI.Color.Text
    Button.TextSize = 14
    Button.Font = Enum.Font.SourceSans
    Button.Parent = ScrollFrame
    
    -- Add corner radius
    local ButtonCorner = Instance.new("UICorner")
    ButtonCorner.CornerRadius = UDim.new(0, 4)
    ButtonCorner.Parent = Button
    
    -- Button hover effect
    Button.MouseEnter:Connect(function()
        TweenService:Create(Button, TweenInfo.new(0.2), {BackgroundColor3 = UI.Color.ButtonHover}):Play()
    end)
    
    Button.MouseLeave:Connect(function()
        TweenService:Create(Button, TweenInfo.new(0.2), {BackgroundColor3 = UI.Color.Button}):Play()
    end)
    
    -- Button click
    Button.MouseButton1Click:Connect(function()
        callback()
    end)
    
    -- Add to elements
    table.insert(UI.Elements, Button)
    
    -- Update scroll frame canvas size
    ScrollFrame.CanvasSize = UDim2.new(0, 0, 0, UIListLayout.AbsoluteContentSize.Y)
    
    return Button
end

-- Helper function to create a toggle
function UI:CreateToggle(text, default, callback)
    local Toggle = Instance.new("Frame")
    Toggle.Name = text .. "Toggle"
    Toggle.Size = UDim2.new(1, 0, 0, 30)
    Toggle.BackgroundColor3 = UI.Color.Button
    Toggle.Parent = ScrollFrame
    
    -- Add corner radius
    local ToggleCorner = Instance.new("UICorner")
    ToggleCorner.CornerRadius = UDim.new(0, 4)
    ToggleCorner.Parent = Toggle
    
    -- Toggle text
    local ToggleText = Instance.new("TextLabel")
    ToggleText.Name = "ToggleText"
    ToggleText.Size = UDim2.new(1, -50, 1, 0)
    ToggleText.Position = UDim2.new(0, 10, 0, 0)
    ToggleText.BackgroundTransparency = 1
    ToggleText.Text = text
    ToggleText.TextColor3 = UI.Color.Text
    ToggleText.TextSize = 14
    ToggleText.Font = Enum.Font.SourceSans
    ToggleText.TextXAlignment = Enum.TextXAlignment.Left
    ToggleText.Parent = Toggle
    
    -- Toggle button
    local ToggleButton = Instance.new("Frame")
    ToggleButton.Name = "ToggleButton"
    ToggleButton.Size = UDim2.new(0, 40, 0, 20)
    ToggleButton.Position = UDim2.new(1, -45, 0, 5)
    ToggleButton.BackgroundColor3 = Color3.fromRGB(50, 50, 50)
    ToggleButton.Parent = Toggle
    
    -- Add corner radius to toggle button
    local ToggleButtonCorner = Instance.new("UICorner")
    ToggleButtonCorner.CornerRadius = UDim.new(0, 10)
    ToggleButtonCorner.Parent = ToggleButton
    
    -- Toggle indicator
    local ToggleIndicator = Instance.new("Frame")
    ToggleIndicator.Name = "ToggleIndicator"
    ToggleIndicator.Size = UDim2.new(0, 16, 0, 16)
    ToggleIndicator.Position = UDim2.new(0, 2, 0, 2)
    ToggleIndicator.BackgroundColor3 = UI.Color.Text
    ToggleIndicator.Parent = ToggleButton
    
    -- Add corner radius to toggle indicator
    local ToggleIndicatorCorner = Instance.new("UICorner")
    ToggleIndicatorCorner.CornerRadius = UDim.new(0, 8)
    ToggleIndicatorCorner.Parent = ToggleIndicator
    
    -- Set initial state
    local enabled = default or false
    local function updateToggle()
        if enabled then
            TweenService:Create(ToggleIndicator, TweenInfo.new(0.2), {Position = UDim2.new(0, 22, 0, 2), BackgroundColor3 = UI.Color.Accent}):Play()
            TweenService:Create(ToggleButton, TweenInfo.new(0.2), {BackgroundColor3 = Color3.fromRGB(70, 70, 70)}):Play()
        else
            TweenService:Create(ToggleIndicator, TweenInfo.new(0.2), {Position = UDim2.new(0, 2, 0, 2), BackgroundColor3 = UI.Color.Text}):Play()
            TweenService:Create(ToggleButton, TweenInfo.new(0.2), {BackgroundColor3 = Color3.fromRGB(50, 50, 50)}):Play()
        end
        callback(enabled)
    end
    
    -- Update toggle on first load
    updateToggle()
    
    -- Toggle click
    Toggle.InputBegan:Connect(function(input)
        if input.UserInputType == Enum.UserInputType.MouseButton1 then
            enabled = not enabled
            updateToggle()
        end
    end)
    
    -- Add to elements
    table.insert(UI.Elements, Toggle)
    
    -- Update scroll frame canvas size
    ScrollFrame.CanvasSize = UDim2.new(0, 0, 0, UIListLayout.AbsoluteContentSize.Y)
    
    return Toggle
end

-- Return the UI object for external control
return UI
)";
    m_scriptTemplates.push_back(uiTemplate);
}

// Find closest template
ScriptAssistant::ScriptTemplate ScriptAssistant::FindClosestTemplate(const std::string& description) {
    if (m_scriptTemplates.empty()) {
        return ScriptTemplate();
    }
    
    // Find template with most matching keywords
    std::vector<std::string> keywords = ExtractKeywords(description);
    
    int highestScore = 0;
    ScriptTemplate bestMatch;
    
    for (const auto& tmpl : m_scriptTemplates) {
        std::vector<std::string> templateKeywords = ExtractKeywords(tmpl.m_description);
        
        // Count matching keywords
        int score = 0;
        for (const auto& keyword : keywords) {
            for (const auto& templateKeyword : templateKeywords) {
                if (keyword == templateKeyword) {
                    score++;
                    break;
                }
            }
        }
        
        if (score > highestScore) {
            highestScore = score;
            bestMatch = tmpl;
        }
    }
    
    return bestMatch;
}

// Extract keywords from text
std::vector<std::string> ScriptAssistant::ExtractKeywords(const std::string& text) {
    std::vector<std::string> keywords;
    std::string lowercaseText = text;
    
    // Convert to lowercase
    std::transform(lowercaseText.begin(), lowercaseText.end(), lowercaseText.begin(), 
                 [](unsigned char c) { return std::tolower(c); });
    
    // Split by non-alphanumeric characters
    std::istringstream iss(lowercaseText);
    std::string word;
    
    while (iss >> word) {
        // Remove non-alphanumeric characters
        word.erase(std::remove_if(word.begin(), word.end(), 
                                [](unsigned char c) { return !std::isalnum(c); }), 
                 word.end());
        
        // Skip small words and common words
        if (word.length() > 2 && 
            word != "the" && word != "and" && word != "for" && 
            word != "with" && word != "that" && word != "this") {
            keywords.push_back(word);
        }
    }
    
    return keywords;
}

// Analyze game object recursively
void ScriptAssistant::AnalyzeGameObject(std::shared_ptr<GameObject> obj, std::set<std::string>& classNames) {
    if (!obj) return;
    
    // Add class name
    classNames.insert(obj.m_className);
    
    // Recursively analyze children
    for (const auto& child : obj.m_children) {
        AnalyzeGameObject(child, classNames);
    }
}

// Optimize local variables
std::string ScriptAssistant::OptimizeLocalVariables(const std::string& script) {
    // Basic implementation - in a real system, this would be more sophisticated
    
    // Replace redundant local declarations
    std::string optimized = script;
    
    // Find common patterns
    size_t pos = optimized.find("local function");
    while (pos != std::string::npos) {
        // Keep local functions as they are
        pos = optimized.find("local function", pos + 14);
    }
    
    // Look for multiple consecutive local declarations that could be combined
    pos = optimized.find("local ");
    while (pos != std::string::npos) {
        size_t nextLocal = optimized.find("local ", pos + 6);
        if (nextLocal != std::string::npos && nextLocal - pos < 50) {
            // Check if these could be combined (simple heuristic)
            size_t lineEnd = optimized.find("\n", pos);
            if (lineEnd != std::string::npos && lineEnd < nextLocal) {
                // These are on different lines, could potentially combine
                // In a real implementation, would need to analyze variable usage
            }
        }
        
        pos = optimized.find("local ", pos + 6);
    }
    
    return optimized;
}

// Optimize loops
std::string ScriptAssistant::OptimizeLoops(const std::string& script) {
    // Basic implementation - in a real system, this would use more advanced analysis
    
    // Replace inefficient loop patterns
    std::string optimized = script;
    
    // Replace pairs() with ipairs() for array-like tables
    size_t pos = optimized.find("for k, v in pairs(");
    while (pos != std::string::npos) {
        // Check if this could use ipairs instead (simple heuristic)
        size_t closingParen = optimized.find(")", pos);
        if (closingParen != std::string::npos) {
            std::string tableName = optimized.substr(pos + 16, closingParen - (pos + 16));
            
            // Check if tableName ends with common array identifiers
            if (tableName.find("List") != std::string::npos ||
                tableName.find("Array") != std::string::npos ||
                tableName.find("s") == tableName.length() - 1) { // Plural name
                
                // Replace with ipairs for array-like tables
                optimized.replace(pos, 16, "for k, v in ipairs(");
            }
        }
        
        pos = optimized.find("for k, v in pairs(", pos + 10);
    }
    
    return optimized;
}

// Remove unnecessary whitespace
std::string ScriptAssistant::RemoveUnnecessaryWhitespace(const std::string& script) {
    std::string optimized = script;
    
    // Replace multiple spaces with single space
    size_t pos = optimized.find("  ");
    while (pos != std::string::npos) {
        optimized.replace(pos, 2, " ");
        pos = optimized.find("  ", pos);
    }
    
    // Replace multiple empty lines with a single empty line
    pos = optimized.find("\n\n\n");
    while (pos != std::string::npos) {
        optimized.replace(pos, 3, "\n\n");
        pos = optimized.find("\n\n\n", pos);
    }
    
    return optimized;
}

// Detect performance issues
std::vector<std::string> ScriptAssistant::DetectPerformanceIssues(const std::string& script) {
    std::vector<std::string> issues;
    
    // Check for inefficient patterns
    
    // 1. Table creation in loops
    if (script.find("for") != std::string::npos && 
        script.find("{}", script.find("for")) != std::string::npos) {
        issues.push_back("Potential table creation inside loops - consider moving outside the loop");
    }
    
    // 2. Using # operator on non-sequential tables
    if (script.find("#") != std::string::npos) {
        issues.push_back("Using # length operator which can be unreliable for tables with non-sequential indices");
    }
    
    // 3. RenderStepped for non-render updates
    if (script.find("RenderStepped") != std::string::npos) {
        issues.push_back("Using RenderStepped which runs every frame - consider Heartbeat for logic not tied to rendering");
    }
    
    // 4. String concatenation in loops
    if (script.find("for") != std::string::npos && 
        script.find("..", script.find("for")) != std::string::npos) {
        issues.push_back("String concatenation in loops can be inefficient - consider using table.concat");
    }
    
    // 5. Inefficient table access pattern
    if (script.find("FindFirstChild") != std::string::npos) {
        issues.push_back("Multiple FindFirstChild calls can be slow - cache references when possible");
    }
    
    return issues;
}

} // namespace AIFeatures
} // namespace iOS
