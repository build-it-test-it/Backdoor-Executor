# AI Integration Guide

This guide explains how to integrate the AI features into your iOS executor project.

## Overview

The AI system provides two main components:

1. **Script Assistant**: Helps users write, understand, and debug Lua scripts
2. **Signature Adaptation**: Adapts to Byfron anti-cheat updates automatically

Both components are designed to work on non-jailbroken devices with efficient memory usage.

## Step 1: Add Files to Your Project

First, add these files to your Xcode project:

```
source/cpp/ios/ai_features/
├── ScriptAssistant.h           # AI assistant for scripts
├── ScriptAssistant.mm
├── SignatureAdaptation.h       # Adaptive anti-cheat bypassing
├── SignatureAdaptation.mm
├── AIIntegration.h             # Main integration interface
├── AIIntegration.mm
```

## Step 2: Add AI Models

Create a `Models` folder in your app bundle's resources directory:

```
YourApp.app/
└── Resources/
    └── Models/
        ├── script_assistant.mlmodelc   # Core assistant model
        ├── script_generator.mlmodelc   # Script generation model
        ├── pattern_recognition.mlmodelc # Byfron pattern detection
        └── behavior_prediction.mlmodelc # Behavior prediction model
```

You can download pre-trained models from your development server or include them in your app bundle.

## Step 3: Initialize AI in Your App Delegate

Add this code to your application's initialization:

```objective-c
// In your AppDelegate.m file
#import "ios/ai_features/AIIntegration.h"

@interface AppDelegate ()
@property (nonatomic, assign) void* aiIntegration;
@end

@implementation AppDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    // Initialize other components
    
    // Initialize AI with progress callback
    self.aiIntegration = InitializeAI(^(float progress) {
        NSLog(@"AI initialization progress: %f", progress);
        // Update loading UI if needed
    });
    
    // Continue with other initialization
    return YES;
}

- (void)applicationDidReceiveMemoryWarning:(UIApplication *)application {
    // Handle memory warnings for AI
    if (self.aiIntegration) {
        HandleAppMemoryWarning(self.aiIntegration);
    }
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
    // Handle app becoming active
    if (self.aiIntegration) {
        HandleAppForeground(self.aiIntegration);
    }
}

@end
```

## Step 4: Connect AI to Main UI

Add this code to your main view controller setup:

```objective-c
// In your main view controller implementation
#import "ios/ai_features/AIIntegration.h"

@interface MainViewController ()
@property (nonatomic, strong) UIButton *aiButton;
@property (nonatomic, strong) UIViewController *aiResponseVC;
@end

@implementation MainViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    // Get AI integration from app delegate
    AppDelegate *appDelegate = (AppDelegate *)[[UIApplication sharedApplication] delegate];
    void *aiIntegration = appDelegate.aiIntegration;
    
    // Connect AI to UI
    std::shared_ptr<iOS::UI::MainViewController> mainVC = std::make_shared<iOS::UI::MainViewController>();
    mainVC->Initialize();
    SetupAIWithUI(aiIntegration, &mainVC);
    
    // Add AI button to your toolbar
    _aiButton = [UIButton buttonWithType:UIButtonTypeSystem];
    [_aiButton setTitle:@"AI Help" forState:UIControlStateNormal];
    [_aiButton addTarget:self action:@selector(showAIHelp:) forControlEvents:UIControlEventTouchUpInside];
    [self.toolbar addSubview:_aiButton];
    
    // Continue with regular setup
}

- (void)showAIHelp:(id)sender {
    // Get current script from editor
    NSString *scriptContent = self.codeTextView.text;
    
    // Get app delegate
    AppDelegate *appDelegate = (AppDelegate *)[[UIApplication sharedApplication] delegate];
    
    // Process query with AI
    ProcessAIQuery(appDelegate.aiIntegration, 
                  [NSString stringWithFormat:@"Help me understand this script: %@", scriptContent].UTF8String,
                  ^(const char* response) {
        // Show response on main thread
        dispatch_async(dispatch_get_main_queue(), ^{
            [self showAIResponse:[NSString stringWithUTF8String:response]];
        });
    });
}

- (void)showAIResponse:(NSString *)response {
    // Create response view controller if needed
    if (!_aiResponseVC) {
        _aiResponseVC = [[UIViewController alloc] init];
        UITextView *textView = [[UITextView alloc] initWithFrame:_aiResponseVC.view.bounds];
        textView.editable = NO;
        textView.tag = 100; // For finding later
        [_aiResponseVC.view addSubview:textView];
    }
    
    // Update response text
    UITextView *textView = (UITextView *)[_aiResponseVC.view viewWithTag:100];
    textView.text = response;
    
    // Present response
    [self presentViewController:_aiResponseVC animated:YES completion:nil];
}

@end
```

## Step 5: Implement Script Debugging with AI

Add this to your script editor view controller:

```objective-c
- (void)debugScriptWithAI {
    // Get current script
    NSString *script = self.codeTextView.text;
    
    // Get app delegate
    AppDelegate *appDelegate = (AppDelegate *)[[UIApplication sharedApplication] delegate];
    
    // Send to AI for debugging
    ProcessAIQuery(appDelegate.aiIntegration, 
                  [NSString stringWithFormat:@"Debug this script and tell me what's wrong: %@", script].UTF8String,
                  ^(const char* response) {
        // Show debug results on main thread
        dispatch_async(dispatch_get_main_queue(), ^{
            [self showDebugResults:[NSString stringWithUTF8String:response]];
        });
    });
}

- (void)showDebugResults:(NSString *)results {
    // Create debug panel
    UIViewController *debugVC = [[UIViewController alloc] init];
    debugVC.title = @"AI Debug Results";
    
    UITextView *textView = [[UITextView alloc] initWithFrame:debugVC.view.bounds];
    textView.text = results;
    textView.editable = NO;
    [debugVC.view addSubview:textView];
    
    // Present debug results
    [self presentViewController:debugVC animated:YES completion:nil];
}
```

## Step 6: Connect Byfron Detection to AI

Add this code to your anti-cheat detection system:

```objective-c
- (void)reportByfronDetection:(NSString *)detectionType signature:(NSData *)signature {
    // Get app delegate
    AppDelegate *appDelegate = (AppDelegate *)[[UIApplication sharedApplication] delegate];
    
    // Get signature adaptation system
    void *sigAdaptationPtr = GetSignatureAdaptation(appDelegate.aiIntegration);
    auto signatureAdaptation = *(static_cast<std::shared_ptr<iOS::AIFeatures::SignatureAdaptation>*>(sigAdaptationPtr));
    
    // Create detection event
    iOS::AIFeatures::SignatureAdaptation::DetectionEvent event;
    event.m_detectionType = [detectionType UTF8String];
    
    // Convert NSData to std::vector<uint8_t>
    const uint8_t *bytes = (const uint8_t*)[signature bytes];
    event.m_signature = std::vector<uint8_t>(bytes, bytes + [signature length]);
    
    // Report the detection
    signatureAdaptation->ReportDetection(event);
}
```

## Step 7: Implement Script Generation

Add this to your script creation UI:

```objective-c
- (void)generateScriptWithAI:(NSString *)description {
    // Get app delegate
    AppDelegate *appDelegate = (AppDelegate *)[[UIApplication sharedApplication] delegate];
    
    // Generate script
    ProcessAIQuery(appDelegate.aiIntegration, 
                  [NSString stringWithFormat:@"Generate a script that: %@", description].UTF8String,
                  ^(const char* response) {
        // Extract script from response
        NSString *fullResponse = [NSString stringWithUTF8String:response];
        
        // Find script between code blocks (```lua ... ```)
        NSRange startRange = [fullResponse rangeOfString:@"```lua"];
        NSRange endRange = [fullResponse rangeOfString:@"```" options:0 range:NSMakeRange(NSMaxRange(startRange), fullResponse.length - NSMaxRange(startRange))];
        
        NSString *script = @"-- Could not extract script";
        
        if (startRange.location != NSNotFound && endRange.location != NSNotFound) {
            NSRange scriptRange = NSMakeRange(NSMaxRange(startRange), endRange.location - NSMaxRange(startRange));
            script = [fullResponse substringWithRange:scriptRange];
        }
        
        // Update UI on main thread
        dispatch_async(dispatch_get_main_queue(), ^{
            [self createNewScriptWithContent:script];
        });
    });
}
```

## Step 8: Add AI Tab to Main UI

Add a dedicated AI assistant tab to your main interface:

```objective-c
- (void)setupAITab {
    // Create AI assistant view controller
    UIViewController *aiVC = [[UIViewController alloc] init];
    aiVC.title = @"AI Assistant";
    
    // Create UI
    UITextView *promptTextView = [[UITextView alloc] initWithFrame:CGRectMake(20, 20, self.view.bounds.size.width - 40, 100)];
    promptTextView.tag = 101;
    [aiVC.view addSubview:promptTextView];
    
    UIButton *askButton = [UIButton buttonWithType:UIButtonTypeSystem];
    askButton.frame = CGRectMake(20, 130, 100, 40);
    [askButton setTitle:@"Ask AI" forState:UIControlStateNormal];
    [askButton addTarget:self action:@selector(askAI:) forControlEvents:UIControlEventTouchUpInside];
    [aiVC.view addSubview:askButton];
    
    UITextView *responseTextView = [[UITextView alloc] initWithFrame:CGRectMake(20, 180, self.view.bounds.size.width - 40, self.view.bounds.size.height - 200)];
    responseTextView.tag = 102;
    responseTextView.editable = NO;
    [aiVC.view addSubview:responseTextView];
    
    // Add to tab controller
    [self.tabBarController addChildViewController:aiVC];
}

- (void)askAI:(id)sender {
    // Get parent view controller
    UIViewController *aiVC = [sender superview].superview.nextResponder;
    
    // Get text views
    UITextView *promptTextView = (UITextView *)[aiVC.view viewWithTag:101];
    UITextView *responseTextView = (UITextView *)[aiVC.view viewWithTag:102];
    
    // Get prompt text
    NSString *prompt = promptTextView.text;
    
    // Get app delegate
    AppDelegate *appDelegate = (AppDelegate *)[[UIApplication sharedApplication] delegate];
    
    // Show "Thinking..." indicator
    responseTextView.text = @"AI is thinking...";
    
    // Process query
    ProcessAIQuery(appDelegate.aiIntegration, [prompt UTF8String], ^(const char* response) {
        // Update response on main thread
        dispatch_async(dispatch_get_main_queue(), ^{
            responseTextView.text = [NSString stringWithUTF8String:response];
        });
    });
}
```

## Memory Management

The AI system is designed to be memory-efficient:

1. It automatically releases resources when the app receives memory warnings
2. Less important models are unloaded first
3. Core functionality remains available even under memory pressure

You don't need to worry about manual memory management for the AI components - they handle it automatically.

## Customizing the AI

You can customize various aspects of the AI:

1. **Script generation style**: Modify prompts to generate scripts in a particular style
2. **Debugging detail level**: Ask for more or less detailed debugging information
3. **UI integration**: Embed AI assistance directly in the editor or in separate views

## Usage Tips

1. **Be Specific**: When asking the AI for help, be specific about what you need
2. **Provide Context**: Include relevant game information when generating scripts
3. **Check Memory Usage**: Monitor AI memory usage in development to ensure efficiency

## Troubleshooting

### AI Not Initializing

Check that:
- All required model files are in the correct location
- You have sufficient disk space (at least 200MB free)
- Device is running iOS 15 or later (preferred)

### AI Responses Are Slow

- Try reducing the model complexity in low-memory conditions
- Ensure you're running on the background thread
- Check for memory leaks in your application

### Memory Warnings When Using AI

This is normal, especially on devices with limited RAM. The AI will automatically:
1. Release non-essential resources
2. Degrade gracefully to simpler models
3. Maintain core functionality

## Conclusion

The AI integration enhances your executor with:
- Intelligent script generation and debugging
- Adaptive Byfron bypass protection
- User-friendly assistance for beginners
- Memory-efficient operation on non-jailbroken devices

All components are designed to work seamlessly together while respecting iOS resource constraints.
