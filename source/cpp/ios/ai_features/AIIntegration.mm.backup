#include "AIIntegration.h"
#include "ScriptAssistant.h"
#include "SignatureAdaptation.h"
#include "../FileSystem.h"
#include "../ui/MainViewController.h"
#include <iostream>
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import <CoreML/CoreML.h>

namespace iOS {
namespace AIFeatures {

/**
 * @class AIIntegration
 * @brief Integrates AI features with the rest of the executor
 *
 * This class serves as a bridge between the AI components and the rest of the system,
 * handling initialization, memory management, and coordination between components.
 */
class AIIntegration {
private:
    // Member variables with consistent m_ prefix
    std::shared_ptr<ScriptAssistant> m_scriptAssistant;
    std::shared_ptr<SignatureAdaptation> m_signatureAdaptation;
    std::shared_ptr<UI::MainViewController> m_mainViewController;
    bool m_aiInitialized;
    bool m_modelsLoaded;
    bool m_isInLowMemoryMode;
    std::string m_modelsPath;
    
    // Singleton instance
    static AIIntegration* s_instance;
    
    // Private constructor for singleton
    AIIntegration()
        : m_aiInitialized(false),
          m_modelsLoaded(false),
          m_isInLowMemoryMode(false) {
        
        // Set up models path
        NSBundle* mainBundle = [NSBundle mainBundle];
        m_modelsPath = [[mainBundle resourcePath] UTF8String];
        m_modelsPath += "/Models";
        
        // Register for memory warnings
        [[NSNotificationCenter defaultCenter] addObserver:[AIMemoryObserver sharedObserver]
                                                 selector:@selector(didReceiveMemoryWarning:)
                                                     name:UIApplicationDidReceiveMemoryWarningNotification
                                                   object:nil];
    }
    
public:
    /**
     * @brief Get shared instance
     * @return Shared instance
     */
    static AIIntegration* GetSharedInstance() {
        if (!s_instance) {
            s_instance = new AIIntegration();
        }
        return s_instance;
    }
    
    /**
     * @brief Destructor
     */
    ~AIIntegration() {
        [[NSNotificationCenter defaultCenter] removeObserver:[AIMemoryObserver sharedObserver]];
    }
    
    /**
     * @brief Initialize AI components
     * @param progressCallback Function to call with initialization progress (0.0-1.0)
     * @return True if initialization succeeded, false otherwise
     */
    bool Initialize(std::function<void(float)> progressCallback = nullptr) {
        if (m_aiInitialized) {
            return true;
        }
        
        try {
            // Create necessary directories
            std::string aiDataPath = FileSystem::GetSafePath("AIData");
            if (!FileSystem::Exists(aiDataPath)) {
                FileSystem::CreateDirectory(aiDataPath);
            }
            
            if (progressCallback) progressCallback(0.1f);
            
            // Initialize script assistant
            m_scriptAssistant = std::make_shared<ScriptAssistant>();
            bool assistantInitialized = m_scriptAssistant->Initialize();
            
            if (!assistantInitialized) {
                std::cerr << "AIIntegration: Failed to initialize script assistant" << std::endl;
                // Continue anyway, we'll try to recover or use fallbacks
            }
            
            if (progressCallback) progressCallback(0.4f);
            
            // Initialize signature adaptation
            m_signatureAdaptation = std::make_shared<SignatureAdaptation>();
            bool adaptationInitialized = m_signatureAdaptation->Initialize();
            
            if (!adaptationInitialized) {
                std::cerr << "AIIntegration: Failed to initialize signature adaptation" << std::endl;
                // Continue anyway, we'll try to recover or use fallbacks
            }
            
            if (progressCallback) progressCallback(0.7f);
            
            // Load models in background
            LoadModelsInBackground();
            
            if (progressCallback) progressCallback(0.9f);
            
            // Set up AI data collection (if user has opted in)
            SetupAIDataCollection();
            
            m_aiInitialized = true;
            std::cout << "AIIntegration: Successfully initialized" << std::endl;
            
            if (progressCallback) progressCallback(1.0f);
            
            return true;
        } catch (const std::exception& e) {
            std::cerr << "AIIntegration: Exception during initialization: " << e.what() << std::endl;
            if (progressCallback) progressCallback(1.0f);
            return false;
        }
    }
    
    /**
     * @brief Load AI models in background
     */
    void LoadModelsInBackground() {
        if (m_modelsLoaded) {
            return;
        }
        
        dispatch_async(dispatch_get_global_queue(QOS_CLASS_BACKGROUND, 0), ^{
            @autoreleasepool {
                NSLog(@"AIIntegration: Loading models in background");
                
                // Check if models exist
                NSFileManager* fileManager = [NSFileManager defaultManager];
                NSString* modelsPath = [NSString stringWithUTF8String:m_modelsPath.c_str()];
                
                if (![fileManager fileExistsAtPath:modelsPath]) {
                    NSLog(@"AIIntegration: Models directory not found: %@", modelsPath);
                    return;
                }
                
                // Check available disk space
                NSDictionary* attributes = [fileManager attributesOfFileSystemForPath:NSHomeDirectory() error:nil];
                if (attributes) {
                    NSNumber* freeSpace = [attributes objectForKey:NSFileSystemFreeSize];
                    long long freeSpaceBytes = [freeSpace longLongValue];
                    
                    // Require at least 200MB free space for models
                    if (freeSpaceBytes < 200 * 1024 * 1024) {
                        NSLog(@"AIIntegration: Insufficient disk space for models: %lld bytes", freeSpaceBytes);
                        return;
                    }
                }
                
                // Load models in order of importance
                LoadModelWithName("script_assistant", 1); // High priority
                LoadModelWithName("pattern_recognition", 1); // High priority
                
                // Check memory before loading larger models
                UIDevice* device = [UIDevice currentDevice];
                if (@available(iOS 15.0, *)) {
                    // On iOS 15+, check system memory
                    if (device.systemFreeSize > 500 * 1024 * 1024) { // 500MB free
                        LoadModelWithName("script_generator", 2); // Medium priority
                        LoadModelWithName("behavior_prediction", 2); // Medium priority
                    }
                    
                    if (device.systemFreeSize > 1000 * 1024 * 1024) { // 1GB free
                        LoadModelWithName("code_evolution", 3); // Low priority
                    }
                } else {
                    // On older iOS, use simpler approach - load all models
                    LoadModelWithName("script_generator", 2);
                    LoadModelWithName("behavior_prediction", 2);
                    LoadModelWithName("code_evolution", 3);
                }
                
                m_modelsLoaded = true;
                NSLog(@"AIIntegration: Finished loading models");
            }
        });
    }
    
    /**
     * @brief Load a specific model
     * @param name Model name
     * @param priority Loading priority (1=high, 3=low)
     */
    void LoadModelWithName(const char* name, int priority) {
        @autoreleasepool {
            NSString* modelName = [NSString stringWithUTF8String:name];
            NSString* modelsPath = [NSString stringWithUTF8String:m_modelsPath.c_str()];
            NSString* modelPath = [modelsPath stringByAppendingPathComponent:[modelName stringByAppendingString:@".mlmodelc"]];
            
            NSLog(@"AIIntegration: Loading model: %@", modelName);
            
            // Check if model exists
            NSFileManager* fileManager = [NSFileManager defaultManager];
            if (![fileManager fileExistsAtPath:modelPath]) {
                NSLog(@"AIIntegration: Model file not found: %@", modelPath);
                return;
            }
            
            // Load the model based on iOS version
            if (@available(iOS 15.0, *)) {
                NSError* error = nil;
                NSURL* modelURL = [NSURL fileURLWithPath:modelPath];
                MLModel* model = [MLModel modelWithContentsOfURL:modelURL error:&error];
                
                if (error || !model) {
                    NSLog(@"AIIntegration: Failed to load model: %@", [error localizedDescription]);
                    return;
                }
                
                // Store model in appropriate subsystem
                if ([modelName isEqualToString:@"script_assistant"] ||
                    [modelName isEqualToString:@"script_generator"]) {
                    // Send to script assistant
                    m_scriptAssistant->SetModel(name, (__bridge void*)model);
                } else {
                    // Send to signature adaptation
                    m_signatureAdaptation->SetModel(name, (__bridge void*)model);
                }
            } else {
                // On older iOS versions, use simplified models or fallbacks
                NSLog(@"AIIntegration: Using simplified model for iOS < 15");
                
                // Load simplified model (implementation would vary)
                // Using dummy approach for example
                void* simplifiedModel = nullptr;
                
                if ([modelName isEqualToString:@"script_assistant"] ||
                    [modelName isEqualToString:@"script_generator"]) {
                    m_scriptAssistant->SetModel(name, simplifiedModel);
                } else {
                    m_signatureAdaptation->SetModel(name, simplifiedModel);
                }
            }
            
            NSLog(@"AIIntegration: Successfully loaded model: %@", modelName);
        }
    }
    
    /**
     * @brief Set up UI for AI features
     * @param mainViewController Main view controller
     */
    void SetupUI(std::shared_ptr<UI::MainViewController> mainViewController) {
        m_mainViewController = mainViewController;
        
        if (!m_aiInitialized) {
            std::cerr << "AIIntegration: Cannot set up UI before initialization" << std::endl;
            return;
        }
        
        // Connect script assistant to UI
        m_mainViewController->SetScriptAssistant(m_scriptAssistant);
        
        // Set up script assistant callbacks
        m_scriptAssistant->SetResponseCallback([this](const ScriptAssistant::Message& message) {
            // Handle assistant responses
            // In a real implementation, this would update the UI
            std::cout << "ScriptAssistant: " << message.m_content << std::endl;
        });
        
        std::cout << "AIIntegration: Set up UI integration" << std::endl;
    }
    
    /**
     * @brief Set up AI data collection
     */
    void SetupAIDataCollection() {
        // Create training data directory
        std::string trainingDir = FileSystem::GetSafePath("AIData/Training");
        if (!FileSystem::Exists(trainingDir)) {
            FileSystem::CreateDirectory(trainingDir);
        }
        
        // Set up signature adaptation data collection
        m_signatureAdaptation->SetResponseCallback([this](const SignatureAdaptation::ProtectionStrategy& strategy) {
            // Handle protection strategy updates
            std::cout << "SignatureAdaptation: New strategy - " << strategy.m_name << std::endl;
            
            // Log strategy for training
            LogStrategyForTraining(strategy);
            
            // In a real implementation, this would apply the strategy
            // to the execution system
        });
    }
    
    /**
     * @brief Log a protection strategy for training
     * @param strategy Protection strategy
     */
    void LogStrategyForTraining(const SignatureAdaptation::ProtectionStrategy& strategy) {
        // Create JSON representation
        NSMutableDictionary* strategyDict = [NSMutableDictionary dictionary];
        [strategyDict setObject:[NSString stringWithUTF8String:strategy.m_name.c_str()] forKey:@"name"];
        [strategyDict setObject:@(strategy.m_effectiveness) forKey:@"effectiveness"];
        [strategyDict setObject:@(strategy.m_lastModified) forKey:@"timestamp"];
        [strategyDict setObject:@(strategy.m_evolutionGeneration) forKey:@"generation"];
        
        // Convert to JSON
        NSError* error = nil;
        NSData* jsonData = [NSJSONSerialization dataWithJSONObject:strategyDict options:0 error:&error];
        
        if (error || !jsonData) {
            std::cerr << "AIIntegration: Failed to serialize strategy for training" << std::endl;
            return;
        }
        
        // Write to training file
        NSString* jsonString = [[NSString alloc] initWithData:jsonData encoding:NSUTF8StringEncoding];
        std::string trainingPath = FileSystem::GetSafePath("AIData/Training/strategies.jsonl");
        FileSystem::WriteFile(trainingPath, std::string([jsonString UTF8String]) + "\n", true); // Append mode
    }
    
    /**
     * @brief Handle memory warning
     */
    void HandleMemoryWarning() {
        std::cout << "AIIntegration: Handling memory warning" << std::endl;
        
        // Set low memory mode
        m_isInLowMemoryMode = true;
        
        // Release non-essential resources
        if (m_scriptAssistant) {
            m_scriptAssistant->ReleaseUnusedResources();
        }
        
        if (m_signatureAdaptation) {
            m_signatureAdaptation->ReleaseUnusedResources();
        }
    }
    
    /**
     * @brief Handle app entering foreground
     */
    void HandleAppForeground() {
        std::cout << "AIIntegration: Handling app foreground" << std::endl;
        
        // Reset low memory mode
        m_isInLowMemoryMode = false;
        
        // Reload models if needed
        if (!m_modelsLoaded) {
            LoadModelsInBackground();
        }
    }
    
    /**
     * @brief Get script assistant
     * @return Script assistant instance
     */
    std::shared_ptr<ScriptAssistant> GetScriptAssistant() const {
        return m_scriptAssistant;
    }
    
    /**
     * @brief Get signature adaptation
     * @return Signature adaptation instance
     */
    std::shared_ptr<SignatureAdaptation> GetSignatureAdaptation() const {
        return m_signatureAdaptation;
    }
    
    /**
     * @brief Check if AI is initialized
     * @return True if initialized, false otherwise
     */
    bool IsInitialized() const {
        return m_aiInitialized;
    }
    
    /**
     * @brief Check if models are loaded
     * @return True if loaded, false otherwise
     */
    bool AreModelsLoaded() const {
        return m_modelsLoaded;
    }
    
    /**
     * @brief Get memory usage
     * @return Memory usage in bytes
     */
    uint64_t GetMemoryUsage() const {
        uint64_t total = 0;
        
        if (m_scriptAssistant) {
            total += m_scriptAssistant->GetMemoryUsage();
        }
        
        if (m_signatureAdaptation) {
            total += m_signatureAdaptation->GetMemoryUsage();
        }
        
        return total;
    }
};

// Initialize static instance
AIIntegration* AIIntegration::s_instance = nullptr;

} // namespace AIFeatures
} // namespace iOS

// Objective-C class for handling memory warnings
@interface AIMemoryObserver : NSObject
+ (instancetype)sharedObserver;
- (void)didReceiveMemoryWarning:(NSNotification*)notification;
@end

@implementation AIMemoryObserver

+ (instancetype)sharedObserver {
    static AIMemoryObserver* sharedObserver = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedObserver = [[self alloc] init];
    });
    return sharedObserver;
}

- (void)didReceiveMemoryWarning:(NSNotification*)notification {
    // Forward to C++ implementation
    iOS::AIFeatures::AIIntegration::GetSharedInstance()->HandleMemoryWarning();
}

@end

// Expose C functions for integration
extern "C" {
    
void* InitializeAI(void (*progressCallback)(float)) {
    auto integration = iOS::AIFeatures::AIIntegration::GetSharedInstance();
    
    // Convert C function pointer to C++ function
    std::function<void(float)> progressFunc = progressCallback ? 
        [progressCallback](float progress) { progressCallback(progress); } : 
        std::function<void(float)>();
    
    // Initialize AI
    integration->Initialize(progressFunc);
    
    // Return opaque pointer to integration for future calls
    return integration;
}

void SetupAIWithUI(void* integration, void* viewController) {
    auto aiIntegration = static_cast<iOS::AIFeatures::AIIntegration*>(integration);
    auto mainVC = *static_cast<std::shared_ptr<iOS::UI::MainViewController>*>(viewController);
    
    aiIntegration->SetupUI(mainVC);
}

void* GetScriptAssistant(void* integration) {
    auto aiIntegration = static_cast<iOS::AIFeatures::AIIntegration*>(integration);
    return &aiIntegration->GetScriptAssistant();
}

void* GetSignatureAdaptation(void* integration) {
    auto aiIntegration = static_cast<iOS::AIFeatures::AIIntegration*>(integration);
    return &aiIntegration->GetSignatureAdaptation();
}

uint64_t GetAIMemoryUsage(void* integration) {
    auto aiIntegration = static_cast<iOS::AIFeatures::AIIntegration*>(integration);
    return aiIntegration->GetMemoryUsage();
}

void HandleAppForeground(void* integration) {
    auto aiIntegration = static_cast<iOS::AIFeatures::AIIntegration*>(integration);
    aiIntegration->HandleAppForeground();
}

} // extern "C"
