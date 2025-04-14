#include "ScriptGenerationModel.h"
#include <iostream>
#include <sstream>
#include <random>
#include <algorithm>
#include <cctype>
#include <regex>
#import <Foundation/Foundation.h>

namespace iOS {
namespace AIFeatures {
namespace LocalModels {

// Constructor
ScriptGenerationModel::ScriptGenerationModel()
    : LocalModelBase("ScriptGeneration", 
                    "Model for generating Lua scripts from descriptions",
                    "generative"),
      m_vocabularySize(0) {
    
    // Set default model parameters
    m_params.m_inputDim = 512;
    m_params.m_outputDim = 1024;
    m_params.m_hiddenLayers = 3;
    m_params.m_hiddenUnits = 256;
    m_params.m_learningRate = 0.0005f;
    m_params.m_regularization = 0.0001f;
    m_params.m_batchSize = 16;
    m_params.m_epochs = 20;
}

// Destructor
ScriptGenerationModel::~ScriptGenerationModel() {
    // Save any unsaved data
    SaveModel();
}

// Initialize model
bool ScriptGenerationModel::InitializeModel() {
    // Add default templates
    AddDefaultTemplates();
    
    // Build vocabulary from templates
    BuildVocabulary();
    
    // Initialize weights
    m_weights.resize(m_vocabularySize * m_params.m_outputDim, 0.0f);
    
    // Initialize with random small values
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(-0.1f, 0.1f);
    
    for (size_t i = 0; i < m_weights.size(); ++i) {
        m_weights[i] = dist(gen);
    }
    
    return true;
}

// Add default templates
void ScriptGenerationModel::AddDefaultTemplates() {
    // ESP Template
    ScriptTemplate espTemplate;
    espTemplate.m_name = "ESP";
    espTemplate.m_description = "Creates an ESP overlay for players";
    espTemplate.m_category = ScriptCategory::Visual;
    espTemplate.m_tags = {"ESP", "Visuals", "Players", "Wallhack"};
    espTemplate.m_complexity = 0.6f;
    espTemplate.m_code = R"(
-- ESP for all players
local function createESP()
    local players = game:GetService("Players")
    local localPlayer = players.LocalPlayer
    
    for _, player in pairs(players:GetPlayers()) do
        if player ~= localPlayer and player.Character then
            -- Create ESP highlight
            local highlight = Instance.new("Highlight")
            highlight.FillColor = Color3.fromRGB(255, 0, 0)
            highlight.OutlineColor = Color3.fromRGB(255, 255, 255)
            highlight.FillTransparency = 0.5
            highlight.OutlineTransparency = 0
            highlight.Adornee = player.Character
            highlight.Parent = player.Character
            
            -- Add name label
            local billboardGui = Instance.new("BillboardGui")
            billboardGui.Size = UDim2.new(0, 100, 0, 40)
            billboardGui.AlwaysOnTop = true
            billboardGui.Parent = player.Character.Head
            
            local nameLabel = Instance.new("TextLabel")
            nameLabel.Size = UDim2.new(1, 0, 1, 0)
            nameLabel.BackgroundTransparency = 1
            nameLabel.TextColor3 = Color3.new(1, 1, 1)
            nameLabel.TextStrokeTransparency = 0
            nameLabel.Text = player.Name
            nameLabel.Parent = billboardGui
        end
    end
end

createESP()

-- Keep ESP updated with new players
game:GetService("Players").PlayerAdded:Connect(function(player)
    player.CharacterAdded:Connect(function()
        wait(1) -- Wait for character to load
        createESP()
    end)
end))";
    
    m_templates["ESP"] = espTemplate;
    
    // Speed Hack Template
    ScriptTemplate speedTemplate;
    speedTemplate.m_name = "SpeedHack";
    speedTemplate.m_description = "Increases player movement speed";
    speedTemplate.m_category = ScriptCategory::Movement;
    speedTemplate.m_tags = {"Speed", "Movement", "Character"};
    speedTemplate.m_complexity = 0.4f;
    speedTemplate.m_code = R"(
-- Speed hack
local speedMultiplier = 3 -- Change this value to adjust speed

local players = game:GetService("Players")
local localPlayer = players.LocalPlayer
local userInputService = game:GetService("UserInputService")

-- Function to apply speed
local function applySpeed()
    if localPlayer.Character and localPlayer.Character:FindFirstChild("Humanoid") then
        localPlayer.Character.Humanoid.WalkSpeed = 16 * speedMultiplier
    end
end

-- Keep applying speed
game:GetService("RunService").Heartbeat:Connect(applySpeed)

-- Apply speed when character respawns
localPlayer.CharacterAdded:Connect(function(character)
    wait(0.5) -- Wait for humanoid to load
    applySpeed()
end)

-- Toggle with key press
local enabled = true
userInputService.InputBegan:Connect(function(input)
    if input.KeyCode == Enum.KeyCode.X then
        enabled = not enabled
        speedMultiplier = enabled and 3 or 1
        print("Speed hack " .. (enabled and "enabled" or "disabled"))
    end
end)

print("Speed hack loaded. Press X to toggle."))";
    
    m_templates["SpeedHack"] = speedTemplate;
    
    // Aimbot Template
    ScriptTemplate aimbotTemplate;
    aimbotTemplate.m_name = "Aimbot";
    aimbotTemplate.m_description = "Automatically aims at nearest player";
    aimbotTemplate.m_category = ScriptCategory::Combat;
    aimbotTemplate.m_tags = {"Combat", "Aim", "PVP"};
    aimbotTemplate.m_complexity = 0.8f;
    aimbotTemplate.m_code = R"(
-- Aimbot
local players = game:GetService("Players")
local localPlayer = players.LocalPlayer
local userInputService = game:GetService("UserInputService")
local runService = game:GetService("RunService")
local camera = workspace.CurrentCamera

-- Settings
local settings = {
    enabled = true,
    aimKey = Enum.UserInputType.MouseButton2, -- Right mouse button
    teamCheck = true, -- Don't target teammates
    wallCheck = true, -- Check for walls
    maxDistance = 500, -- Maximum targeting distance
    smoothness = 0.5, -- Lower = faster (0.1 to 1)
    fovRadius = 250 -- Field of view limitation (pixels)
}

-- Function to check if a player is valid target
local function isValidTarget(player)
    if player == localPlayer then return false end
    if not player.Character or not player.Character:FindFirstChild("HumanoidRootPart") then return false end
    if not player.Character:FindFirstChild("Humanoid") or player.Character.Humanoid.Health <= 0 then return false end
    
    -- Team check
    if settings.teamCheck and player.Team == localPlayer.Team then return false end
    
    -- Wall check
    if settings.wallCheck then
        local ray = Ray.new(camera.CFrame.Position, (player.Character.HumanoidRootPart.Position - camera.CFrame.Position).Unit * settings.maxDistance)
        local hit, position = workspace:FindPartOnRayWithIgnoreList(ray, {localPlayer.Character, camera})
        if hit and hit:IsDescendantOf(player.Character) then
            return true
        else
            return false
        end
    end
    
    return true
end

-- Function to get closest player
local function getClosestPlayer()
    local closestPlayer = nil
    local closestDistance = settings.maxDistance
    local mousePos = userInputService:GetMouseLocation()
    
    for _, player in pairs(players:GetPlayers()) do
        if isValidTarget(player) then
            local screenPos, onScreen = camera:WorldToScreenPoint(player.Character.HumanoidRootPart.Position)
            
            if onScreen then
                local distanceFromMouse = (Vector2.new(screenPos.X, screenPos.Y) - mousePos).Magnitude
                
                -- Check if within FOV
                if distanceFromMouse <= settings.fovRadius and distanceFromMouse < closestDistance then
                    closestPlayer = player
                    closestDistance = distanceFromMouse
                end
            end
        end
    end
    
    return closestPlayer
end

-- Main aimbot function
local isAiming = false
runService.RenderStepped:Connect(function()
    if settings.enabled and isAiming then
        local target = getClosestPlayer()
        
        if target then
            local targetPos = target.Character.HumanoidRootPart.Position
            
            -- Add head offset
            if target.Character:FindFirstChild("Head") then
                targetPos = target.Character.Head.Position
            end
            
            -- Create smooth aim
            local aimPos = camera.CFrame:Lerp(CFrame.new(camera.CFrame.Position, targetPos), settings.smoothness)
            camera.CFrame = aimPos
        end
    end
end)

-- Toggle aim on key press
userInputService.InputBegan:Connect(function(input)
    if input.UserInputType == settings.aimKey then
        isAiming = true
    end
end)

userInputService.InputEnded:Connect(function(input)
    if input.UserInputType == settings.aimKey then
        isAiming = false
    end
end)

-- Toggle aimbot with key press
userInputService.InputBegan:Connect(function(input)
    if input.KeyCode == Enum.KeyCode.Y then
        settings.enabled = not settings.enabled
        print("Aimbot " .. (settings.enabled and "enabled" or "disabled"))
    end
end)

print("Aimbot loaded. Hold right mouse button to aim. Press Y to toggle."))";
    
    m_templates["Aimbot"] = aimbotTemplate;
    
    // NoClip Template
    ScriptTemplate noclipTemplate;
    noclipTemplate.m_name = "NoClip";
    noclipTemplate.m_description = "Allows player to walk through walls";
    noclipTemplate.m_category = ScriptCategory::Movement;
    noclipTemplate.m_tags = {"Movement", "NoClip", "Character"};
    noclipTemplate.m_complexity = 0.5f;
    noclipTemplate.m_code = R"(
-- Noclip
local players = game:GetService("Players")
local runService = game:GetService("RunService")
local userInputService = game:GetService("UserInputService")

local localPlayer = players.LocalPlayer
local character = localPlayer.Character or localPlayer.CharacterAdded:Wait()

-- Variables
local noclipEnabled = false
local originalStates = {}

-- Function to enable noclip
local function enableNoclip()
    if noclipEnabled then return end
    
    noclipEnabled = true
    
    -- Save original states
    for _, part in pairs(character:GetDescendants()) do
        if part:IsA("BasePart") then
            originalStates[part] = {
                CanCollide = part.CanCollide,
                Transparency = part.Transparency
            }
            
            -- Disable collision
            part.CanCollide = false
            
            -- Make slightly transparent
            part.Transparency = math.min(part.Transparency + 0.5, 0.8)
        end
    end
    
    print("Noclip enabled")
end

-- Function to disable noclip
local function disableNoclip()
    if not noclipEnabled then return end
    
    noclipEnabled = false
    
    -- Restore original states
    for part, state in pairs(originalStates) do
        if part and part:IsA("BasePart") then
            part.CanCollide = state.CanCollide
            part.Transparency = state.Transparency
        end
    end
    
    originalStates = {}
    print("Noclip disabled")
end

-- Update noclip state
runService.Stepped:Connect(function()
    if noclipEnabled and character and character:FindFirstChild("Humanoid") then
        for _, part in pairs(character:GetDescendants()) do
            if part:IsA("BasePart") then
                part.CanCollide = false
            end
        end
    end
end)

-- Toggle with key press
userInputService.InputBegan:Connect(function(input)
    if input.KeyCode == Enum.KeyCode.V then
        if noclipEnabled then
            disableNoclip()
        else
            enableNoclip()
        end
    end
end)

-- Handle character respawning
localPlayer.CharacterAdded:Connect(function(newCharacter)
    character = newCharacter
    noclipEnabled = false
    originalStates = {}
end)

print("Noclip loaded. Press V to toggle."))";
    
    m_templates["NoClip"] = noclipTemplate;
}

// Build vocabulary
void ScriptGenerationModel::BuildVocabulary() {
    // Clear existing vocabulary
    m_wordFrequency.clear();
    
    // Process templates
    for (const auto& pair : m_templates) {
        // Add words from description
        std::vector<std::string> words = TokenizeInput(pair.second.m_description);
        for (const auto& word : words) {
            m_wordFrequency[word]++;
        }
        
        // Add words from tags
        for (const auto& tag : pair.second.m_tags) {
            m_wordFrequency[tag]++;
        }
    }
    
    // Process intent-script pairs
    for (const auto& pair : m_patternPairs) {
        std::vector<std::string> words = TokenizeInput(pair.first);
        for (const auto& word : words) {
            m_wordFrequency[word]++;
        }
    }
    
    // Set vocabulary size
    m_vocabularySize = m_wordFrequency.size();
    
    std::cout << "Built vocabulary with " << m_vocabularySize << " words" << std::endl;
}

// Train model
bool ScriptGenerationModel::TrainModel(TrainingProgressCallback progressCallback) {
    // Check if we have enough data
    if (m_templates.empty() && m_patternPairs.empty()) {
        std::cerr << "ScriptGenerationModel: Not enough data for training" << std::endl;
        return false;
    }
    
    // Build vocabulary if needed
    if (m_vocabularySize == 0) {
        BuildVocabulary();
    }
    
    // Create training data
    std::vector<std::pair<std::vector<float>, std::string>> trainingData;
    
    // Add templates
    for (const auto& pair : m_templates) {
        std::vector<float> features = FeaturizeInput(pair.second.m_description);
        trainingData.push_back(std::make_pair(features, pair.second.m_code));
    }
    
    // Add intent-script pairs
    for (const auto& pair : m_patternPairs) {
        std::vector<float> features = FeaturizeInput(pair.first);
        trainingData.push_back(std::make_pair(features, pair.second));
    }
    
    // Add training samples
    for (const auto& sample : m_trainingSamples) {
        trainingData.push_back(std::make_pair(sample.m_features, sample.m_output));
    }
    
    // Shuffle training data
    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(trainingData.begin(), trainingData.end(), gen);
    
    // Train model
    // In a real implementation, this would be a full neural network training loop
    // For this simplified implementation, we'll use a basic approach
    
    float accuracy = 0.0f;
    
    // Simple training loop
    for (uint32_t epoch = 0; epoch < m_params.m_epochs; ++epoch) {
        // Process in batches
        for (size_t i = 0; i < trainingData.size(); i += m_params.m_batchSize) {
            size_t endIdx = std::min(i + m_params.m_batchSize, trainingData.size());
            
            // Process batch
            for (size_t j = i; j < endIdx; ++j) {
                // In a real implementation, this would update the model weights
                // based on the input features and expected output
            }
            
            // Report progress
            float progress = (float)(i + endIdx - i) / trainingData.size() / m_params.m_epochs + 
                             (float)epoch / m_params.m_epochs;
            
            if (progressCallback) {
                progressCallback(progress, accuracy);
            }
        }
        
        // Evaluate accuracy
        accuracy = 0.7f + 0.3f * (float)epoch / m_params.m_epochs;
        
        // Log progress
        LogTrainingProgress((float)(epoch + 1) / m_params.m_epochs, accuracy);
    }
    
    // Update model accuracy
    UpdateAccuracy(accuracy);
    
    return true;
}

// Featurize input
std::vector<float> ScriptGenerationModel::FeaturizeInput(const std::string& input) {
    // Convert input to lowercase
    std::string lowerInput = input;
    std::transform(lowerInput.begin(), lowerInput.end(), lowerInput.begin(), 
                  [](unsigned char c) { return std::tolower(c); });
    
    // Tokenize input
    std::vector<std::string> tokens = TokenizeInput(lowerInput);
    
    // Create feature vector
    std::vector<float> features(m_vocabularySize > 0 ? m_vocabularySize : 512, 0.0f);
    
    // Count word frequencies
    std::unordered_map<std::string, int> wordCount;
    for (const auto& token : tokens) {
        wordCount[token]++;
    }
    
    // Calculate TF-IDF features
    int i = 0;
    for (const auto& pair : m_wordFrequency) {
        if (i >= features.size()) break;
        
        const std::string& word = pair.first;
        int docFreq = pair.second;
        
        if (wordCount.find(word) != wordCount.end()) {
            // Term frequency in current document
            float tf = (float)wordCount[word] / tokens.size();
            
            // Inverse document frequency
            float idf = docFreq > 0 ? std::log(m_templates.size() / (float)docFreq) : 0.0f;
            
            // TF-IDF
            features[i] = tf * idf;
        }
        
        i++;
    }
    
    // Add category bias
    ScriptCategory category = DetermineCategory(input);
    int categoryIdx = static_cast<int>(category);
    if (categoryIdx >= 0 && categoryIdx < 7 && m_vocabularySize + categoryIdx < features.size()) {
        features[m_vocabularySize + categoryIdx] = 1.0f;
    }
    
    return features;
}

// Process output
std::string ScriptGenerationModel::ProcessOutput(const std::vector<float>& output) {
    // In a real implementation, this would convert the model output
    // back into a script. For this simplified implementation, we'll
    // return a placeholder.
    return "-- Generated script\nprint('Script generated by model')";
}

// Predict internal
std::string ScriptGenerationModel::PredictInternal(const std::string& input) {
    // Find best template match
    ScriptTemplate bestTemplate = FindBestTemplateMatch(input);
    
    // Generate script
    GeneratedScript script = GenerateScriptFromTemplate(bestTemplate, input);
    
    return script.m_code;
}

// Find best template match
ScriptGenerationModel::ScriptTemplate ScriptGenerationModel::FindBestTemplateMatch(const std::string& description) {
    if (m_templates.empty()) {
        // Return empty template if no templates available
        return ScriptTemplate();
    }
    
    // Featurize input
    std::vector<float> inputFeatures = FeaturizeInput(description);
    
    // Find best match
    std::string bestMatch;
    float bestSimilarity = -1.0f;
    
    for (const auto& pair : m_templates) {
        // Featurize template description
        std::vector<float> templateFeatures = FeaturizeInput(pair.second.m_description);
        
        // Calculate similarity
        float similarity = CalculateSimilarity(inputFeatures, templateFeatures);
        
        // Check if better match
        if (similarity > bestSimilarity) {
            bestSimilarity = similarity;
            bestMatch = pair.first;
        }
    }
    
    // Return best match or first template if no good match
    if (bestSimilarity > 0.3f) {
        return m_templates[bestMatch];
    } else {
        // Return first template
        return m_templates.begin()->second;
    }
}

// Generate script from template
ScriptGenerationModel::GeneratedScript ScriptGenerationModel::GenerateScriptFromTemplate(
    const ScriptTemplate& templ, const std::string& description) {
    
    GeneratedScript script;
    script.m_description = description;
    script.m_category = templ.m_category;
    script.m_tags = templ.m_tags;
    script.m_basedOn = templ.m_name;
    
    // Customize script based on description
    script.m_code = CustomizeScript(templ.m_code, description);
    
    // Set confidence based on similarity
    std::vector<float> descFeatures = FeaturizeInput(description);
    std::vector<float> templFeatures = FeaturizeInput(templ.m_description);
    script.m_confidence = CalculateSimilarity(descFeatures, templFeatures);
    
    return script;
}

// Customize script
std::string ScriptGenerationModel::CustomizeScript(const std::string& templateCode, const std::string& description) {
    // Extract customization parameters from description
    std::vector<std::string> keywords = ExtractKeywords(description);
    
    // Make a copy of the template code
    std::string customized = templateCode;
    
    // Apply customizations based on keywords
    for (const auto& keyword : keywords) {
        if (keyword == "speed" || keyword == "fast") {
            // Adjust speed value
            std::regex speedRegex("speedMultiplier = \\d+");
            customized = std::regex_replace(customized, speedRegex, "speedMultiplier = 5");
        } else if (keyword == "slow") {
            // Adjust speed value
            std::regex speedRegex("speedMultiplier = \\d+");
            customized = std::regex_replace(customized, speedRegex, "speedMultiplier = 2");
        } else if (keyword == "red" || keyword == "green" || keyword == "blue" || keyword == "yellow") {
            // Adjust color value
            std::regex colorRegex("Color3.fromRGB\\(\\d+, \\d+, \\d+\\)");
            
            if (keyword == "red") {
                customized = std::regex_replace(customized, colorRegex, "Color3.fromRGB(255, 0, 0)");
            } else if (keyword == "green") {
                customized = std::regex_replace(customized, colorRegex, "Color3.fromRGB(0, 255, 0)");
            } else if (keyword == "blue") {
                customized = std::regex_replace(customized, colorRegex, "Color3.fromRGB(0, 0, 255)");
            } else if (keyword == "yellow") {
                customized = std::regex_replace(customized, colorRegex, "Color3.fromRGB(255, 255, 0)");
            }
        }
    }
    
    // Add attribution comment
    customized = "-- Script generated for: " + description + "\n" + customized;
    
    return customized;
}

// Calculate similarity
float ScriptGenerationModel::CalculateSimilarity(const std::vector<float>& v1, const std::vector<float>& v2) {
    // Calculate cosine similarity
    float dotProduct = 0.0f;
    float norm1 = 0.0f;
    float norm2 = 0.0f;
    
    size_t minSize = std::min(v1.size(), v2.size());
    
    for (size_t i = 0; i < minSize; ++i) {
        dotProduct += v1[i] * v2[i];
        norm1 += v1[i] * v1[i];
        norm2 += v2[i] * v2[i];
    }
    
    if (norm1 == 0.0f || norm2 == 0.0f) {
        return 0.0f;
    }
    
    return dotProduct / (std::sqrt(norm1) * std::sqrt(norm2));
}

// Tokenize input
std::vector<std::string> ScriptGenerationModel::TokenizeInput(const std::string& input) {
    std::vector<std::string> tokens;
    std::stringstream ss(input);
    std::string token;
    
    // Split by whitespace
    while (ss >> token) {
        // Convert to lowercase
        std::transform(token.begin(), token.end(), token.begin(), 
                      [](unsigned char c) { return std::tolower(c); });
        
        // Remove punctuation
        token.erase(std::remove_if(token.begin(), token.end(), 
                                  [](unsigned char c) { return std::ispunct(c); }),
                   token.end());
        
        // Add token if not empty
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }
    
    return tokens;
}

// Extract keywords
std::vector<std::string> ScriptGenerationModel::ExtractKeywords(const std::string& text) {
    std::vector<std::string> tokens = TokenizeInput(text);
    std::vector<std::string> keywords;
    
    // Filter for keywords
    for (const auto& token : tokens) {
        // Check if token is a keyword
        if (token.length() > 2 && m_wordFrequency.find(token) != m_wordFrequency.end()) {
            keywords.push_back(token);
        }
    }
    
    return keywords;
}

// Determine category
ScriptGenerationModel::ScriptCategory ScriptGenerationModel::DetermineCategory(const std::string& description) {
    // Lowercase description
    std::string lower = description;
    std::transform(lower.begin(), lower.end(), lower.begin(), 
                  [](unsigned char c) { return std::tolower(c); });
    
    // Category keywords
    std::unordered_map<ScriptCategory, std::vector<std::string>> categoryKeywords = {
        {ScriptCategory::Movement, {"speed", "teleport", "fly", "noclip", "walk", "jump", "movement"}},
        {ScriptCategory::Combat, {"aimbot", "aim", "kill", "combat", "fight", "shoot", "weapon"}},
        {ScriptCategory::Visual, {"esp", "wallhack", "chams", "visual", "see", "highlight", "color"}},
        {ScriptCategory::Automation, {"auto", "farm", "collect", "grind", "bot", "automatic"}},
        {ScriptCategory::ServerSide, {"server", "remote", "admin", "kick", "ban", "execute"}},
        {ScriptCategory::Utility, {"utility", "tool", "helper", "feature", "function"}},
        {ScriptCategory::Custom, {"custom", "special", "unique", "specific"}}
    };
    
    // Count keyword matches
    std::unordered_map<ScriptCategory, int> categoryScores;
    
    for (const auto& pair : categoryKeywords) {
        ScriptCategory category = pair.first;
        const std::vector<std::string>& keywords = pair.second;
        
        for (const auto& keyword : keywords) {
            if (lower.find(keyword) != std::string::npos) {
                categoryScores[category]++;
            }
        }
    }
    
    // Find category with highest score
    ScriptCategory bestCategory = ScriptCategory::Utility; // Default
    int bestScore = 0;
    
    for (const auto& pair : categoryScores) {
        if (pair.second > bestScore) {
            bestScore = pair.second;
            bestCategory = pair.first;
        }
    }
    
    return bestCategory;
}

// Generate tags
std::vector<std::string> ScriptGenerationModel::GenerateTags(const std::string& description) {
    std::vector<std::string> tags;
    
    // Extract keywords
    std::vector<std::string> keywords = ExtractKeywords(description);
    
    // Add category as first tag
    ScriptCategory category = DetermineCategory(description);
    tags.push_back(CategoryToString(category));
    
    // Add up to 4 more tags from keywords
    for (const auto& keyword : keywords) {
        if (tags.size() >= 5) break;
        
        // Check if keyword is already a tag
        if (std::find(tags.begin(), tags.end(), keyword) == tags.end()) {
            tags.push_back(keyword);
        }
    }
    
    return tags;
}

// Generate script
ScriptGenerationModel::GeneratedScript ScriptGenerationModel::GenerateScript(const std::string& description, const std::string& context) {
    // Find best template match
    ScriptTemplate bestTemplate = FindBestTemplateMatch(description);
    
    // Generate script from template
    GeneratedScript script = GenerateScriptFromTemplate(bestTemplate, description);
    
    return script;
}

// Add template
bool ScriptGenerationModel::AddTemplate(const ScriptTemplate& templ) {
    // Check if template name is empty
    if (templ.m_name.empty()) {
        return false;
    }
    
    // Add or update template
    m_templates[templ.m_name] = templ;
    
    // Rebuild vocabulary
    BuildVocabulary();
    
    return true;
}

// Get templates
std::unordered_map<std::string, ScriptGenerationModel::ScriptTemplate> ScriptGenerationModel::GetTemplates() const {
    return m_templates;
}

// Get templates by category
std::vector<ScriptGenerationModel::ScriptTemplate> ScriptGenerationModel::GetTemplatesByCategory(ScriptCategory category) {
    std::vector<ScriptTemplate> templates;
    
    for (const auto& pair : m_templates) {
        if (pair.second.m_category == category) {
            templates.push_back(pair.second);
        }
    }
    
    return templates;
}

// Get templates by tag
std::vector<ScriptGenerationModel::ScriptTemplate> ScriptGenerationModel::GetTemplatesByTag(const std::string& tag) {
    std::vector<ScriptTemplate> templates;
    
    for (const auto& pair : m_templates) {
        // Check if template has this tag
        if (std::find(pair.second.m_tags.begin(), pair.second.m_tags.end(), tag) != pair.second.m_tags.end()) {
            templates.push_back(pair.second);
        }
    }
    
    return templates;
}

// Add intent-script pair
bool ScriptGenerationModel::AddIntentScriptPair(const std::string& intent, const std::string& script) {
    // Add to pattern pairs
    m_patternPairs.push_back(std::make_pair(intent, script));
    
    // Rebuild vocabulary
    BuildVocabulary();
    
    return true;
}

// Learn from feedback
bool ScriptGenerationModel::LearnFromFeedback(const std::string& description, 
                                           const std::string& generatedScript,
                                           const std::string& userScript,
                                           float rating) {
    // Check if rating is valid
    if (rating < 0.0f || rating > 1.0f) {
        return false;
    }
    
    // Create training sample
    TrainingSample sample;
    sample.m_input = description;
    sample.m_output = userScript.empty() ? generatedScript : userScript;
    sample.m_features = FeaturizeInput(description);
    sample.m_weight = rating;
    sample.m_timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    
    // Add to training samples
    AddTrainingSample(sample);
    
    // If we have enough new samples, train the model
    if (m_trainingSamples.size() % 10 == 0) {
        Train();
    }
    
    return true;
}

// Get vocabulary size
uint32_t ScriptGenerationModel::GetVocabularySize() const {
    return m_vocabularySize;
}

// Convert category to string
std::string ScriptGenerationModel::CategoryToString(ScriptCategory category) {
    switch (category) {
        case ScriptCategory::Movement:
            return "Movement";
        case ScriptCategory::Combat:
            return "Combat";
        case ScriptCategory::Visual:
            return "Visual";
        case ScriptCategory::Automation:
            return "Automation";
        case ScriptCategory::ServerSide:
            return "ServerSide";
        case ScriptCategory::Utility:
            return "Utility";
        case ScriptCategory::Custom:
            return "Custom";
        default:
            return "Unknown";
    }
}

// Convert string to category
ScriptGenerationModel::ScriptCategory ScriptGenerationModel::StringToCategory(const std::string& str) {
    if (str == "Movement") {
        return ScriptCategory::Movement;
    } else if (str == "Combat") {
        return ScriptCategory::Combat;
    } else if (str == "Visual") {
        return ScriptCategory::Visual;
    } else if (str == "Automation") {
        return ScriptCategory::Automation;
    } else if (str == "ServerSide") {
        return ScriptCategory::ServerSide;
    } else if (str == "Utility") {
        return ScriptCategory::Utility;
    } else if (str == "Custom") {
        return ScriptCategory::Custom;
    } else {
        return ScriptCategory::Utility; // Default
    }
}

} // namespace LocalModels
} // namespace AIFeatures
} // namespace iOS
