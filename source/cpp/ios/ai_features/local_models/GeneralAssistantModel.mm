#include "GeneralAssistantModel.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <algorithm>
#include <chrono>
#include <regex>
#include <random>

namespace iOS {
namespace AIFeatures {
namespace LocalModels {

// Constructor
GeneralAssistantModel::GeneralAssistantModel() 
    : m_isInitialized(false),
      m_internalModel(nullptr) {
    
    // Initialize default current profile
    m_currentProfile.m_proficiency = UserProficiency::Beginner;
    m_currentProfile.m_lastActive = static_cast<uint64_t>(time(nullptr));
    m_currentProfile.m_interactionCount = 0;
    
    // Add default interests and preferences
    m_currentProfile.m_interests = {"general", "scripting", "automation"};
    m_currentProfile.m_preferences["detailed_explanations"] = 0.7f;
    m_currentProfile.m_preferences["code_examples"] = 0.8f;
    m_currentProfile.m_preferences["security_focus"] = 0.5f;
    
    std::cout << "GeneralAssistantModel: Created new instance" << std::endl;
}

// Destructor
GeneralAssistantModel::~GeneralAssistantModel() {
    // Clean up internal model resources
    if (m_internalModel) {
        // Release model resources
        ReleaseUnusedResources();
        delete[] static_cast<char*>(m_internalModel);
        m_internalModel = nullptr;
    }
    
    // Save user profiles before destroying
    if (m_isInitialized) {
        SaveUserProfiles();
    }
    
    std::cout << "GeneralAssistantModel: Instance destroyed" << std::endl;
}

// Initialize the model
bool GeneralAssistantModel::Initialize(const std::string& modelPath) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    std::cout << "GeneralAssistantModel: Initializing with path: " << modelPath << std::endl;
    
    if (modelPath.empty()) {
        std::cerr << "GeneralAssistantModel: Empty model path provided" << std::endl;
        return false;
    }
    
    m_storagePath = modelPath;
    
    // Create directory if it doesn't exist
    std::string profilesDir = m_storagePath + "/profiles";
    std::string command = "mkdir -p \"" + profilesDir + "\"";
    int result = system(command.c_str());
    if (result != 0) {
        std::cerr << "GeneralAssistantModel: Failed to create profiles directory: " << profilesDir << std::endl;
        // Non-critical error, continue
    }
    
    // Load user profiles
    LoadUserProfiles();
    
    // In a production implementation, we would load ML models from the path
    // For now, we'll create a model with context-based capabilities
    m_internalModel = new char[4096]; // Placeholder for model, sized to store context data
    
    // Add basic system context
    std::string baseSystemPrompt = 
        "You are an AI assistant integrated into a Roblox Executor application. "
        "You help users with scripting, game exploitation, and using the executor's features. "
        "Your responses should be accurate, helpful, and tailored to the user's expertise level. "
        "For beginners, provide detailed explanations. For experts, be concise and technical.";
    
    // Store in model memory (in production this would be loaded into an actual ML model)
    std::memcpy(m_internalModel, baseSystemPrompt.c_str(), std::min(baseSystemPrompt.size(), static_cast<size_t>(4096)));
    
    // Add initial system messages to interaction history
    uint64_t timestamp = static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count()
    );
    
    Interaction systemInteraction(baseSystemPrompt, MessageType::System, timestamp);
    m_interactionHistory.push_back(systemInteraction);
    
    m_isInitialized = true;
    std::cout << "GeneralAssistantModel: Initialization complete" << std::endl;
    return true;
}

// Process user input
std::string GeneralAssistantModel::ProcessInput(const std::string& input, const std::string& userId) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (!m_isInitialized) {
        return "Sorry, the assistant model is not initialized yet. Please try again later.";
    }
    
    if (input.empty()) {
        return "I didn't receive any input. How can I help you with the executor today?";
    }
    
    // Set or update user if provided
    if (!userId.empty()) {
        SetCurrentUser(userId);
    }
    
    std::cout << "GeneralAssistantModel: Processing input: " << input << std::endl;
    
    // Add user message to history
    uint64_t timestamp = static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count()
    );
    
    Interaction userInteraction(input, MessageType::User, timestamp);
    m_interactionHistory.push_back(userInteraction);
    
    // Update user profile based on interaction
    UpdateUserProfile(userInteraction);
    
    // Prepare generation context
    GenerationContext context;
    context.proficiency = m_currentProfile.m_proficiency;
    context.interests = m_currentProfile.m_interests;
    context.preferences = m_currentProfile.m_preferences;
    context.recentInteractions = GetRelevantInteractionHistory();
    
    // Generate response
    std::string response = GenerateContextAwareResponse(input, context);
    
    // Add assistant response to history
    timestamp = static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count()
    );
    
    Interaction assistantInteraction(response, MessageType::Assistant, timestamp);
    m_interactionHistory.push_back(assistantInteraction);
    
    // Increment interaction count
    m_currentProfile.m_interactionCount++;
    m_currentProfile.m_lastActive = static_cast<uint64_t>(time(nullptr));
    
    // Save profiles periodically (every 5 interactions)
    if (m_currentProfile.m_interactionCount % 5 == 0) {
        SaveUserProfiles();
    }
    
    return response;
}

// Process user input with context
std::string GeneralAssistantModel::ProcessInputWithContext(const std::string& input, const std::string& systemContext, const std::string& userId) {
    // Add system context before processing
    if (!systemContext.empty()) {
        AddSystemMessage(systemContext);
    }
    
    // Now process the input normally
    return ProcessInput(input, userId);
}

// Set current user
bool GeneralAssistantModel::SetCurrentUser(const std::string& userId) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (userId.empty()) {
        std::cerr << "GeneralAssistantModel: Empty user ID provided" << std::endl;
        return false;
    }
    
    // Save current user profile if different from new user
    if (!m_currentProfile.m_userId.empty() && m_currentProfile.m_userId != userId) {
        m_userProfiles[m_currentProfile.m_userId] = m_currentProfile;
    }
    
    // Check if we have a profile for this user
    auto it = m_userProfiles.find(userId);
    if (it != m_userProfiles.end()) {
        // Load existing profile
        m_currentProfile = it->second;
        
        // Update last active timestamp
        m_currentProfile.m_lastActive = static_cast<uint64_t>(time(nullptr));
        
        std::cout << "GeneralAssistantModel: Loaded profile for user: " << userId << std::endl;
    } else {
        // Create new profile
        m_currentProfile = UserProfile();
        m_currentProfile.m_userId = userId;
        m_currentProfile.m_proficiency = UserProficiency::Beginner;
        m_currentProfile.m_lastActive = static_cast<uint64_t>(time(nullptr));
        m_currentProfile.m_interactionCount = 0;
        
        // Add default interests and preferences
        m_currentProfile.m_interests = {"general", "scripting", "automation"};
        m_currentProfile.m_preferences["detailed_explanations"] = 0.7f;
        m_currentProfile.m_preferences["code_examples"] = 0.8f;
        m_currentProfile.m_preferences["security_focus"] = 0.5f;
        
        std::cout << "GeneralAssistantModel: Created new profile for user: " << userId << std::endl;
    }
    
    // Adapt model to current user profile
    AdaptModelToUser(m_currentProfile);
    
    return true;
}

// Add system message
void GeneralAssistantModel::AddSystemMessage(const std::string& message) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (message.empty()) {
        return;
    }
    
    uint64_t timestamp = static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count()
    );
    
    Interaction systemInteraction(message, MessageType::System, timestamp);
    m_interactionHistory.push_back(systemInteraction);
    
    std::cout << "GeneralAssistantModel: Added system message: " << message << std::endl;
}

// Add tool output
void GeneralAssistantModel::AddToolOutput(const std::string& toolName, const std::string& output) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (toolName.empty() || output.empty()) {
        return;
    }
    
    std::string formattedOutput = "Tool: " + toolName + "\n" + output;
    uint64_t timestamp = static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count()
    );
    
    Interaction toolInteraction(formattedOutput, MessageType::Tool, timestamp);
    m_interactionHistory.push_back(toolInteraction);
    
    std::cout << "GeneralAssistantModel: Added tool output from: " << toolName << std::endl;
}

// Get user proficiency
GeneralAssistantModel::UserProficiency GeneralAssistantModel::GetUserProficiency() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_currentProfile.m_proficiency;
}

// Check if model is initialized
bool GeneralAssistantModel::IsInitialized() const {
    return m_isInitialized;
}

// Set model path
bool GeneralAssistantModel::SetModelPath(const std::string& path) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (path.empty()) {
        std::cerr << "GeneralAssistantModel: Empty path provided" << std::endl;
        return false;
    }
    
    m_storagePath = path;
    return true;
}

// Reset conversation history
void GeneralAssistantModel::ResetConversation() {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // Clear interaction history except for system messages
    std::vector<Interaction> systemMessages;
    for (const auto& interaction : m_interactionHistory) {
        if (interaction.m_type == MessageType::System) {
            systemMessages.push_back(interaction);
        }
    }
    
    m_interactionHistory = systemMessages;
    std::cout << "GeneralAssistantModel: Conversation reset, kept " << systemMessages.size() << " system messages" << std::endl;
}

// Get model version
std::string GeneralAssistantModel::GetVersion() const {
    return "1.0.0";
}

// Get memory usage
uint64_t GeneralAssistantModel::GetMemoryUsage() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // Calculate approximate memory usage
    size_t usage = 0;
    
    // Interaction history
    for (const auto& interaction : m_interactionHistory) {
        usage += interaction.m_content.size() + sizeof(Interaction);
    }
    
    // User profiles
    for (const auto& profile : m_userProfiles) {
        usage += profile.first.size() + sizeof(UserProfile);
        for (const auto& interest : profile.second.m_interests) {
            usage += interest.size();
        }
        usage += profile.second.m_preferences.size() * (sizeof(std::string) + sizeof(float));
    }
    
    // Current profile
    usage += sizeof(UserProfile);
    for (const auto& interest : m_currentProfile.m_interests) {
        usage += interest.size();
    }
    usage += m_currentProfile.m_preferences.size() * (sizeof(std::string) + sizeof(float));
    
    // Internal model
    if (m_internalModel) {
        usage += 4096; // Size allocated for model
    }
    
    return usage;
}

// Release unused resources
void GeneralAssistantModel::ReleaseUnusedResources() {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // Trim interaction history if it's too long
    if (m_interactionHistory.size() > 100) {
        // Keep all system messages and the last 50 non-system interactions
        std::vector<Interaction> keptInteractions;
        
        // First, keep all system messages
        for (const auto& interaction : m_interactionHistory) {
            if (interaction.m_type == MessageType::System) {
                keptInteractions.push_back(interaction);
            }
        }
        
        // Then add the most recent non-system interactions
        std::vector<Interaction> recentInteractions;
        for (const auto& interaction : m_interactionHistory) {
            if (interaction.m_type != MessageType::System) {
                recentInteractions.push_back(interaction);
            }
        }
        
        // Sort by timestamp (most recent first)
        std::sort(recentInteractions.begin(), recentInteractions.end(), 
                 [](const Interaction& a, const Interaction& b) {
                     return a.m_timestamp > b.m_timestamp;
                 });
        
        // Keep only the 50 most recent
        size_t numToKeep = std::min(recentInteractions.size(), static_cast<size_t>(50));
        for (size_t i = 0; i < numToKeep; i++) {
            keptInteractions.push_back(recentInteractions[i]);
        }
        
        // Sort final collection by timestamp (oldest first)
        std::sort(keptInteractions.begin(), keptInteractions.end(), 
                 [](const Interaction& a, const Interaction& b) {
                     return a.m_timestamp < b.m_timestamp;
                 });
        
        m_interactionHistory = keptInteractions;
    }
    
    // Also prune old/inactive user profiles
    std::vector<std::string> profilesToRemove;
    uint64_t currentTime = static_cast<uint64_t>(time(nullptr));
    uint64_t oneMonthInSeconds = 30 * 24 * 60 * 60; // 30 days
    
    for (const auto& profile : m_userProfiles) {
        // Remove profiles not accessed in a month
        if (currentTime - profile.second.m_lastActive > oneMonthInSeconds) {
            profilesToRemove.push_back(profile.first);
        }
    }
    
    for (const auto& userId : profilesToRemove) {
        m_userProfiles.erase(userId);
    }
    
    std::cout << "GeneralAssistantModel: Released unused resources, removed " 
              << profilesToRemove.size() << " inactive profiles" << std::endl;
}

// Add model awareness
void GeneralAssistantModel::AddModelAwareness(const std::string& modelName, 
                                            const std::string& modelDescription, 
                                            const std::vector<std::string>& modelCapabilities) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    std::stringstream ss;
    ss << "System is aware of model: " << modelName << "\n";
    ss << "Description: " << modelDescription << "\n";
    ss << "Capabilities:";
    
    for (const auto& capability : modelCapabilities) {
        ss << "\n- " << capability;
    }
    
    AddSystemMessage(ss.str());
    std::cout << "GeneralAssistantModel: Added awareness of model: " << modelName << std::endl;
}

// Notify of feature usage
void GeneralAssistantModel::NotifyFeatureUsage(const std::string& featureName, const std::string& context) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (featureName.empty()) {
        return;
    }
    
    std::stringstream ss;
    ss << "User used feature: " << featureName;
    if (!context.empty()) {
        ss << " in context: " << context;
    }
    
    // Add to preferences if not exists
    if (m_currentProfile.m_preferences.find(featureName) == m_currentProfile.m_preferences.end()) {
        m_currentProfile.m_preferences[featureName] = 0.5f;
    }
    
    // Increase preference weight
    m_currentProfile.m_preferences[featureName] = std::min(1.0f, m_currentProfile.m_preferences[featureName] + 0.1f);
    
    // Add as tool output for context
    AddToolOutput("FeatureTracker", ss.str());
    
    std::cout << "GeneralAssistantModel: Notified of feature usage: " << featureName << std::endl;
}

// Train the model
bool GeneralAssistantModel::Train() {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (!m_isInitialized) {
        std::cerr << "GeneralAssistantModel: Cannot train - model not initialized" << std::endl;
        return false;
    }
    
    std::cout << "GeneralAssistantModel: Starting training..." << std::endl;
    
    // Build a topic frequency map from user interactions
    std::unordered_map<std::string, int> topicFrequency;
    
    for (const auto& interaction : m_interactionHistory) {
        if (interaction.m_type == MessageType::User) {
            // Extract topics from user messages
            std::vector<std::string> topics = FindRelevantTopics(interaction.m_content);
            for (const auto& topic : topics) {
                topicFrequency[topic]++;
            }
        }
    }
    
    // Update user interests based on topic frequency
    if (!topicFrequency.empty()) {
        // Sort topics by frequency
        std::vector<std::pair<std::string, int>> sortedTopics(
            topicFrequency.begin(), topicFrequency.end()
        );
        
        std::sort(sortedTopics.begin(), sortedTopics.end(),
                 [](const auto& a, const auto& b) {
                     return a.second > b.second;
                 });
        
        // Update user interests with the top topics
        std::vector<std::string> newInterests;
        size_t maxInterests = 5; // Keep top 5 interests
        
        for (size_t i = 0; i < std::min(sortedTopics.size(), maxInterests); i++) {
            newInterests.push_back(sortedTopics[i].first);
        }
        
        if (!newInterests.empty()) {
            m_currentProfile.m_interests = newInterests;
        }
    }
    
    // Save user profiles after training
    SaveUserProfiles();
    
    std::cout << "GeneralAssistantModel: Training complete" << std::endl;
    return true;
}

// Set user interests
void GeneralAssistantModel::SetUserInterests(const std::vector<std::string>& interests) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_currentProfile.m_interests = interests;
}

// Get user interests
std::vector<std::string> GeneralAssistantModel::GetUserInterests() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_currentProfile.m_interests;
}

// Set user preference
void GeneralAssistantModel::SetUserPreference(const std::string& preference, float value) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (preference.empty()) {
        return;
    }
    
    // Clamp value to 0.0-1.0 range
    value = std::max(0.0f, std::min(1.0f, value));
    m_currentProfile.m_preferences[preference] = value;
}

// Get user preference
float GeneralAssistantModel::GetUserPreference(const std::string& preference, float defaultValue) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (preference.empty()) {
        return defaultValue;
    }
    
    auto it = m_currentProfile.m_preferences.find(preference);
    if (it != m_currentProfile.m_preferences.end()) {
        return it->second;
    }
    
    return defaultValue;
}

// Private: Update user profile
void GeneralAssistantModel::UpdateUserProfile(const Interaction& interaction) {
    // Update proficiency based on interaction count
    if (m_currentProfile.m_interactionCount > 10 && m_currentProfile.m_proficiency == UserProficiency::Beginner) {
        m_currentProfile.m_proficiency = UserProficiency::Intermediate;
    } else if (m_currentProfile.m_interactionCount > 30 && m_currentProfile.m_proficiency == UserProficiency::Intermediate) {
        m_currentProfile.m_proficiency = UserProficiency::Advanced;
    } else if (m_currentProfile.m_interactionCount > 100 && m_currentProfile.m_proficiency == UserProficiency::Advanced) {
        m_currentProfile.m_proficiency = UserProficiency::Expert;
    }
    
    // Update interests based on interaction content
    std::vector<std::string> topics = FindRelevantTopics(interaction.m_content);
    
    // Add new topics to interests if not already present
    for (const auto& topic : topics) {
        if (std::find(m_currentProfile.m_interests.begin(), 
                      m_currentProfile.m_interests.end(), 
                      topic) == m_currentProfile.m_interests.end()) {
            // Limit to max 10 interests
            if (m_currentProfile.m_interests.size() < 10) {
                m_currentProfile.m_interests.push_back(topic);
            }
        }
    }
}

// Private: Save user profiles
void GeneralAssistantModel::SaveUserProfiles() {
    if (m_storagePath.empty()) {
        std::cerr << "GeneralAssistantModel: Cannot save - storage path not set" << std::endl;
        return;
    }
    
    // Save current profile if it has a user ID
    if (!m_currentProfile.m_userId.empty()) {
        m_userProfiles[m_currentProfile.m_userId] = m_currentProfile;
    }
    
    // Create the profiles directory
    std::string profilesDir = m_storagePath + "/profiles";
    std::string command = "mkdir -p \"" + profilesDir + "\"";
    int result = system(command.c_str());
    if (result != 0) {
        std::cerr << "GeneralAssistantModel: Failed to create profiles directory: " << profilesDir << std::endl;
        return;
    }
    
    // Save each profile to a separate file
    for (const auto& profile : m_userProfiles) {
        std::string filePath = profilesDir + "/" + profile.first + ".profile";
        std::ofstream file(filePath);
        
        if (!file.is_open()) {
            std::cerr << "GeneralAssistantModel: Failed to save profile for user: " << profile.first << std::endl;
            continue;
        }
        
        // Write user ID
        file << "user_id:" << profile.second.m_userId << std::endl;
        
        // Write proficiency
        file << "proficiency:" << static_cast<int>(profile.second.m_proficiency) << std::endl;
        
        // Write last active timestamp
        file << "last_active:" << profile.second.m_lastActive << std::endl;
        
        // Write interaction count
        file << "interaction_count:" << profile.second.m_interactionCount << std::endl;
        
        // Write interests
        file << "interests:";
        for (const auto& interest : profile.second.m_interests) {
            file << interest << ";";
        }
        file << std::endl;
        
        // Write preferences
        file << "preferences:";
        for (const auto& pref : profile.second.m_preferences) {
            file << pref.first << "=" << pref.second << ";";
        }
        file << std::endl;
        
        file.close();
    }
    
    std::cout << "GeneralAssistantModel: Saved " << m_userProfiles.size() << " user profiles" << std::endl;
}

// Private: Load user profiles
void GeneralAssistantModel::LoadUserProfiles() {
    if (m_storagePath.empty()) {
        std::cerr << "GeneralAssistantModel: Cannot load - storage path not set" << std::endl;
        return;
    }
    
    // Clear existing profiles
    m_userProfiles.clear();
    
    // Check if profiles directory exists
    std::string profilesDir = m_storagePath + "/profiles";
    std::string command = "ls \"" + profilesDir + "\" 2>/dev/null | grep -i \".profile$\"";
    
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) {
        std::cerr << "GeneralAssistantModel: Failed to list profiles directory" << std::endl;
        return;
    }
    
    char buffer[256];
    std::vector<std::string> profileFiles;
    
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        std::string filename(buffer);
        
        // Remove trailing newline
        if (!filename.empty() && filename[filename.length() - 1] == '\n') {
            filename.erase(filename.length() - 1);
        }
        
        profileFiles.push_back(filename);
    }
    
    pclose(pipe);
    
    // Load each profile
    for (const auto& filename : profileFiles) {
        std::string filePath = profilesDir + "/" + filename;
        std::ifstream file(filePath);
        
        if (!file.is_open()) {
            std::cerr << "GeneralAssistantModel: Failed to open profile file: " << filePath << std::endl;
            continue;
        }
        
        UserProfile profile;
        std::string line;
        
        while (std::getline(file, line)) {
            // Parse each line based on its prefix
            if (line.substr(0, 8) == "user_id:") {
                profile.m_userId = line.substr(8);
            } else if (line.substr(0, 12) == "proficiency:") {
                int prof = std::stoi(line.substr(12));
                profile.m_proficiency = static_cast<UserProficiency>(prof);
            } else if (line.substr(0, 12) == "last_active:") {
                profile.m_lastActive = std::stoull(line.substr(12));
            } else if (line.substr(0, 18) == "interaction_count:") {
                profile.m_interactionCount = std::stoi(line.substr(18));
            } else if (line.substr(0, 10) == "interests:") {
                std::string interestsStr = line.substr(10);
                std::vector<std::string> interests;
                
                size_t pos = 0;
                std::string token;
                while ((pos = interestsStr.find(";")) != std::string::npos) {
                    token = interestsStr.substr(0, pos);
                    if (!token.empty()) {
                        interests.push_back(token);
                    }
                    interestsStr.erase(0, pos + 1);
                }
                
                profile.m_interests = interests;
            } else if (line.substr(0, 12) == "preferences:") {
                std::string prefsStr = line.substr(12);
                std::unordered_map<std::string, float> preferences;
                
                size_t pos = 0;
                std::string token;
                while ((pos = prefsStr.find(";")) != std::string::npos) {
                    token = prefsStr.substr(0, pos);
                    
                    size_t equalPos = token.find("=");
                    if (equalPos != std::string::npos) {
                        std::string key = token.substr(0, equalPos);
                        float value = std::stof(token.substr(equalPos + 1));
                        preferences[key] = value;
                    }
                    
                    prefsStr.erase(0, pos + 1);
                }
                
                profile.m_preferences = preferences;
            }
        }
        
        file.close();
        
        // Add profile to map
        if (!profile.m_userId.empty()) {
            m_userProfiles[profile.m_userId] = profile;
        }
    }
    
    std::cout << "GeneralAssistantModel: Loaded " << m_userProfiles.size() << " user profiles" << std::endl;
}

// Private: Adapt model to user
void GeneralAssistantModel::AdaptModelToUser(const UserProfile& profile) {
    // In a production implementation, this would adjust internal model parameters
    // For now, we'll just log the adaptation
    std::cout << "GeneralAssistantModel: Adapted to user proficiency: ";
    
    switch (profile.m_proficiency) {
        case UserProficiency::Beginner:
            std::cout << "Beginner";
            break;
        case UserProficiency::Intermediate:
            std::cout << "Intermediate";
            break;
        case UserProficiency::Advanced:
            std::cout << "Advanced";
            break;
        case UserProficiency::Expert:
            std::cout << "Expert";
            break;
    }
    
    std::cout << std::endl;
}

// Private: Generate context-aware response
std::string GeneralAssistantModel::GenerateContextAwareResponse(const std::string& input, const GenerationContext& context) {
    // In a production implementation, this would use an ML model to generate a response
    // For now, we'll generate rule-based responses with context awareness
    
    if (input.empty()) {
        return "I didn't receive any input. How can I help you with the executor today?";
    }
    
    // Detect intent from input
    std::string intent = DetectIntent(input);
    
    // Get response for this intent, considering context
    std::string response = GetResponseForIntent(intent, context);
    
    return response;
}

// Private: Get relevant interaction history
std::vector<GeneralAssistantModel::Interaction> GeneralAssistantModel::GetRelevantInteractionHistory(size_t maxItems) const {
    // Get the most recent interactions, prioritizing system context
    std::vector<Interaction> relevantHistory;
    
    // First add system messages
    for (const auto& interaction : m_interactionHistory) {
        if (interaction.m_type == MessageType::System) {
            relevantHistory.push_back(interaction);
            
            // Limit to avoid overloading with too much system context
            if (relevantHistory.size() >= 5) {
                break;
            }
        }
    }
    
    // Then add the most recent user-assistant exchanges
    std::vector<Interaction> recentExchanges;
    
    for (auto it = m_interactionHistory.rbegin(); it != m_interactionHistory.rend(); ++it) {
        if (it->m_type == MessageType::User || it->m_type == MessageType::Assistant) {
            recentExchanges.push_back(*it);
            
            if (recentExchanges.size() >= maxItems) {
                break;
            }
        }
    }
    
    // Reverse the recent exchanges to get them in chronological order
    std::reverse(recentExchanges.begin(), recentExchanges.end());
    
    // Add them to the relevant history
    relevantHistory.insert(relevantHistory.end(), recentExchanges.begin(), recentExchanges.end());
    
    return relevantHistory;
}

// Private: Detect intent from input
std::string GeneralAssistantModel::DetectIntent(const std::string& input) const {
    // Convert input to lowercase for case-insensitive matching
    std::string lowercaseInput = input;
    std::transform(lowercaseInput.begin(), lowercaseInput.end(), lowercaseInput.begin(), ::tolower);
    
    // Check for greeting intent
    if (lowercaseInput.find("hello") != std::string::npos || 
        lowercaseInput.find("hi ") != std::string::npos || 
        lowercaseInput.find("hey ") != std::string::npos ||
        lowercaseInput == "hi" || lowercaseInput == "hey") {
        return "greeting";
    }
    
    // Check for help intent
    if (lowercaseInput.find("help") != std::string::npos || 
        lowercaseInput.find("assist") != std::string::npos || 
        lowercaseInput.find("how to") != std::string::npos ||
        lowercaseInput.find("how do i") != std::string::npos ||
        lowercaseInput.find("what is") != std::string::npos ||
        lowercaseInput.find("?") != std::string::npos) {
        
        // Check for specific help topics
        if (lowercaseInput.find("script") != std::string::npos) {
            if (lowercaseInput.find("create") != std::string::npos || 
                lowercaseInput.find("write") != std::string::npos || 
                lowercaseInput.find("make") != std::string::npos) {
                return "help_script_creation";
            } else if (lowercaseInput.find("debug") != std::string::npos || 
                       lowercaseInput.find("fix") != std::string::npos || 
                       lowercaseInput.find("error") != std::string::npos) {
                return "help_script_debugging";
            } else {
                return "help_scripts";
            }
        } else if (lowercaseInput.find("game") != std::string::npos || 
                   lowercaseInput.find("detect") != std::string::npos) {
            return "help_game_detection";
        } else if (lowercaseInput.find("vulnerability") != std::string::npos || 
                   lowercaseInput.find("exploit") != std::string::npos || 
                   lowercaseInput.find("security") != std::string::npos) {
            return "help_vulnerabilities";
        } else if (lowercaseInput.find("executor") != std::string::npos || 
                   lowercaseInput.find("feature") != std::string::npos || 
                   lowercaseInput.find("function") != std::string::npos) {
            return "help_executor";
        } else {
            return "help_general";
        }
    }
    
    // Check for script-related intent
    if (lowercaseInput.find("script") != std::string::npos || 
        lowercaseInput.find("code") != std::string::npos || 
        lowercaseInput.find("lua") != std::string::npos) {
        return "script";
    }
    
    // Check for game-related intent
    if (lowercaseInput.find("game") != std::string::npos || 
        lowercaseInput.find("roblox") != std::string::npos || 
        lowercaseInput.find("detect") != std::string::npos) {
        return "game";
    }
    
    // Check for security-related intent
    if (lowercaseInput.find("security") != std::string::npos || 
        lowercaseInput.find("vulnerability") != std::string::npos || 
        lowercaseInput.find("exploit") != std::string::npos || 
        lowercaseInput.find("hack") != std::string::npos) {
        return "security";
    }
    
    // Fallback intent
    return "general";
}

// Private: Get response for intent
std::string GeneralAssistantModel::GetResponseForIntent(const std::string& intent, const GenerationContext& context) const {
    // Tailor response based on user proficiency
    bool isDetailed = (context.proficiency == UserProficiency::Beginner || 
                        context.proficiency == UserProficiency::Intermediate);
    
    // Get response based on intent
    if (intent == "greeting") {
        switch (context.proficiency) {
            case UserProficiency::Beginner:
                return "Hello! I'm your Executor Assistant. I notice you're new to the executor. Would you like me to give you a quick tour?";
            case UserProficiency::Intermediate:
                return "Hi there! Welcome back! Is there a particular script or feature you want to work with today?";
            case UserProficiency::Advanced:
                return "Hello! Good to see you again. Let me know if you need help with any advanced executor features.";
            case UserProficiency::Expert:
                return "Hey there! Ready to push the executor to its limits again? What advanced features are you working with today?";
            default:
                return "Hello! How can I help you with the executor today?";
        }
    } else if (intent == "help_general") {
        std::string response = "I'm here to help with all aspects of the executor. Here are some things I can assist with:";
        response += "\n\n- Creating and debugging scripts";
        response += "\n- Finding game vulnerabilities";
        response += "\n- Bypassing game security measures";
        response += "\n- Optimizing your scripts for performance";
        response += "\n- Explaining how different executor features work";
        
        if (isDetailed) {
            response += "\n\nJust let me know what specific area you need help with, and I can provide more detailed guidance.";
        }
        
        return response;
    } else if (intent == "help_script_creation") {
        if (isDetailed) {
            std::string response = "To create a new script, you can use the built-in script editor or ask the script generation model for help. Here's how:";
            response += "\n\n1. Open the Script Editor by clicking on the 'Editor' tab";
            response += "\n2. Start writing your Lua code or use the 'Generate Script' feature";
            response += "\n3. For simple scripts, enter a description of what you want the script to do";
            response += "\n4. Click 'Generate' and the AI will create a script based on your description";
            response += "\n5. You can modify the generated script as needed";
            response += "\n6. Test your script by clicking 'Execute'";
            response += "\n\nWould you like me to show you a basic example script to get started?";
            return response;
        } else {
            return "For script creation, you can use the Script Editor or the script generation AI. Just describe what you want the script to do, and the system can create a starting point for you. Need any specific script functionality?";
        }
    } else if (intent == "help_script_debugging") {
        if (isDetailed) {
            std::string response = "When debugging scripts, the executor provides several tools to help identify and fix issues:";
            response += "\n\n1. Error Console: Shows syntax errors and runtime exceptions";
            response += "\n2. Vulnerability Scanner: Checks your script for common mistakes and security issues";
            response += "\n3. Variable Inspector: Shows the current value of variables during execution";
            response += "\n4. Breakpoints: Allow you to pause execution at specific lines";
            response += "\n\nWhat kind of error are you encountering? I can help provide more specific debugging advice.";
            return response;
        } else {
            return "For debugging, check the Error Console for details on the issue. The Vulnerability Scanner can also help identify problems. If you need advanced debugging, use the breakpoints feature to step through execution. What specific error are you seeing?";
        }
    } else if (intent == "help_scripts") {
        std::string response = "The script management system allows you to organize, edit, and execute your scripts. ";
        
        if (isDetailed) {
            response += "Here's what you can do:\n\n";
            response += "- Create new scripts in the Editor tab\n";
            response += "- Save scripts to your library for future use\n";
            response += "- Organize scripts by category (Movement, Combat, Visual, etc.)\n";
            response += "- Import scripts from external sources\n";
            response += "- Execute scripts in different security contexts\n";
            response += "- Use the AI to generate or modify scripts\n\n";
            response += "Would you like to know more about any specific aspect of script management?";
        } else {
            response += "You can create, save, organize, and execute scripts from the management interface. The system includes AI assistance, import/export capabilities, and different execution modes for security. Need help with something specific?";
        }
        
        return response;
    } else if (intent == "help_game_detection") {
        std::string response = "The executor includes a game detection system that identifies what game you're playing. ";
        
        if (isDetailed) {
            response += "This works by scanning memory patterns and UI elements to reliably identify the current game. When a game is detected:\n\n";
            response += "- The executor automatically loads compatible scripts\n";
            response += "- Security settings are adjusted for that specific game\n";
            response += "- Vulnerability detection is tuned to the game's environment\n";
            response += "- Script suggestions are customized for the detected game\n\n";
            response += "Would you like me to explain how to use game-specific features?";
        } else {
            response += "Once a game is detected, the executor adjusts its behavior and suggests compatible scripts. You can also specify custom behaviors for specific games in the settings. Is there a particular game you're working with?";
        }
        
        return response;
    } else if (intent == "help_vulnerabilities") {
        std::string response = "The vulnerability detection system helps you identify security weaknesses in games. ";
        
        if (isDetailed) {
            response += "Here's how it works:\n\n";
            response += "1. The scanner analyzes the game's code and memory\n";
            response += "2. It identifies potential vulnerabilities like:\n";
            response += "   - Unsecured remote events\n";
            response += "   - Server-client trust issues\n";
            response += "   - Improperly filtered user input\n";
            response += "   - Exposed administrative functions\n";
            response += "3. Results are displayed with severity ratings\n";
            response += "4. For each vulnerability, the system suggests exploitation methods\n\n";
            response += "Would you like to run a vulnerability scan on your current game?";
        } else {
            response += "It scans for weaknesses like unsecured remote events, trust issues, and exposed admin functions. Each finding includes a severity rating and exploitation suggestion. Need help with a specific vulnerability type?";
        }
        
        return response;
    } else if (intent == "help_executor") {
        std::string response = "The executor provides a comprehensive environment for interacting with Roblox games. ";
        
        if (isDetailed) {
            response += "Key features include:\n\n";
            response += "- Script execution with multiple security levels\n";
            response += "- Built-in script editor with syntax highlighting\n";
            response += "- AI-powered script generation and assistance\n";
            response += "- Game vulnerability detection\n";
            response += "- Script library management\n";
            response += "- Game detection for automatic script loading\n";
            response += "- Floating UI for easy access\n\n";
            response += "Which feature would you like to learn more about?";
        } else {
            response += "It integrates script execution, editing, vulnerability detection, and AI assistance in one package. You can access everything from the main interface or the floating button. Any specific functionality you need help with?";
        }
        
        return response;
    } else if (intent == "script") {
        if (isDetailed) {
            std::string response = "For scripting, the executor supports the full Luau language with Roblox API compatibility. ";
            response += "You can create scripts for:\n\n";
            response += "- Player movement modifications (speed, jump height, noclip, etc.)\n";
            response += "- Visual enhancements (ESP, wallhack, custom rendering)\n";
            response += "- Combat assistance (aimbot, hitbox expansion)\n";
            response += "- Automation (farming, repetitive tasks)\n";
            response += "- UI enhancements (custom menus, information displays)\n\n";
            response += "What type of script are you looking to create?";
            return response;
        } else {
            return "The executor supports full Luau with Roblox API compatibility. This includes player mods, visual enhancements, combat scripts, automation, and UI customization. What specific scripting functionality do you need?";
        }
    } else if (intent == "game") {
        if (context.proficiency == UserProficiency::Beginner) {
            return "When you join a game, the executor will automatically detect it and suggest compatible scripts. You can also set up custom behaviors for specific games. Would you like to see what scripts are available for your current game?";
        } else {
            return "The game detection system is currently active and will identify games automatically. You can view detected games in the status panel and access game-specific scripts from the library. Any specific game integration you need help with?";
        }
    } else if (intent == "security") {
        if (isDetailed) {
            std::string response = "The executor includes several security features:\n\n";
            response += "- Anti-detection measures to avoid game security systems\n";
            response += "- Signature adaptation that evolves to bypass Byfron\n";
            response += "- Sandboxed execution environments\n";
            response += "- Vulnerability scanning for finding exploitable weaknesses\n";
            response += "- Memory pattern scanning for identifying targets\n\n";
            response += "Which security aspect are you interested in?";
            return response;
        } else {
            return "The security system includes anti-detection, signature adaptation for Byfron, sandboxed execution, and vulnerability scanning. The system continuously adapts to game security updates. Need any specific security bypass assistance?";
        }
    } else {
        // General response
        if (context.proficiency == UserProficiency::Beginner) {
            return "I'm here to help you with all aspects of the executor. As you're getting started, I recommend exploring the script editor and trying some basic scripts. What would you like to learn about first?";
        } else if (context.proficiency == UserProficiency::Intermediate) {
            return "I can assist with any executor functionality you need. Based on your experience level, you might be interested in creating custom scripts or using the vulnerability scanner. How can I help you today?";
        } else {
            return "As an experienced user, you probably know most of the executor's capabilities. I'm here if you need advanced assistance with optimization, security bypasses, or custom implementations. What are you working on?";
        }
    }
}

// Private: Extract entities from input
std::vector<std::string> GeneralAssistantModel::ExtractEntities(const std::string& input) const {
    // In a production implementation, this would use NLP to extract entities
    // For now, we'll use a simple keyword-based approach
    std::vector<std::string> entities;
    std::string lowercaseInput = input;
    std::transform(lowercaseInput.begin(), lowercaseInput.end(), lowercaseInput.begin(), ::tolower);
    
    // Script types
    if (lowercaseInput.find("aimbot") != std::string::npos) entities.push_back("aimbot");
    if (lowercaseInput.find("esp") != std::string::npos) entities.push_back("esp");
    if (lowercaseInput.find("wallhack") != std::string::npos) entities.push_back("wallhack");
    if (lowercaseInput.find("speed") != std::string::npos) entities.push_back("speed");
    if (lowercaseInput.find("teleport") != std::string::npos) entities.push_back("teleport");
    if (lowercaseInput.find("noclip") != std::string::npos) entities.push_back("noclip");
    if (lowercaseInput.find("fly") != std::string::npos) entities.push_back("fly");
    if (lowercaseInput.find("auto") != std::string::npos) entities.push_back("automation");
    
    // Game names
    if (lowercaseInput.find("adopt me") != std::string::npos) entities.push_back("Adopt Me");
    if (lowercaseInput.find("jailbreak") != std::string::npos) entities.push_back("Jailbreak");
    if (lowercaseInput.find("arsenal") != std::string::npos) entities.push_back("Arsenal");
    if (lowercaseInput.find("phantom forces") != std::string::npos) entities.push_back("Phantom Forces");
    if (lowercaseInput.find("blox fruits") != std::string::npos) entities.push_back("Blox Fruits");
    
    // Vulnerability types
    if (lowercaseInput.find("remote") != std::string::npos) entities.push_back("remote events");
    if (lowercaseInput.find("admin") != std::string::npos) entities.push_back("admin commands");
    if (lowercaseInput.find("backdoor") != std::string::npos) entities.push_back("backdoor");
    
    return entities;
}

// Private: Find relevant topics
std::vector<std::string> GeneralAssistantModel::FindRelevantTopics(const std::string& input) const {
    // In a production implementation, this would use topic modeling or classification
    // For now, we'll use a simple keyword-based approach
    std::vector<std::string> topics;
    std::string lowercaseInput = input;
    std::transform(lowercaseInput.begin(), lowercaseInput.end(), lowercaseInput.begin(), ::tolower);
    
    // High-level topics
    if (lowercaseInput.find("script") != std::string::npos) topics.push_back("scripting");
    if (lowercaseInput.find("game") != std::string::npos) topics.push_back("games");
    if (lowercaseInput.find("exploit") != std::string::npos || 
        lowercaseInput.find("hack") != std::string::npos) topics.push_back("exploitation");
    if (lowercaseInput.find("security") != std::string::npos || 
        lowercaseInput.find("vulnerability") != std::string::npos) topics.push_back("security");
    if (lowercaseInput.find("ui") != std::string::npos || 
        lowercaseInput.find("interface") != std::string::npos) topics.push_back("ui");
    if (lowercaseInput.find("feature") != std::string::npos) topics.push_back("features");
    
    // Script types
    if (lowercaseInput.find("aimbot") != std::string::npos) topics.push_back("aimbot");
    if (lowercaseInput.find("esp") != std::string::npos) topics.push_back("esp");
    if (lowercaseInput.find("wallhack") != std::string::npos) topics.push_back("wallhack");
    if (lowercaseInput.find("speed") != std::string::npos) topics.push_back("speed");
    if (lowercaseInput.find("teleport") != std::string::npos) topics.push_back("teleport");
    if (lowercaseInput.find("noclip") != std::string::npos) topics.push_back("noclip");
    if (lowercaseInput.find("auto") != std::string::npos) topics.push_back("automation");
    
    // Game-specific topics
    if (lowercaseInput.find("adopt me") != std::string::npos) topics.push_back("Adopt Me");
    if (lowercaseInput.find("jailbreak") != std::string::npos) topics.push_back("Jailbreak");
    if (lowercaseInput.find("arsenal") != std::string::npos) topics.push_back("Arsenal");
    if (lowercaseInput.find("phantom forces") != std::string::npos) topics.push_back("Phantom Forces");
    if (lowercaseInput.find("blox fruits") != std::string::npos) topics.push_back("Blox Fruits");
    
    // Functionality topics
    if (lowercaseInput.find("debug") != std::string::npos) topics.push_back("debugging");
    if (lowercaseInput.find("error") != std::string::npos) topics.push_back("errors");
    if (lowercaseInput.find("performance") != std::string::npos) topics.push_back("performance");
    if (lowercaseInput.find("detection") != std::string::npos) topics.push_back("detection");
    if (lowercaseInput.find("bypass") != std::string::npos) topics.push_back("bypass");
    if (lowercaseInput.find("byfron") != std::string::npos) topics.push_back("byfron");
    
    return topics;
}

} // namespace LocalModels
} // namespace AIFeatures
} // namespace iOS
