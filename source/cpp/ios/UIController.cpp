#include "UIController.h"
#include <algorithm>
#include <chrono>
#include <iostream>
#include <thread>
#import <UIKit/UIKit.h>
#import <Foundation/Foundation.h>
#import <objc/runtime.h>

namespace iOS {
    
    // Constructor
    UIController::UIController()
        : m_uiView(nullptr),
          m_floatingButton(std::make_unique<FloatingButtonController>()),
          m_isVisible(false),
          m_currentTab(TabType::Editor),
          m_opacity(0.9f),
          m_isDraggable(true),
          m_currentScript("") {
        // Initialize with empty callbacks
        m_executeCallback = [](const std::string&) { return false; };
        m_saveScriptCallback = [](const ScriptInfo&) { return false; };
        m_loadScriptsCallback = []() { return std::vector<ScriptInfo>(); };
    }
    
    // Destructor
    UIController::~UIController() {
        // Save UI state before destroying
        SaveUIState();
        
        // Release the UI view (will be handled by ARC, but we need to release our reference)
        if (m_uiView) {
            m_uiView = nullptr;
        }
        
        // FloatingButtonController is handled by unique_ptr
    }
    
    // Initialize the UI
    bool UIController::Initialize() {
        // Create the UI elements
        CreateUI();
        
        // Load saved UI state
        LoadUIState();
        
        // Set up the floating button
        if (m_floatingButton) {
            m_floatingButton->SetTapCallback([this]() {
                Toggle();
            });
        }
        
        // Initial refresh of scripts list
        RefreshScriptsList();
        
        return true;
    }
    
    // Show the UI
    void UIController::Show() {
        if (m_isVisible) return;
        
        // Use dispatch_async to ensure UI operations happen on the main thread
        dispatch_async(dispatch_get_main_queue(), ^{
            if (m_uiView) {
                UIView* view = (__bridge UIView*)m_uiView;
                view.hidden = NO;
                view.alpha = 0.0;
                
                // Animate the appearance
                [UIView animateWithDuration:0.25 animations:^{
                    view.alpha = m_opacity;
                } completion:nil];
            }
        });
        
        m_isVisible = true;
    }
    
    // Hide the UI
    void UIController::Hide() {
        if (!m_isVisible) return;
        
        // Use dispatch_async to ensure UI operations happen on the main thread
        dispatch_async(dispatch_get_main_queue(), ^{
            if (m_uiView) {
                UIView* view = (__bridge UIView*)m_uiView;
                
                // Animate the disappearance
                [UIView animateWithDuration:0.25 animations:^{
                    view.alpha = 0.0;
                } completion:^(BOOL finished) {
                    view.hidden = YES;
                }];
            }
        });
        
        m_isVisible = false;
    }
    
    // Toggle UI visibility
    bool UIController::Toggle() {
        if (m_isVisible) {
            Hide();
        } else {
            Show();
        }
        return m_isVisible;
    }
    
    // Check if UI is visible
    bool UIController::IsVisible() const {
        return m_isVisible;
    }
    
    // Switch to a specific tab
    void UIController::SwitchTab(TabType tab) {
        if (tab == m_currentTab) return;
        
        m_currentTab = tab;
        
        // Update UI to show the selected tab
        dispatch_async(dispatch_get_main_queue(), ^{
            if (m_uiView) {
                UIView* view = (__bridge UIView*)m_uiView;
                
                // Get the tab buttons and content views
                UITabBar* tabBar = [view viewWithTag:1000];
                UIView* editorView = [view viewWithTag:1001];
                UIView* scriptsView = [view viewWithTag:1002];
                UIView* consoleView = [view viewWithTag:1003];
                UIView* settingsView = [view viewWithTag:1004];
                
                // Hide all content views
                editorView.hidden = YES;
                scriptsView.hidden = YES;
                consoleView.hidden = YES;
                settingsView.hidden = YES;
                
                // Show the selected content view
                switch (m_currentTab) {
                    case TabType::Editor:
                        editorView.hidden = NO;
                        tabBar.selectedItem = tabBar.items[0];
                        break;
                    case TabType::Scripts:
                        scriptsView.hidden = NO;
                        tabBar.selectedItem = tabBar.items[1];
                        break;
                    case TabType::Console:
                        consoleView.hidden = NO;
                        tabBar.selectedItem = tabBar.items[2];
                        break;
                    case TabType::Settings:
                        settingsView.hidden = NO;
                        tabBar.selectedItem = tabBar.items[3];
                        break;
                }
                
                // Apply a simple fade transition
                [UIView transitionWithView:view
                                  duration:0.2
                                   options:UIViewAnimationOptionTransitionCrossDissolve
                                animations:nil
                                completion:nil];
            }
        });
        
        UpdateLayout();
    }
    
    // Get current tab
    UIController::TabType UIController::GetCurrentTab() const {
        return m_currentTab;
    }
    
    // Set UI opacity
    void UIController::SetOpacity(float opacity) {
        // Clamp opacity to valid range
        m_opacity = std::max(0.0f, std::min(1.0f, opacity));
        
        // Update UI opacity if visible
        if (m_isVisible) {
            dispatch_async(dispatch_get_main_queue(), ^{
                if (m_uiView) {
                    UIView* view = (__bridge UIView*)m_uiView;
                    view.alpha = m_opacity;
                }
            });
        }
    }
    
    // Get UI opacity
    float UIController::GetOpacity() const {
        return m_opacity;
    }
    
    // Enable/disable UI dragging
    void UIController::SetDraggable(bool enabled) {
        m_isDraggable = enabled;
        
        // Update the draggability of the UI
        dispatch_async(dispatch_get_main_queue(), ^{
            if (m_uiView) {
                UIView* view = (__bridge UIView*)m_uiView;
                UIPanGestureRecognizer* panGesture = nil;
                
                // Find existing pan gesture if any
                for (UIGestureRecognizer* gesture in view.gestureRecognizers) {
                    if ([gesture isKindOfClass:[UIPanGestureRecognizer class]]) {
                        panGesture = (UIPanGestureRecognizer*)gesture;
                        break;
                    }
                }
                
                // Enable or disable the gesture
                if (panGesture) {
                    panGesture.enabled = m_isDraggable;
                } else if (m_isDraggable) {
                    // Create new gesture if needed
                    panGesture = [[UIPanGestureRecognizer alloc] initWithTarget:nil action:@selector(handlePan:)];
                    [view addGestureRecognizer:panGesture];
                    
                    // Define a block to handle pan gesture (for dragging the UI)
                    static void (^panHandler)(UIPanGestureRecognizer*) = ^(UIPanGestureRecognizer* gesture) {
                        UIView* panView = gesture.view;
                        CGPoint translation = [gesture translationInView:panView.superview];
                        
                        if (gesture.state == UIGestureRecognizerStateBegan || 
                            gesture.state == UIGestureRecognizerStateChanged) {
                            panView.center = CGPointMake(panView.center.x + translation.x, 
                                                        panView.center.y + translation.y);
                            [gesture setTranslation:CGPointZero inView:panView.superview];
                        }
                    };
                    
                    // Use objc_setAssociatedObject to associate the block with the selector
                    // This technique is used because we can't easily create Obj-C methods from C++
                    #pragma clang diagnostic push
                    #pragma clang diagnostic ignored "-Wundeclared-selector"
                    class_addMethod([view class], @selector(handlePan:), imp_implementationWithBlock(panHandler), "v@:@");
                    #pragma clang diagnostic pop
                }
            }
        });
    }
    
    // Check if UI is draggable
    bool UIController::IsDraggable() const {
        return m_isDraggable;
    }
    
    // Set script content in editor
    void UIController::SetScriptContent(const std::string& script) {
        m_currentScript = script;
        
        // Update the script editor UI
        dispatch_async(dispatch_get_main_queue(), ^{
            if (m_uiView) {
                UIView* view = (__bridge UIView*)m_uiView;
                UITextView* scriptTextView = [view viewWithTag:2000];
                
                if ([scriptTextView isKindOfClass:[UITextView class]]) {
                    scriptTextView.text = [NSString stringWithUTF8String:script.c_str()];
                }
            }
        });
    }
    
    // Get script content from editor
    std::string iOS::UIController::GetScriptContent() const {
        __block std::string content = m_currentScript;
        
        // Retrieve content from UI on main thread synchronously
        dispatch_sync(dispatch_get_main_queue(), ^{
            if (m_uiView) {
                UIView* view = (__bridge UIView*)m_uiView;
                UITextView* scriptTextView = [view viewWithTag:2000];
                
                if ([scriptTextView isKindOfClass:[UITextView class]]) {
                    content = [scriptTextView.text UTF8String];
                }
            }
        });
        
        return content;
    }
    
    // Execute current script in editor
    bool iOS::UIController::ExecuteCurrentScript() {
        // Get the current script content
        std::string script = GetScriptContent();
        
        // Call the execute callback
        bool success = m_executeCallback(script);
        
        // Log to console
        if (success) {
            AppendToConsole("Script executed successfully.");
        } else {
            AppendToConsole("Script execution failed.");
        }
        
        return success;
    }
    
    // Save current script in editor
    bool iOS::UIController::SaveCurrentScript(const std::string& name) {
        // Get the current script content
        std::string script = GetScriptContent();
        
        // Generate a name if not provided
        std::string scriptName = name;
        if (scriptName.empty()) {
            // Generate name based on current timestamp
            auto now = std::chrono::system_clock::now();
            auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(
                now.time_since_epoch()).count();
            scriptName = "Script_" + std::to_string(timestamp);
        }
        
        // Create script info
        ScriptInfo scriptInfo(scriptName, script, std::chrono::system_clock::now().time_since_epoch().count());
        
        // Call the save callback
        bool success = m_saveScriptCallback(scriptInfo);
        
        if (success) {
            // Refresh the scripts list
            RefreshScriptsList();
            AppendToConsole("Script saved: " + scriptName);
        } else {
            AppendToConsole("Failed to save script: " + scriptName);
        }
        
        return success;
    }
    
    // Load a script into the editor
    bool iOS::UIController::LoadScript(const iOS::UIController::ScriptInfo& scriptInfo) {
        // Set the script content
        SetScriptContent(scriptInfo.m_content);
        
        // Ensure editor tab is active
        SwitchTab(TabType::Editor);
        
        AppendToConsole("Loaded script: " + scriptInfo.m_name);
        
        return true;
    }
    
    // Delete a saved script
    bool iOS::UIController::DeleteScript(const std::string& name) {
        bool success = false;
        
        // Find and remove the script from the saved scripts list
        auto it = std::find_if(m_savedScripts.begin(), m_savedScripts.end(),
                             [&name](const ScriptInfo& info) {
                                 return info.m_name == name;
                             });
        
        if (it != m_savedScripts.end()) {
            m_savedScripts.erase(it);
            success = true;
            
            // Update the UI list
            RefreshScriptsList();
            AppendToConsole("Deleted script: " + name);
        } else {
            AppendToConsole("Script not found: " + name);
        }
        
        return success;
    }
    
    // Clear the console
    void iOS::UIController::ClearConsole() {
        m_consoleText.clear();
        
        // Update the console UI
        dispatch_async(dispatch_get_main_queue(), ^{
            if (m_uiView) {
                UIView* view = (__bridge UIView*)m_uiView;
                UITextView* consoleTextView = [view viewWithTag:3000];
                
                if ([consoleTextView isKindOfClass:[UITextView class]]) {
                    consoleTextView.text = @"";
                }
            }
        });
    }
    
    // Get console text
    std::string iOS::UIController::GetConsoleText() const {
        return m_consoleText;
    }
    
    // Set execute callback
    void iOS::UIController::SetExecuteCallback(iOS::UIController::ExecuteCallback callback) {
        if (callback) {
            m_executeCallback = callback;
        }
    }
    
    // Set save script callback
    void iOS::UIController::SetSaveScriptCallback(iOS::UIController::SaveScriptCallback callback) {
        if (callback) {
            m_saveScriptCallback = callback;
        }
    }
    
    // Set load scripts callback
    void iOS::UIController::SetLoadScriptsCallback(iOS::UIController::LoadScriptsCallback callback) {
        if (callback) {
            m_loadScriptsCallback = callback;
        }
    }
    
    // Check if button is visible
    bool iOS::UIController::IsButtonVisible() const {
        return m_floatingButton && m_floatingButton->IsVisible();
    }
    
    // Show/hide floating button
    void iOS::UIController::SetButtonVisible(bool visible) {
        if (m_floatingButton) {
            if (visible) {
                m_floatingButton->Show();
            } else {
                m_floatingButton->Hide();
            }
        }
    }
    
    // Private method implementations
    
    void iOS::UIController::CreateUI() {
        // Ensure we're on the main thread for UI operations
        dispatch_async(dispatch_get_main_queue(), ^{
            // Get the key window
            UIWindow* keyWindow = nil;
            NSArray* windows = [[UIApplication sharedApplication] windows];
            for (UIWindow* window in windows) {
                if (window.isKeyWindow) {
                    keyWindow = window;
                    break;
                }
            }
            
            if (!keyWindow) {
                // Fallback to the first window if no key window
                keyWindow = [windows firstObject];
            }
            
            if (!keyWindow) {
                std::cerr << "Error: No window found to attach UI to" << std::endl;
                return;
            }
            
            // Create main container view with visual effect (blur)
            UIVisualEffectView* containerView = [[UIVisualEffectView alloc] 
                                               initWithEffect:[UIBlurEffect effectWithStyle:UIBlurEffectStyleDark]];
            containerView.frame = CGRectMake(20, 60, keyWindow.bounds.size.width - 40, 
                                           keyWindow.bounds.size.height - 120);
            containerView.layer.cornerRadius = 16.0;
            containerView.layer.masksToBounds = YES;
            containerView.alpha = m_opacity;
            containerView.hidden = !m_isVisible;
            
            // Content view for the blur effect
            UIView* contentView = containerView.contentView;
            
            // Add a subtle border
            containerView.layer.borderWidth = 1.0;
            containerView.layer.borderColor = [UIColor colorWithWhite:1.0 alpha:0.3].CGColor;
            
            // Create tab bar
            UITabBar* tabBar = [[UITabBar alloc] initWithFrame:CGRectMake(0, 0, 
                                                                        containerView.bounds.size.width, 49)];
            tabBar.tag = 1000;
            tabBar.delegate = nil; // We'll use tags to identify tabs
            
            // Add tabs
            UITabBarItem* editorTab = [[UITabBarItem alloc] initWithTitle:@"Editor" image:nil tag:0];
            UITabBarItem* scriptsTab = [[UITabBarItem alloc] initWithTitle:@"Scripts" image:nil tag:1];
            UITabBarItem* consoleTab = [[UITabBarItem alloc] initWithTitle:@"Console" image:nil tag:2];
            UITabBarItem* settingsTab = [[UITabBarItem alloc] initWithTitle:@"Settings" image:nil tag:3];
            
            tabBar.items = @[editorTab, scriptsTab, consoleTab, settingsTab];
            tabBar.selectedItem = editorTab; // Default to editor tab
            [contentView addSubview:tabBar];
            
            // Set up tab tap handler
            [tabBar addObserver:tabBar forKeyPath:@"selectedItem" options:NSKeyValueObservingOptionNew context:nil];
            
            // Define a block to handle tab bar selection
            ^{
                SEL selector = NSSelectorFromString(@"observeValueForKeyPath:ofObject:change:context:");
                IMP imp = imp_implementationWithBlock(^(id self, NSString* keyPath, id object, NSDictionary* change, void* context) {
                    if ([keyPath isEqualToString:@"selectedItem"]) {
                        UITabBarItem* selectedItem = change[NSKeyValueChangeNewKey];
                        // Find the C++ UIController instance and call SwitchTab
                        // This is a simplified approach; in a real implementation you'd have a more robust way to find the controller
                        UIView* containerView = [(UITabBar*)self superview].superview;
                        UIViewController* rootVC = nil;
                        
                        for (UIWindow* window in [[UIApplication sharedApplication] windows]) {
                            if (window.isKeyWindow) {
                                rootVC = window.rootViewController;
                                break;
                            }
                        }
                        
                        if (rootVC) {
                            // This approach is simplified; in a real implementation you'd have proper associations
                            // between UI components and C++ objects
                            iOS::UIController* controller = (__bridge iOS::UIController*)(void*)objc_getAssociatedObject(rootVC, "UIControllerInstance");
                            if (controller) {
                                iOS::UIController::TabType tabType = iOS::UIController::TabType::Editor;
                                switch (selectedItem.tag) {
                                    case 0: tabType = iOS::UIController::TabType::Editor; break;
                                    case 1: tabType = iOS::UIController::TabType::Scripts; break;
                                    case 2: tabType = iOS::UIController::TabType::Console; break;
                                    case 3: tabType = iOS::UIController::TabType::Settings; break;
                                }
                                controller->SwitchTab(tabType);
                            }
                        }
                    }
                });
                
                class_replaceMethod([tabBar class], 
                                 NSSelectorFromString(@"observeValueForKeyPath:ofObject:change:context:"),
                                 imp, 
                                 "v@:@@@@");
            }();
            
            // Create content views for each tab
            
            // 1. Editor view
            UIView* editorView = [[UIView alloc] initWithFrame:CGRectMake(0, 50, 
                                                                        containerView.bounds.size.width,
                                                                        containerView.bounds.size.height - 50)];
            editorView.tag = 1001;
            editorView.backgroundColor = [UIColor clearColor];
            [contentView addSubview:editorView];
            
            // Script editor text view
            UITextView* scriptTextView = [[UITextView alloc] initWithFrame:CGRectMake(10, 10, 
                                                                                  editorView.bounds.size.width - 20,
                                                                                  editorView.bounds.size.height - 70)];
            scriptTextView.tag = 2000;
            scriptTextView.font = [UIFont fontWithName:@"Menlo" size:14.0];
            scriptTextView.backgroundColor = [UIColor colorWithWhite:0.1 alpha:0.5];
            scriptTextView.textColor = [UIColor whiteColor];
            scriptTextView.autocorrectionType = UITextAutocorrectionTypeNo;
            scriptTextView.autocapitalizationType = UITextAutocapitalizationTypeNone;
            scriptTextView.text = [NSString stringWithUTF8String:m_currentScript.c_str()];
            scriptTextView.layer.cornerRadius = 8.0;
            scriptTextView.layer.masksToBounds = YES;
            [editorView addSubview:scriptTextView];
            
            // Execute button
            UIButton* executeButton = [UIButton buttonWithType:UIButtonTypeSystem];
            executeButton.frame = CGRectMake(editorView.bounds.size.width - 100, 
                                          editorView.bounds.size.height - 50,
                                          90, 40);
            [executeButton setTitle:@"Execute" forState:UIControlStateNormal];
            executeButton.backgroundColor = [UIColor colorWithRed:0.2 green:0.6 blue:1.0 alpha:0.7];
            executeButton.layer.cornerRadius = 8.0;
            [executeButton setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
            [editorView addSubview:executeButton];
            
            // Save button
            UIButton* saveButton = [UIButton buttonWithType:UIButtonTypeSystem];
            saveButton.frame = CGRectMake(editorView.bounds.size.width - 200, 
                                        editorView.bounds.size.height - 50,
                                        90, 40);
            [saveButton setTitle:@"Save" forState:UIControlStateNormal];
            saveButton.backgroundColor = [UIColor colorWithRed:0.2 green:0.8 blue:0.2 alpha:0.7];
            saveButton.layer.cornerRadius = 8.0;
            [saveButton setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
            [editorView addSubview:saveButton];
            
            // Set up execute and save button actions
            [executeButton addTarget:nil action:NSSelectorFromString(@"executeButtonTapped:") forControlEvents:UIControlEventTouchUpInside];
            [saveButton addTarget:nil action:NSSelectorFromString(@"saveButtonTapped:") forControlEvents:UIControlEventTouchUpInside];
            
            // 2. Scripts view
            UIView* scriptsView = [[UIView alloc] initWithFrame:CGRectMake(0, 50, 
                                                                         containerView.bounds.size.width,
                                                                         containerView.bounds.size.height - 50)];
            scriptsView.tag = 1002;
            scriptsView.backgroundColor = [UIColor clearColor];
            scriptsView.hidden = YES;
            [contentView addSubview:scriptsView];
            
            // Table view for scripts
            UITableView* scriptsTableView = [[UITableView alloc] initWithFrame:CGRectMake(10, 10, 
                                                                                     scriptsView.bounds.size.width - 20,
                                                                                     scriptsView.bounds.size.height - 20)
                                                                         style:UITableViewStylePlain];
            scriptsTableView.tag = 2100;
            scriptsTableView.backgroundColor = [UIColor colorWithWhite:0.1 alpha:0.5];
            scriptsTableView.delegate = nil;
            scriptsTableView.dataSource = nil;
            scriptsTableView.layer.cornerRadius = 8.0;
            scriptsTableView.layer.masksToBounds = YES;
            [scriptsView addSubview:scriptsTableView];
            
            // 3. Console view
            UIView* consoleView = [[UIView alloc] initWithFrame:CGRectMake(0, 50, 
                                                                         containerView.bounds.size.width,
                                                                         containerView.bounds.size.height - 50)];
            consoleView.tag = 1003;
            consoleView.backgroundColor = [UIColor clearColor];
            consoleView.hidden = YES;
            [contentView addSubview:consoleView];
            
            // Console text view
            UITextView* consoleTextView = [[UITextView alloc] initWithFrame:CGRectMake(10, 10, 
                                                                                  consoleView.bounds.size.width - 20,
                                                                                  consoleView.bounds.size.height - 70)];
            consoleTextView.tag = 3000;
            consoleTextView.font = [UIFont fontWithName:@"Menlo" size:12.0];
            consoleTextView.backgroundColor = [UIColor colorWithWhite:0.1 alpha:0.5];
            consoleTextView.textColor = [UIColor whiteColor];
            consoleTextView.editable = NO;
            consoleTextView.text = [NSString stringWithUTF8String:m_consoleText.c_str()];
            consoleTextView.layer.cornerRadius = 8.0;
            consoleTextView.layer.masksToBounds = YES;
            [consoleView addSubview:consoleTextView];
            
            // Clear console button
            UIButton* clearButton = [UIButton buttonWithType:UIButtonTypeSystem];
            clearButton.frame = CGRectMake(10, consoleView.bounds.size.height - 50, 90, 40);
            [clearButton setTitle:@"Clear" forState:UIControlStateNormal];
            clearButton.backgroundColor = [UIColor colorWithRed:0.8 green:0.2 blue:0.2 alpha:0.7];
            clearButton.layer.cornerRadius = 8.0;
            [clearButton setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
            [consoleView addSubview:clearButton];
            
            // Set up clear button action
            [clearButton addTarget:nil action:NSSelectorFromString(@"clearButtonTapped:") forControlEvents:UIControlEventTouchUpInside];
            
            // 4. Settings view
            UIView* settingsView = [[UIView alloc] initWithFrame:CGRectMake(0, 50, 
                                                                          containerView.bounds.size.width,
                                                                          containerView.bounds.size.height - 50)];
            settingsView.tag = 1004;
            settingsView.backgroundColor = [UIColor clearColor];
            settingsView.hidden = YES;
            [contentView addSubview:settingsView];
            
            // Settings options
            UIView* settingsContainer = [[UIView alloc] initWithFrame:CGRectMake(10, 10, 
                                                                              settingsView.bounds.size.width - 20,
                                                                              settingsView.bounds.size.height - 20)];
            settingsContainer.backgroundColor = [UIColor colorWithWhite:0.1 alpha:0.5];
            settingsContainer.layer.cornerRadius = 8.0;
            settingsContainer.layer.masksToBounds = YES;
            [settingsView addSubview:settingsContainer];
            
            // Add our UI view to the key window
            [keyWindow addSubview:containerView];
            
            // Store the UI view for later use
            m_uiView = (__bridge_retained void*)containerView;
        });
    }
    
    void iOS::UIController::UpdateLayout() {
        dispatch_async(dispatch_get_main_queue(), ^{
            // Update the UI layout based on the current state
        });
    }
    
    void iOS::UIController::SaveUIState() {
        // Save UI state (position, opacity, visibility) to user defaults
        dispatch_async(dispatch_get_main_queue(), ^{
            if (m_uiView) {
                UIView* view = (__bridge UIView*)m_uiView;
                NSUserDefaults* defaults = [NSUserDefaults standardUserDefaults];
                
                // Store position
                [defaults setFloat:view.frame.origin.x forKey:@"UIControllerPositionX"];
                [defaults setFloat:view.frame.origin.y forKey:@"UIControllerPositionY"];
                
                // Store opacity
                [defaults setFloat:m_opacity forKey:@"UIControllerOpacity"];
                
                // Store visibility
                [defaults setBool:m_isVisible forKey:@"UIControllerVisible"];
                
                // Store current tab
                [defaults setInteger:(NSInteger)m_currentTab forKey:@"UIControllerCurrentTab"];
                
                [defaults synchronize];
            }
        });
    }
    
    void iOS::UIController::LoadUIState() {
        // Load UI state from user defaults
        dispatch_async(dispatch_get_main_queue(), ^{
            if (m_uiView) {
                UIView* view = (__bridge UIView*)m_uiView;
                NSUserDefaults* defaults = [NSUserDefaults standardUserDefaults];
                
                // Load position if available
                if ([defaults objectForKey:@"UIControllerPositionX"] && [defaults objectForKey:@"UIControllerPositionY"]) {
                    CGFloat x = [defaults floatForKey:@"UIControllerPositionX"];
                    CGFloat y = [defaults floatForKey:@"UIControllerPositionY"];
                    CGRect frame = view.frame;
                    frame.origin.x = x;
                    frame.origin.y = y;
                    view.frame = frame;
                }
                
                // Load opacity if available
                if ([defaults objectForKey:@"UIControllerOpacity"]) {
                    float opacity = [defaults floatForKey:@"UIControllerOpacity"];
                    SetOpacity(opacity);
                }
                
                // Load visibility if available
                if ([defaults objectForKey:@"UIControllerVisible"]) {
                    bool visible = [defaults boolForKey:@"UIControllerVisible"];
                    if (visible) {
                        Show();
                    } else {
                        Hide();
                    }
                }
                
                // Load current tab if available
                if ([defaults objectForKey:@"UIControllerCurrentTab"]) {
                    NSInteger tabIndex = [defaults integerForKey:@"UIControllerCurrentTab"];
                    SwitchTab(static_cast<TabType>(tabIndex));
                }
            }
        });
    }
    
    void iOS::UIController::RefreshScriptsList() {
        // Load scripts using the callback
        m_savedScripts = m_loadScriptsCallback();
        
        // Update the scripts table view
        dispatch_async(dispatch_get_main_queue(), ^{
            if (m_uiView) {
                UIView* view = (__bridge UIView*)m_uiView;
                UITableView* scriptsTableView = [view viewWithTag:2100];
                
                if ([scriptsTableView isKindOfClass:[UITableView class]]) {
                    // Create a data source and delegate for the table view
                    // This is done using Objective-C runtime because we can't use protocols in C++
                    
                    // Create a class to handle data source and delegate methods
                    static Class TableHandlerClass = nil;
                    static std::vector<iOS::UIController::ScriptInfo>* scriptsPtr = nullptr;
                    static void* controllerPtr = nullptr;
                    
                    // Store references to the scripts and controller
                    scriptsPtr = &m_savedScripts;
                    controllerPtr = (__bridge void*)this;
                    
                    // Create the class dynamically if it doesn't exist
                    if (!TableHandlerClass) {
                        TableHandlerClass = objc_allocateClassPair([NSObject class], "ScriptsTableHandler", 0);
                        
                        // Add protocol conformance
                        class_addProtocol(TableHandlerClass, @protocol(UITableViewDataSource));
                        class_addProtocol(TableHandlerClass, @protocol(UITableViewDelegate));
                        
                        // Add methods for the data source protocol
                        class_addMethod(TableHandlerClass, @selector(tableView:numberOfRowsInSection:),
                                       imp_implementationWithBlock(^NSInteger(id self, UITableView* tableView, NSInteger section) {
                            return static_cast<NSInteger>(scriptsPtr->size());
                        }), "i@:@i");
                        
                        class_addMethod(TableHandlerClass, @selector(tableView:cellForRowAtIndexPath:),
                                       imp_implementationWithBlock(^UITableViewCell*(id self, UITableView* tableView, NSIndexPath* indexPath) {
                            static NSString* CellID = @"ScriptCell";
                            UITableViewCell* cell = [tableView dequeueReusableCellWithIdentifier:CellID];
                            
                            if (!cell) {
                                cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleSubtitle reuseIdentifier:CellID];
                                cell.backgroundColor = [UIColor clearColor];
                                cell.textLabel.textColor = [UIColor whiteColor];
                                cell.detailTextLabel.textColor = [UIColor lightGrayColor];
                                
                                // Add load button
                                UIButton* loadButton = [UIButton buttonWithType:UIButtonTypeSystem];
                                loadButton.frame = CGRectMake(0, 0, 60, 30);
                                loadButton.backgroundColor = [UIColor colorWithRed:0.2 green:0.2 blue:0.8 alpha:0.7];
                                loadButton.layer.cornerRadius = 5.0;
                                [loadButton setTitle:@"Load" forState:UIControlStateNormal];
                                [loadButton setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
                                cell.accessoryView = loadButton;
                                
                                // Set up the button action
                                [loadButton addTarget:self action:@selector(loadScript:) forControlEvents:UIControlEventTouchUpInside];
                            }
                            
                            // Configure the cell
                            NSUInteger index = static_cast<NSUInteger>(indexPath.row);
                            if (index < scriptsPtr->size()) {
                                const auto& script = (*scriptsPtr)[index];
                                cell.textLabel.text = [NSString stringWithUTF8String:script.m_name.c_str()];
                                
                                // Format the timestamp
                                NSDate* date = [NSDate dateWithTimeIntervalSince1970:script.m_timestamp / 1000.0];
                                NSDateFormatter* formatter = [[NSDateFormatter alloc] init];
                                formatter.dateStyle = NSDateFormatterShortStyle;
                                formatter.timeStyle = NSDateFormatterShortStyle;
                                NSString* dateStr = [formatter stringFromDate:date];
                                cell.detailTextLabel.text = dateStr;
                                
                                // Store the script index in the button's tag
                                UIButton* loadButton = (UIButton*)cell.accessoryView;
                                loadButton.tag = index;
                            }
                            
                            return cell;
                        }), "@@:@@");
                        
                        // Add method for the load button action
                        class_addMethod(TableHandlerClass, @selector(loadScript:),
                                       imp_implementationWithBlock(^(id self, UIButton* sender) {
                            NSUInteger index = sender.tag;
                            if (index < scriptsPtr->size()) {
                                iOS::UIController* controller = (__bridge iOS::UIController*)controllerPtr;
                                controller->LoadScript((*scriptsPtr)[index]);
                            }
                        }), "v@:@");
                        
                        // Add method for row deletion
                        class_addMethod(TableHandlerClass, @selector(tableView:canEditRowAtIndexPath:),
                                       imp_implementationWithBlock(^BOOL(id self, UITableView* tableView, NSIndexPath* indexPath) {
                            return YES;
                        }), "B@:@@");
                        
                        class_addMethod(TableHandlerClass, @selector(tableView:commitEditingStyle:forRowAtIndexPath:),
                                       imp_implementationWithBlock(^(id self, UITableView* tableView, UITableViewCellEditingStyle editingStyle, NSIndexPath* indexPath) {
                            if (editingStyle == UITableViewCellEditingStyleDelete) {
                                NSUInteger index = static_cast<NSUInteger>(indexPath.row);
                                if (index < scriptsPtr->size()) {
                                    iOS::UIController* controller = (__bridge iOS::UIController*)controllerPtr;
                                    controller->DeleteScript((*scriptsPtr)[index].m_name);
                                    // Table view will be refreshed by DeleteScript
                                }
                            }
                        }), "v@:@i@");
                        
                        // Register the class
                        objc_registerClassPair(TableHandlerClass);
                    }
                    
                    // Create the handler instance
                    id handler = [[TableHandlerClass alloc] init];
                    
                    // Set the delegate and data source
                    scriptsTableView.delegate = handler;
                    scriptsTableView.dataSource = handler;
                    
                    // Reload the table view
                    [scriptsTableView reloadData];
                }
            }
        });
    }
    
    void iOS::UIController::AppendToConsole(const std::string& text) {
        // Add the text to the console with a timestamp
        auto now = std::chrono::system_clock::now();
        auto nowTime = std::chrono::system_clock::to_time_t(now);
        std::string timestamp = std::ctime(&nowTime);
        timestamp.pop_back(); // Remove trailing newline
        
        std::string logEntry = "[" + timestamp + "] " + text + "\n";
        m_consoleText += logEntry;
        
        // Update the console UI
        dispatch_async(dispatch_get_main_queue(), ^{
            if (m_uiView) {
                UIView* view = (__bridge UIView*)m_uiView;
                UITextView* consoleTextView = [view viewWithTag:3000];
                
                if ([consoleTextView isKindOfClass:[UITextView class]]) {
                    NSString* currentText = consoleTextView.text;
                    NSString* newEntry = [NSString stringWithUTF8String:logEntry.c_str()];
                    consoleTextView.text = [currentText stringByAppendingString:newEntry];
                    
                    // Scroll to the bottom
                    NSRange range = NSMakeRange(consoleTextView.text.length, 0);
                    [consoleTextView scrollRangeToVisible:range];
                }
            }
        });
    }
} // namespace iOS
                                                                          containerView.bounds.size.width,
                                                                          containerView.bounds.size.height - 50)];
            settingsView.tag = 1004;
            settingsView.backgroundColor = [UIColor clearColor];
            settingsView.hidden = YES;
            [contentView addSubview:settingsView];
            
            // Add settings UI elements (simplified)
            UILabel* opacityLabel = [[UILabel alloc] initWithFrame:CGRectMake(20, 20, 100, 30)];
            opacityLabel.text = @"Opacity:";
            opacityLabel.textColor = [UIColor whiteColor];
            [settingsView addSubview:opacityLabel];
            
            UISlider* opacitySlider = [[UISlider alloc] initWithFrame:CGRectMake(130, 20, 
                                                                              settingsView.bounds.size.width - 150, 30)];
            opacitySlider.tag = 4000;
            opacitySlider.minimumValue = 0.1;
            opacitySlider.maximumValue = 1.0;
            opacitySlider.value = m_opacity;
            [settingsView addSubview:opacitySlider];
            
            // Draggable switch
            UILabel* draggableLabel = [[UILabel alloc] initWithFrame:CGRectMake(20, 70, 100, 30)];
            draggableLabel.text = @"Draggable:";
            draggableLabel.textColor = [UIColor whiteColor];
            [settingsView addSubview:draggableLabel];
            
            UISwitch* draggableSwitch = [[UISwitch alloc] initWithFrame:CGRectMake(130, 70, 51, 31)];
            draggableSwitch.tag = 4001;
            draggableSwitch.on = m_isDraggable;
            [settingsView addSubview:draggableSwitch];
            
            // Button visibility switch
            UILabel* buttonLabel = [[UILabel alloc] initWithFrame:CGRectMake(20, 120, 100, 30)];
            buttonLabel.text = @"Button:";
            buttonLabel.textColor = [UIColor whiteColor];
            [settingsView addSubview:buttonLabel];
            
            UISwitch* buttonSwitch = [[UISwitch alloc] initWithFrame:CGRectMake(130, 120, 51, 31)];
            buttonSwitch.tag = 4002;
            buttonSwitch.on = IsButtonVisible();
            [settingsView addSubview:buttonSwitch];
            
            // Set up settings controls actions
            [opacitySlider addTarget:nil action:NSSelectorFromString(@"opacitySliderChanged:") forControlEvents:UIControlEventValueChanged];
            [draggableSwitch addTarget:nil action:NSSelectorFromString(@"draggableSwitchChanged:") forControlEvents:UIControlEventValueChanged];
            [buttonSwitch addTarget:nil action:NSSelectorFromString(@"buttonSwitchChanged:") forControlEvents:UIControlEventValueChanged];
            
            // Implement action handlers
            ^{
                // Execute button action
                SEL executeSelector = NSSelectorFromString(@"executeButtonTapped:");
                IMP executeImp = imp_implementationWithBlock(^(id self, UIButton* sender) {
                    UIViewController* rootVC = nil;
                    for (UIWindow* window in [[UIApplication sharedApplication] windows]) {
                        if (window.isKeyWindow) {
                            rootVC = window.rootViewController;
                            break;
                        }
                    }
                    
                    if (rootVC) {
                        UIController* controller = (__bridge UIController*)(void*)objc_getAssociatedObject(rootVC, "UIControllerInstance");
                        if (controller) {
                            controller->ExecuteCurrentScript();
                        }
                    }
                });
                class_addMethod([executeButton class], executeSelector, executeImp, "v@:@");
                
                // Save button action
                SEL saveSelector = NSSelectorFromString(@"saveButtonTapped:");
                IMP saveImp = imp_implementationWithBlock(^(id self, UIButton* sender) {
                    UIViewController* rootVC = nil;
                    for (UIWindow* window in [[UIApplication sharedApplication] windows]) {
                        if (window.isKeyWindow) {
                            rootVC = window.rootViewController;
                            break;
                        }
                    }
                    
                    if (rootVC) {
                        UIController* controller = (__bridge UIController*)(void*)objc_getAssociatedObject(rootVC, "UIControllerInstance");
                        if (controller) {
                            // Show alert to get script name
                            UIAlertController* alertController = [UIAlertController 
                                                              alertControllerWithTitle:@"Save Script"
                                                              message:@"Enter a name for the script:"
                                                              preferredStyle:UIAlertControllerStyleAlert];
                            
                            [alertController addTextFieldWithConfigurationHandler:^(UITextField* textField) {
                                textField.placeholder = @"Script name";
                            }];
                            
                            UIAlertAction* saveAction = [UIAlertAction 
                                                     actionWithTitle:@"Save"
                                                     style:UIAlertActionStyleDefault
                                                     handler:^(UIAlertAction* action) {
                                NSString* scriptName = alertController.textFields.firstObject.text;
                                controller->SaveCurrentScript([scriptName UTF8String]);
                            }];
                            
                            UIAlertAction* cancelAction = [UIAlertAction 
                                                       actionWithTitle:@"Cancel"
                                                       style:UIAlertActionStyleCancel
                                                       handler:nil];
                            
                            [alertController addAction:saveAction];
                            [alertController addAction:cancelAction];
                            
                            [rootVC presentViewController:alertController animated:YES completion:nil];
                        }
                    }
                });
                class_addMethod([saveButton class], saveSelector, saveImp, "v@:@");
                
                // Clear button action
                SEL clearSelector = NSSelectorFromString(@"clearButtonTapped:");
                IMP clearImp = imp_implementationWithBlock(^(id self, UIButton* sender) {
                    UIViewController* rootVC = nil;
                    for (UIWindow* window in [[UIApplication sharedApplication] windows]) {
                        if (window.isKeyWindow) {
                            rootVC = window.rootViewController;
                            break;
                        }
                    }
                    
                    if (rootVC) {
                        UIController* controller = (__bridge UIController*)(void*)objc_getAssociatedObject(rootVC, "UIControllerInstance");
                        if (controller) {
                            controller->ClearConsole();
                        }
                    }
                });
                class_addMethod([clearButton class], clearSelector, clearImp, "v@:@");
                
                // Opacity slider action
                SEL opacitySelector = NSSelectorFromString(@"opacitySliderChanged:");
                IMP opacityImp = imp_implementationWithBlock(^(id self, UISlider* sender) {
                    UIViewController* rootVC = nil;
                    for (UIWindow* window in [[UIApplication sharedApplication] windows]) {
                        if (window.isKeyWindow) {
                            rootVC = window.rootViewController;
                            break;
                        }
                    }
                    
                    if (rootVC) {
                        UIController* controller = (__bridge UIController*)(void*)objc_getAssociatedObject(rootVC, "UIControllerInstance");
                        if (controller) {
                            controller->SetOpacity(sender.value);
                        }
                    }
                });
                class_addMethod([opacitySlider class], opacitySelector, opacityImp, "v@:@");
                
                // Draggable switch action
                SEL draggableSelector = NSSelectorFromString(@"draggableSwitchChanged:");
                IMP draggableImp = imp_implementationWithBlock(^(id self, UISwitch* sender) {
                    UIViewController* rootVC = nil;
                    for (UIWindow* window in [[UIApplication sharedApplication] windows]) {
                        if (window.isKeyWindow) {
                            rootVC = window.rootViewController;
                            break;
                        }
                    }
                    
                    if (rootVC) {
                        UIController* controller = (__bridge UIController*)(void*)objc_getAssociatedObject(rootVC, "UIControllerInstance");
                        if (controller) {
                            controller->SetDraggable(sender.isOn);
                        }
                    }
                });
                class_addMethod([draggableSwitch class], draggableSelector, draggableImp, "v@:@");
                
                // Button switch action
                SEL buttonSelector = NSSelectorFromString(@"buttonSwitchChanged:");
                IMP buttonImp = imp_implementationWithBlock(^(id self, UISwitch* sender) {
                    UIViewController* rootVC = nil;
                    for (UIWindow* window in [[UIApplication sharedApplication] windows]) {
                        if (window.isKeyWindow) {
                            rootVC = window.rootViewController;
                            break;
                        }
                    }
                    
                    if (rootVC) {
                        UIController* controller = (__bridge UIController*)(void*)objc_getAssociatedObject(rootVC, "UIControllerInstance");
                        if (controller) {
                            controller->SetButtonVisible(sender.isOn);
                        }
                    }
                });
                class_addMethod([buttonSwitch class], buttonSelector, buttonImp, "v@:@");
            }();
            
            // Set up dragging behavior for the container
            UIPanGestureRecognizer* panGesture = [[UIPanGestureRecognizer alloc] 
                                               initWithTarget:nil 
                                               action:NSSelectorFromString(@"handleContainerPan:")];
            [containerView addGestureRecognizer:panGesture];
            
            // Implement pan gesture handler
            ^{
                SEL panSelector = NSSelectorFromString(@"handleContainerPan:");
                IMP panImp = imp_implementationWithBlock(^(id self, UIPanGestureRecognizer* gesture) {
                    UIView* panView = gesture.view;
                    CGPoint translation = [gesture translationInView:panView.superview];
                    
                    if (gesture.state == UIGestureRecognizerStateBegan || 
                        gesture.state == UIGestureRecognizerStateChanged) {
                        panView.center = CGPointMake(panView.center.x + translation.x, 
                                                   panView.center.y + translation.y);
                        [gesture setTranslation:CGPointZero inView:panView.superview];
                    }
                });
                class_addMethod([containerView class], panSelector, panImp, "v@:@");
            }();
            
            // Enable or disable pan gesture based on draggability
            panGesture.enabled = m_isDraggable;
            
            // Add the container view to the key window
            [keyWindow addSubview:containerView];
            
            // Store the UI view
            m_uiView = (__bridge_retained void*)containerView;
            
            // Set up scripts table view delegate and data source
            // In a real implementation, you'd create proper delegate classes
            
            // Register the UIController instance with the root view controller for later access
            UIViewController* rootVC = keyWindow.rootViewController;
            if (rootVC) {
                // This approach is simplified; in a real implementation you'd use a proper association method
                objc_setAssociatedObject(rootVC, "UIControllerInstance", (__bridge id)self, OBJC_ASSOCIATION_ASSIGN);
            }
        });
    }
    
    void UIController::UpdateLayout() {
        // Implementation to adjust layout based on current state
    }
    
    void UIController::SaveUIState() {
        // Save UI state to user defaults
        NSUserDefaults* defaults = [NSUserDefaults standardUserDefaults];
        [defaults setFloat:m_opacity forKey:@"UIController_Opacity"];
        [defaults setBool:m_isDraggable forKey:@"UIController_Draggable"];
        [defaults setBool:IsButtonVisible() forKey:@"UIController_ButtonVisible"];
        [defaults setInteger:(NSInteger)m_currentTab forKey:@"UIController_CurrentTab"];
        [defaults synchronize];
    }
    
    void UIController::LoadUIState() {
        // Load UI state from user defaults
        NSUserDefaults* defaults = [NSUserDefaults standardUserDefaults];
        
        // Load opacity
        if ([defaults objectForKey:@"UIController_Opacity"]) {
            SetOpacity([defaults floatForKey:@"UIController_Opacity"]);
        }
        
        // Load draggable state
        if ([defaults objectForKey:@"UIController_Draggable"]) {
            SetDraggable([defaults boolForKey:@"UIController_Draggable"]);
        }
        
        // Load button visibility
        if ([defaults objectForKey:@"UIController_ButtonVisible"]) {
            SetButtonVisible([defaults boolForKey:@"UIController_ButtonVisible"]);
        }
        
        // Load current tab
        if ([defaults objectForKey:@"UIController_CurrentTab"]) {
            TabType tab = (TabType)[defaults integerForKey:@"UIController_CurrentTab"];
            m_currentTab = tab; // Set directly to avoid layout changes before UI is created
        }
    }
    
    void UIController::RefreshScriptsList() {
        // Get the list of saved scripts from the callback
        if (m_loadScriptsCallback) {
            m_savedScripts = m_loadScriptsCallback();
        }
        
        // Update the UI with the scripts list
        dispatch_async(dispatch_get_main_queue(), ^{
            if (m_uiView) {
                UIView* view = (__bridge UIView*)m_uiView;
                UITableView* scriptsTableView = [view viewWithTag:2100];
                
                if ([scriptsTableView isKindOfClass:[UITableView class]]) {
                    // Set up table view delegate and data source
                    
                    // Using associated objects to store the scripts data
                    NSMutableArray* scripts = [NSMutableArray array];
                    for (const auto& script : m_savedScripts) {
                        NSString* name = [NSString stringWithUTF8String:script.m_name.c_str()];
                        NSString* content = [NSString stringWithUTF8String:script.m_content.c_str()];
                        NSTimeInterval timestamp = script.m_timestamp / 1000.0; // Convert to seconds
                        
                        NSDictionary* scriptDict = @{
                            @"name": name,
                            @"content": content,
                            @"timestamp": @(timestamp)
                        };
                        
                        [scripts addObject:scriptDict];
                    }
                    
                    // Set up data source
                    objc_setAssociatedObject(scriptsTableView, "ScriptsData", scripts, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
                    
                    // Set up delegate and data source
                    if (!scriptsTableView.delegate) {
                        // Create conforming protocol class
                        Class TableDelegate = objc_allocateClassPair([NSObject class], "ScriptsTableDelegate", 0);
                        
                        // Add protocol conformance
                        class_addProtocol(TableDelegate, @protocol(UITableViewDelegate));
                        class_addProtocol(TableDelegate, @protocol(UITableViewDataSource));
                        
                        // Add methods
                        class_addMethod(TableDelegate, @selector(tableView:numberOfRowsInSection:), imp_implementationWithBlock(^(id self, UITableView* tableView, NSInteger section) {
                            NSArray* scripts = objc_getAssociatedObject(tableView, "ScriptsData");
                            return (NSInteger)[scripts count];
                        }), "i@:@i");
                        
                        class_addMethod(TableDelegate, @selector(tableView:cellForRowAtIndexPath:), imp_implementationWithBlock(^(id self, UITableView* tableView, NSIndexPath* indexPath) {
                            NSArray* scripts = objc_getAssociatedObject(tableView, "ScriptsData");
                            
                            UITableViewCell* cell = [tableView dequeueReusableCellWithIdentifier:@"ScriptCell"];
                            if (!cell) {
                                cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleSubtitle reuseIdentifier:@"ScriptCell"];
                                cell.backgroundColor = [UIColor clearColor];
                                cell.textLabel.textColor = [UIColor whiteColor];
                                cell.detailTextLabel.textColor = [UIColor lightGrayColor];
                            }
                            
                            NSDictionary* script = scripts[indexPath.row];
                            cell.textLabel.text = script[@"name"];
                            
                            // Format date
                            NSDate* date = [NSDate dateWithTimeIntervalSince1970:[script[@"timestamp"] doubleValue]];
                            NSDateFormatter* formatter = [[NSDateFormatter alloc] init];
                            formatter.dateStyle = NSDateFormatterShortStyle;
                            formatter.timeStyle = NSDateFormatterShortStyle;
                            
                            cell.detailTextLabel.text = [formatter stringFromDate:date];
                            
                            return cell;
                        }), "@@:@@");
                        
                        class_addMethod(TableDelegate, @selector(tableView:didSelectRowAtIndexPath:), imp_implementationWithBlock(^(id self, UITableView* tableView, NSIndexPath* indexPath) {
                            [tableView deselectRowAtIndexPath:indexPath animated:YES];
                            
                            NSArray* scripts = objc_getAssociatedObject(tableView, "ScriptsData");
                            NSDictionary* script = scripts[indexPath.row];
                            
                            // Find the UIController instance
                            UIViewController* rootVC = nil;
                            for (UIWindow* window in [[UIApplication sharedApplication] windows]) {
                                if (window.isKeyWindow) {
                                    rootVC = window.rootViewController;
                                    break;
                                }
                            }
                            
                            if (rootVC) {
                                UIController* controller = (__bridge UIController*)(void*)objc_getAssociatedObject(rootVC, "UIControllerInstance");
                                if (controller) {
                                    // Create ScriptInfo and load script
                                    ScriptInfo scriptInfo(
                                        [script[@"name"] UTF8String],
                                        [script[@"content"] UTF8String],
                                        (int64_t)([script[@"timestamp"] doubleValue] * 1000)
                                    );
                                    
                                    controller->LoadScript(scriptInfo);
                                }
                            }
                        }), "v@:@@");
                        
                        class_addMethod(TableDelegate, @selector(tableView:canEditRowAtIndexPath:), imp_implementationWithBlock(^(id self, UITableView* tableView, NSIndexPath* indexPath) {
                            return YES;
                        }), "B@:@@");
                        
                        class_addMethod(TableDelegate, @selector(tableView:commitEditingStyle:forRowAtIndexPath:), imp_implementationWithBlock(^(id self, UITableView* tableView, UITableViewCellEditingStyle editingStyle, NSIndexPath* indexPath) {
                            if (editingStyle == UITableViewCellEditingStyleDelete) {
                                NSMutableArray* scripts = objc_getAssociatedObject(tableView, "ScriptsData");
                                NSDictionary* script = scripts[indexPath.row];
                                
                                // Find the UIController instance
                                UIViewController* rootVC = nil;
                                for (UIWindow* window in [[UIApplication sharedApplication] windows]) {
                                    if (window.isKeyWindow) {
                                        rootVC = window.rootViewController;
                                        break;
                                    }
                                }
                                
                                if (rootVC) {
                                    UIController* controller = (__bridge UIController*)(void*)objc_getAssociatedObject(rootVC, "UIControllerInstance");
                                    if (controller) {
                                        controller->DeleteScript([script[@"name"] UTF8String]);
                                    }
                                }
                            }
                        }), "v@:@i@");
                        
                        // Register class
                        objc_registerClassPair(TableDelegate);
                        
                        // Create delegate instance
                        id delegate = [[TableDelegate alloc] init];
                        
                        // Set delegate and data source
                        scriptsTableView.delegate = delegate;
                        scriptsTableView.dataSource = delegate;
                        
                        // Store delegate with the table view
                        objc_setAssociatedObject(scriptsTableView, "TableDelegate", delegate, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
                    }
                    
                    // Reload the table view
                    [scriptsTableView reloadData];
                }
            }
        });
    }
    
    void UIController::AppendToConsole(const std::string& text) {
        // Add timestamp
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::string timestamp = std::ctime(&time);
        timestamp.resize(timestamp.size() - 1); // Remove newline
        
        std::string entry = "[" + timestamp + "] " + text + "\n";
        
        // Append to console text
        m_consoleText += entry;
        
        // Update the console UI
        dispatch_async(dispatch_get_main_queue(), ^{
            if (m_uiView) {
                UIView* view = (__bridge UIView*)m_uiView;
                UITextView* consoleTextView = [view viewWithTag:3000];
                
                if ([consoleTextView isKindOfClass:[UITextView class]]) {
                    NSString* newText = [NSString stringWithUTF8String:entry.c_str()];
                    consoleTextView.text = [consoleTextView.text stringByAppendingString:newText];
                    
                    // Scroll to bottom
                    NSRange range = NSMakeRange(consoleTextView.text.length, 0);
                    [consoleTextView scrollRangeToVisible:range];
                }
            }
        });
    }
}
