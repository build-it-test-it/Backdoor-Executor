#include "AssistantButtonController.h"
#include "../ai_features/local_models/GeneralAssistantModel.h"
#include <iostream>
#include <chrono>
#include <ctime>
#include <algorithm>

// Objective-C imports
#if __OBJC__
#import <UIKit/UIKit.h>
#import <Foundation/Foundation.h>
#import <objc/runtime.h> // Added for objc_setAssociatedObject
#else
typedef void UIButton;
typedef void UIView;
typedef void UIViewController;
#endif

namespace iOS {
namespace UI {

AssistantButtonController::AssistantButtonController() 
    : m_assistantModel(nullptr),
      m_position(Position::BottomRight),
      m_visibilityState(VisibilityState::Minimized),
      m_theme(ButtonTheme::Blue),
      m_button(nullptr),
      m_panelView(nullptr),
      m_chatView(nullptr),
      m_inputField(nullptr),
      m_isDragging(false) {
    
    // Initialize safe area insets with default values
    m_safeAreaInsets[0] = 20; // top
    m_safeAreaInsets[1] = 20; // left
    m_safeAreaInsets[2] = 20; // bottom
    m_safeAreaInsets[3] = 20; // right
    
    // Initialize callbacks
    m_buttonTapCallback = [](){};
    m_visibilityChangedCallback = [](VisibilityState){};
    m_messageCallback = [](const std::string&){};
}

AssistantButtonController::~AssistantButtonController() {
    // Cleanup Objective-C objects
#if __OBJC__
    if (m_button) {
        UIButton* button = (__bridge_transfer UIButton*)m_button;
        m_button = nullptr;
    }
    
    if (m_panelView) {
        UIView* panelView = (__bridge_transfer UIView*)m_panelView;
        m_panelView = nullptr;
    }
    
    if (m_chatView) {
        UIView* chatView = (__bridge_transfer UIView*)m_chatView;
        m_chatView = nullptr;
    }
    
    if (m_inputField) {
        UIView* inputField = (__bridge_transfer UIView*)m_inputField;
        m_inputField = nullptr;
    }
#endif
}

bool AssistantButtonController::Initialize(void* viewController) {
#if __OBJC__
    UIViewController* controller = (__bridge UIViewController*)viewController;
    if (!controller) return false;
    
    // Get main view bounds
    CGRect bounds = controller.view.bounds;
    
    // Get safe area insets
    UIEdgeInsets safeAreaInsets = controller.view.safeAreaInsets;
    m_safeAreaInsets[0] = safeAreaInsets.top;
    m_safeAreaInsets[1] = safeAreaInsets.left;
    m_safeAreaInsets[2] = safeAreaInsets.bottom;
    m_safeAreaInsets[3] = safeAreaInsets.right;
    
    // Create floating button
    CreateFloatingButton(controller);
    
    // Create assistant panel
    CreateAssistantPanel(controller);
    
    // Position the button based on the current position
    PositionFloatingButton();
    
    // Register for keyboard notifications
    NSNotificationCenter* center = [NSNotificationCenter defaultCenter];
    [center addObserver:NSObject.class
               selector:@selector(keyboardWillShow:)
                   name:UIKeyboardWillShowNotification
                 object:nil];
    
    [center addObserver:NSObject.class
               selector:@selector(keyboardWillHide:)
                   name:UIKeyboardWillHideNotification
                 object:nil];
    
    return true;
#else
    return false;
#endif
}

void AssistantButtonController::Shutdown() {
#if __OBJC__
    // Unregister from keyboard notifications
    NSNotificationCenter* center = [NSNotificationCenter defaultCenter];
    [center removeObserver:NSObject.class 
                      name:UIKeyboardWillShowNotification 
                    object:nil];
    
    [center removeObserver:NSObject.class 
                      name:UIKeyboardWillHideNotification 
                    object:nil];
    
    // Clean up Objective-C objects
    if (m_button) {
        UIButton* button = (__bridge UIButton*)m_button;
        [button removeFromSuperview];
    }
    
    if (m_panelView) {
        UIView* panelView = (__bridge UIView*)m_panelView;
        [panelView removeFromSuperview];
    }
    
    if (m_chatView) {
        UIView* chatView = (__bridge UIView*)m_chatView;
        [chatView removeFromSuperview];
    }
#endif
}

void AssistantButtonController::CreateFloatingButton(void* viewController) {
#if __OBJC__
    UIViewController* controller = (__bridge UIViewController*)viewController;
    
    // Create button
    UIButton* button = [UIButton buttonWithType:UIButtonTypeCustom];
    button.frame = CGRectMake(0, 0, 60, 60);
    
    // Configure appearance
    button.layer.cornerRadius = 30;
    button.backgroundColor = [UIColor colorWithRed:0.0 green:0.5 blue:1.0 alpha:1.0];
    [button setImage:[UIImage systemImageNamed:@"wand.and.stars"] forState:UIControlStateNormal];
    button.tintColor = [UIColor whiteColor];
    
    // Add shadow
    button.layer.shadowColor = [UIColor blackColor].CGColor;
    button.layer.shadowOffset = CGSizeMake(0, 3);
    button.layer.shadowOpacity = 0.3;
    button.layer.shadowRadius = 5;
    
    // Add to the view
    [controller.view addSubview:button];
    
    // Store in member variable
    m_button = (__bridge_retained void*)button;
    
    // Add tap handler
    // Fix the block capture - need to use a pointer to 'this' instead of 'self'
    AssistantButtonController* controllerPtr = this;
    void (^buttonTapHandler)(UIButton*) = ^(UIButton* sender) {
        controllerPtr->HandleButtonTap();
    };
    
    // Store block to avoid ARC releasing it
    objc_setAssociatedObject(button, "tapHandler", buttonTapHandler, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    
    // Add target-action for button tap
    [button addTarget:buttonTapHandler 
               action:@selector(invoke:) 
     forControlEvents:UIControlEventTouchUpInside];
    
    // Add pan gesture recognizer
    UIPanGestureRecognizer* panGesture = [[UIPanGestureRecognizer alloc] init];
    
    // Add pan handler
    AssistantButtonController* controller_ptr = this; // Create local pointer to this for capture
    void (^panHandler)(UIPanGestureRecognizer*) = ^(UIPanGestureRecognizer* recognizer) {
        UIButton* button = (__bridge UIButton*)controller_ptr->m_button;
        UIViewController* viewController = (__bridge UIViewController*)viewController;
        CGRect bounds = viewController.view.bounds;
        
        if (recognizer.state == UIGestureRecognizerStateBegan) {
            controller_ptr->m_isDragging = true;
        }
        else if (recognizer.state == UIGestureRecognizerStateChanged) {
            CGPoint translation = [recognizer translationInView:button.superview];
            
            CGRect frame = button.frame;
            frame.origin.x += translation.x;
            frame.origin.y += translation.y;
            button.frame = frame;
            
            [recognizer setTranslation:CGPointZero inView:button.superview];
        }
        else if (recognizer.state == UIGestureRecognizerStateEnded) {
            controller_ptr->m_isDragging = false;
            
            // Snap to closest edge
            CGFloat minX = controller_ptr->m_safeAreaInsets[1];
            CGFloat minY = controller_ptr->m_safeAreaInsets[0];
            CGFloat maxX = bounds.size.width - button.frame.size.width - controller_ptr->m_safeAreaInsets[3];
            CGFloat maxY = bounds.size.height - button.frame.size.height - controller_ptr->m_safeAreaInsets[2];
            
            CGRect frame = button.frame;
            
            // Find horizontal and vertical position
            bool isLeft = (frame.origin.x < bounds.size.width / 2);
            bool isTop = (frame.origin.y < bounds.size.height / 2);
            
            // Snap to each corner
            if (isLeft && isTop) {
                frame.origin.x = minX;
                frame.origin.y = minY;
                controller_ptr->m_position = Position::TopLeft;
            }
            else if (isLeft && !isTop) {
                frame.origin.x = minX;
                frame.origin.y = maxY;
                controller_ptr->m_position = Position::BottomLeft;
            }
            else if (!isLeft && isTop) {
                frame.origin.x = maxX;
                frame.origin.y = minY;
                controller_ptr->m_position = Position::TopRight;
            }
            else {
                frame.origin.x = maxX;
                frame.origin.y = maxY;
                controller_ptr->m_position = Position::BottomRight;
            }
            
            // Animate to snapped position
            [UIView animateWithDuration:0.3
                             animations:^{
                                 button.frame = frame;
                             }];
        }
    };
    
    // Store block to avoid ARC releasing it
    objc_setAssociatedObject(panGesture, "panHandler", panHandler, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    
    // Add target-action for pan gesture
    [panGesture addTarget:panHandler action:@selector(invoke:)];
    [button addGestureRecognizer:panGesture];
#endif
}

void AssistantButtonController::CreateAssistantPanel(void* viewController) {
#if __OBJC__
    UIViewController* controller = (__bridge UIViewController*)viewController;
    
    // Create panel view
    UIView* panelView = [[UIView alloc] initWithFrame:CGRectMake(0, 0, 300, 400)];
    panelView.backgroundColor = [UIColor colorWithWhite:0.9 alpha:1.0];
    panelView.layer.cornerRadius = 16;
    panelView.clipsToBounds = true;
    panelView.alpha = 0.0; // Start hidden
    
    // Add shadow
    panelView.layer.shadowColor = [UIColor blackColor].CGColor;
    panelView.layer.shadowOffset = CGSizeMake(0, 5);
    panelView.layer.shadowOpacity = 0.3;
    panelView.layer.shadowRadius = 10;
    
    // Add to the view
    [controller.view addSubview:panelView];
    
    // Store in member variable
    m_panelView = (__bridge_retained void*)panelView;
    
    // Create header view
    UIView* headerView = [[UIView alloc] initWithFrame:CGRectMake(0, 0, panelView.frame.size.width, 50)];
    headerView.backgroundColor = [UIColor colorWithRed:0.0 green:0.5 blue:1.0 alpha:1.0];
    [panelView addSubview:headerView];
    
    // Create title label
    UILabel* titleLabel = [[UILabel alloc] initWithFrame:CGRectMake(16, 0, headerView.frame.size.width - 66, 50)];
    titleLabel.text = @"AI Assistant";
    titleLabel.textColor = [UIColor whiteColor];
    titleLabel.font = [UIFont boldSystemFontOfSize:18];
    [headerView addSubview:titleLabel];
    
    // Create close button
    UIButton* closeButton = [UIButton buttonWithType:UIButtonTypeSystem];
    closeButton.frame = CGRectMake(headerView.frame.size.width - 50, 0, 50, 50);
    [closeButton setImage:[UIImage systemImageNamed:@"xmark"] forState:UIControlStateNormal];
    closeButton.tintColor = [UIColor whiteColor];
    [headerView addSubview:closeButton];
    
    // Add close button handler
    AssistantButtonController* controllerPtr = this;
    void (^closeButtonHandler)(UIButton*) = ^(UIButton* sender) {
        controllerPtr->SetVisibilityState(VisibilityState::Minimized);
    };
    
    // Store block to avoid ARC releasing it
    objc_setAssociatedObject(closeButton, "closeHandler", closeButtonHandler, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    
    // Add target-action for close button
    [closeButton addTarget:closeButtonHandler
                    action:@selector(invoke:)
          forControlEvents:UIControlEventTouchUpInside];
    
    // Create input field
    UITextField* inputField = [[UITextField alloc] initWithFrame:CGRectMake(16, panelView.frame.size.height - 60, panelView.frame.size.width - 82, 40)];
    inputField.placeholder = @"Ask a question...";
    inputField.borderStyle = UITextBorderStyleRoundedRect;
    inputField.backgroundColor = [UIColor whiteColor];
    [panelView addSubview:inputField];
    
    // Store input field
    m_inputField = (__bridge_retained void*)inputField;
    
    // Create send button
    UIButton* sendButton = [UIButton buttonWithType:UIButtonTypeSystem];
    sendButton.frame = CGRectMake(panelView.frame.size.width - 60, panelView.frame.size.height - 60, 44, 40);
    [sendButton setImage:[UIImage systemImageNamed:@"arrow.up.circle.fill"] forState:UIControlStateNormal];
    sendButton.tintColor = [UIColor colorWithRed:0.0 green:0.5 blue:1.0 alpha:1.0];
    [panelView addSubview:sendButton];
    
    // Add send button handler
    AssistantButtonController* controller_ptr = this;
    void (^sendButtonHandler)(UIButton*) = ^(UIButton* sender) {
        UITextField* field = (__bridge UITextField*)controller_ptr->m_inputField;
        NSString* text = field.text;
        
        if (text.length > 0) {
            std::string message = [text UTF8String];
            controller_ptr->ProcessUserMessage(message);
            field.text = @"";
        }
    };
    
    // Store block to avoid ARC releasing it
    objc_setAssociatedObject(sendButton, "sendHandler", sendButtonHandler, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    
    // Add target-action for send button
    [sendButton addTarget:sendButtonHandler
                   action:@selector(invoke:)
         forControlEvents:UIControlEventTouchUpInside];
    
    // Create chat view (scrollable)
    UIScrollView* chatScrollView = [[UIScrollView alloc] initWithFrame:CGRectMake(0, 50, panelView.frame.size.width, panelView.frame.size.height - 110)];
    chatScrollView.backgroundColor = [UIColor whiteColor];
    [panelView addSubview:chatScrollView];
    
    // Add content view to scroll view
    UIView* chatContentView = [[UIView alloc] initWithFrame:CGRectMake(0, 0, chatScrollView.frame.size.width, 0)]; // Height will grow as messages are added
    chatContentView.backgroundColor = [UIColor whiteColor];
    [chatScrollView addSubview:chatContentView];
    
    // Store chat view
    m_chatView = (__bridge_retained void*)chatContentView;
#endif
}

void AssistantButtonController::PositionFloatingButton() {
#if __OBJC__
    UIButton* button = (__bridge UIButton*)m_button;
    if (!button) return;
    
    UIView* superview = button.superview;
    if (!superview) return;
    
    CGRect bounds = superview.bounds;
    
    // Position based on the current setting
    CGFloat minX = m_safeAreaInsets[1];
    CGFloat minY = m_safeAreaInsets[0];
    CGFloat maxX = bounds.size.width - button.frame.size.width - m_safeAreaInsets[3];
    CGFloat maxY = bounds.size.height - button.frame.size.height - m_safeAreaInsets[2];
    
    CGRect frame = button.frame;
    
    switch (m_position) {
        case Position::TopLeft:
            frame.origin.x = minX;
            frame.origin.y = minY;
            break;
        
        case Position::TopRight:
            frame.origin.x = maxX;
            frame.origin.y = minY;
            break;
        
        case Position::BottomLeft:
            frame.origin.x = minX;
            frame.origin.y = maxY;
            break;
        
        case Position::BottomRight:
            frame.origin.x = maxX;
            frame.origin.y = maxY;
            break;
        
        case Position::Center:
            frame.origin.x = (bounds.size.width - button.frame.size.width) / 2;
            frame.origin.y = (bounds.size.height - button.frame.size.height) / 2;
            break;
    }
    
    button.frame = frame;
#endif
}

void AssistantButtonController::PositionAssistantPanel() {
#if __OBJC__
    UIView* panelView = (__bridge UIView*)m_panelView;
    UIButton* button = (__bridge UIButton*)m_button;
    
    if (!panelView || !button) return;
    
    UIView* superview = button.superview;
    if (!superview) return;
    
    CGRect bounds = superview.bounds;
    
    // Panel dimensions
    CGFloat panelWidth = 300;
    CGFloat panelHeight = 400;
    
    // Position based on the current button position
    CGRect frame = panelView.frame;
    frame.size.width = panelWidth;
    frame.size.height = panelHeight;
    
    switch (m_position) {
        case Position::TopLeft:
            frame.origin.x = m_safeAreaInsets[1];
            frame.origin.y = m_safeAreaInsets[0] + button.frame.size.height + 10;
            break;
        
        case Position::TopRight:
            frame.origin.x = bounds.size.width - panelWidth - m_safeAreaInsets[3];
            frame.origin.y = m_safeAreaInsets[0] + button.frame.size.height + 10;
            break;
        
        case Position::BottomLeft:
            frame.origin.x = m_safeAreaInsets[1];
            frame.origin.y = bounds.size.height - panelHeight - m_safeAreaInsets[2] - 10;
            break;
        
        case Position::BottomRight:
            frame.origin.x = bounds.size.width - panelWidth - m_safeAreaInsets[3];
            frame.origin.y = bounds.size.height - panelHeight - m_safeAreaInsets[2] - 10;
            break;
        
        case Position::Center:
            frame.origin.x = (bounds.size.width - panelWidth) / 2;
            frame.origin.y = (bounds.size.height - panelHeight) / 2;
            break;
    }
    
    panelView.frame = frame;
#endif
}

void AssistantButtonController::HandleButtonTap() {
    if (m_visibilityState == VisibilityState::Minimized) {
        SetVisibilityState(VisibilityState::Maximized);
    } else {
        SetVisibilityState(VisibilityState::Minimized);
    }
    
    // Call button tap callback
    m_buttonTapCallback();
}

void AssistantButtonController::SetVisibilityState(VisibilityState state) {
    if (m_visibilityState == state) return;
    
    m_visibilityState = state;
    
#if __OBJC__
    UIView* panelView = (__bridge UIView*)m_panelView;
    
    if (state == VisibilityState::Maximized) {
        // Position panel
        PositionAssistantPanel();
        
        // Show panel with animation
        panelView.alpha = 0.0;
        panelView.hidden = NO;
        
        [UIView animateWithDuration:0.3
                         animations:^{
                             panelView.alpha = 1.0;
                         }];
    } else {
        // Hide panel with animation
        [UIView animateWithDuration:0.3
                         animations:^{
                             panelView.alpha = 0.0;
                         }
                         completion:^(BOOL finished) {
                             panelView.hidden = YES;
                         }];
    }
#endif
    
    // Call visibility changed callback
    m_visibilityChangedCallback(state);
}

void AssistantButtonController::ProcessUserMessage(const std::string& message) {
    if (message.empty()) return;
    
    // Call message callback
    m_messageCallback(message);
    
    // Process with AI model if available
    if (m_assistantModel) {
        m_assistantModel->ProcessQuery(message, [this](const std::string& response) {
            AddAssistantMessage(response);
        });
    } else {
        // Default response if no model is available
        AddAssistantMessage("AI model not available. Please try again later.");
    }
}

void AssistantButtonController::AddUserMessage(const std::string& message) {
#if __OBJC__
    UIView* chatContentView = (__bridge UIView*)m_chatView;
    if (!chatContentView) return;
    
    // Create message view
    AddMessageBubble(chatContentView, message, true);
#endif
}

void AssistantButtonController::AddAssistantMessage(const std::string& message) {
#if __OBJC__
    UIView* chatContentView = (__bridge UIView*)m_chatView;
    if (!chatContentView) return;
    
    // Create message view
    AddMessageBubble(chatContentView, message, false);
#endif
}

void AssistantButtonController::SetButtonTapCallback(ButtonTapCallback callback) {
    m_buttonTapCallback = callback;
}

void AssistantButtonController::SetVisibilityChangedCallback(VisibilityChangedCallback callback) {
    m_visibilityChangedCallback = callback;
}

void AssistantButtonController::SetMessageCallback(MessageCallback callback) {
    m_messageCallback = callback;
}

void AssistantButtonController::SetAssistantModel(std::shared_ptr<AIFeatures::LocalModels::GeneralAssistantModel> model) {
    m_assistantModel = model;
}

void AssistantButtonController::HandleKeyboardWillShow(void* notification) {
#if __OBJC__
    NSNotification* note = (__bridge NSNotification*)notification;
    NSValue* keyboardFrameValue = note.userInfo[UIKeyboardFrameEndUserInfoKey];
    CGRect keyboardFrame = [keyboardFrameValue CGRectValue];
    CGFloat keyboardTop = keyboardFrame.origin.y;
    
    // Adjust chat view if keyboard is showing
    AdjustForKeyboard(true, keyboardTop);
#endif
}

void AssistantButtonController::HandleKeyboardWillHide(void* notification) {
#if __OBJC__
    // Reset chat view position
    AdjustForKeyboard(false, 0);
#endif
}

void AssistantButtonController::AdjustForKeyboard(bool visible, float keyboardTop) {
#if __OBJC__
    UIView* chatView = (__bridge UIView*)m_chatView;
    if (chatView && !chatView.hidden) {
        CGRect frame = chatView.frame;
        
        if (visible) {
            // Check if chat view overlaps with keyboard
            CGFloat chatBottom = frame.origin.y + frame.size.height;
            
            if (chatBottom > keyboardTop) {
                // Move chat view up
                CGFloat delta = chatBottom - keyboardTop + 8; // 8px padding
                
                frame.origin.y -= delta;
                
                if (frame.origin.y < m_safeAreaInsets[0]) {
                    // Reduce height instead
                    CGFloat heightReduction = m_safeAreaInsets[0] - frame.origin.y;
                    frame.origin.y = m_safeAreaInsets[0];
                    frame.size.height -= heightReduction;
                }
                
                chatView.frame = frame;
            }
        } else {
            // Reset to original position
            SetChatViewHidden(false); // This will reposition the chat view
        }
    }
#endif
}

void AssistantButtonController::SetChatViewHidden(bool hidden) {
#if __OBJC__
    UIView* chatView = (__bridge UIView*)m_chatView;
    if (chatView) {
        chatView.hidden = hidden;
        
        if (!hidden) {
            // Reposition chat view
            UIScrollView* scrollView = (UIScrollView*)chatView.superview;
            if (scrollView && [scrollView isKindOfClass:[UIScrollView class]]) {
                CGRect frame = chatView.frame;
                frame.origin.y = 0;
                chatView.frame = frame;
                
                // Scroll to bottom
                if (chatView.frame.size.height > scrollView.frame.size.height) {
                    CGPoint bottomOffset = CGPointMake(0, chatView.frame.size.height - scrollView.frame.size.height);
                    [scrollView setContentOffset:bottomOffset animated:YES];
                }
            }
        }
    }
#endif
}

// Private helper methods

void AssistantButtonController::AddMessageBubble(void* chatView, const std::string& message, bool isUser) {
#if __OBJC__
    UIView* contentView = (__bridge UIView*)chatView;
    
    // Constants for bubble appearance
    const CGFloat MaxWidth = contentView.frame.size.width * 0.7;
    const CGFloat HorizontalPadding = 16;
    const CGFloat VerticalPadding = 10;
    const CGFloat BubbleSpacing = 8;
    
    // Determine current content height
    CGFloat currentHeight = 0;
    for (UIView* subview in contentView.subviews) {
        CGFloat subviewBottom = subview.frame.origin.y + subview.frame.size.height;
        if (subviewBottom > currentHeight) {
            currentHeight = subviewBottom;
        }
    }
    
    if (currentHeight > 0) {
        currentHeight += BubbleSpacing;
    }
    
    // Create label for message
    UILabel* messageLabel = [[UILabel alloc] init];
    messageLabel.text = [NSString stringWithUTF8String:message.c_str()];
    messageLabel.numberOfLines = 0;
    messageLabel.lineBreakMode = NSLineBreakByWordWrapping;
    messageLabel.font = [UIFont systemFontOfSize:16];
    
    // Calculate size needed for text
    CGSize maxSize = CGSizeMake(MaxWidth - HorizontalPadding * 2, CGFLOAT_MAX);
    CGRect textRect = [messageLabel.text boundingRectWithSize:maxSize
                                                      options:NSStringDrawingUsesLineFragmentOrigin
                                                   attributes:@{NSFontAttributeName: messageLabel.font}
                                                      context:nil];
    
    // Create bubble view
    CGFloat bubbleWidth = textRect.size.width + HorizontalPadding * 2;
    CGFloat bubbleHeight = textRect.size.height + VerticalPadding * 2;
    
    CGFloat bubbleX = isUser ? contentView.frame.size.width - bubbleWidth - 8 : 8;
    CGRect bubbleFrame = CGRectMake(bubbleX, currentHeight, bubbleWidth, bubbleHeight);
    
    UIView* bubbleView = [[UIView alloc] initWithFrame:bubbleFrame];
    bubbleView.backgroundColor = isUser ? [UIColor colorWithRed:0.0 green:0.5 blue:1.0 alpha:1.0] : [UIColor colorWithWhite:0.9 alpha:1.0];
    bubbleView.layer.cornerRadius = 16;
    
    // Set message label frame
    messageLabel.frame = CGRectMake(HorizontalPadding, VerticalPadding, textRect.size.width, textRect.size.height);
    messageLabel.textColor = isUser ? [UIColor whiteColor] : [UIColor blackColor];
    
    // Add label to bubble
    [bubbleView addSubview:messageLabel];
    
    // Add bubble to content view
    [contentView addSubview:bubbleView];
    
    // Update content view height
    CGRect contentFrame = contentView.frame;
    CGFloat newHeight = bubbleFrame.origin.y + bubbleFrame.size.height + BubbleSpacing;
    if (newHeight > contentFrame.size.height) {
        contentFrame.size.height = newHeight;
        contentView.frame = contentFrame;
        
        // Scroll parent scroll view to bottom
        UIScrollView* scrollView = (UIScrollView*)contentView.superview;
        if (scrollView && [scrollView isKindOfClass:[UIScrollView class]]) {
            scrollView.contentSize = contentView.frame.size;
            
            if (contentView.frame.size.height > scrollView.frame.size.height) {
                CGPoint bottomOffset = CGPointMake(0, contentView.frame.size.height - scrollView.frame.size.height);
                [scrollView setContentOffset:bottomOffset animated:YES];
            }
        }
    }
#endif
}

} // namespace UI
} // namespace iOS
