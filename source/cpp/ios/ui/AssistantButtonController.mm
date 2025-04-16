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
#else
typedef void UIButton;
typedef void UIView;
typedef void UIViewController;
typedef void UIPanGestureRecognizer;
#endif

namespace iOS {
namespace UI {

// Constructor
AssistantButtonController::AssistantButtonController(void* viewController)
    : m_viewController(viewController),
      m_button(nullptr),
      m_chatView(nullptr),
      m_inputField(nullptr),
      m_panGestureRecognizer(nullptr),
      m_position(Position::BottomRight),
      m_state(VisibilityState::Minimized),
      m_isDragging(false) {
    
    // Initialize safe area insets
    m_safeAreaInsets[0] = 0.0f; // top
    m_safeAreaInsets[1] = 0.0f; // left
    m_safeAreaInsets[2] = 0.0f; // bottom
    m_safeAreaInsets[3] = 0.0f; // right
    
    // Setup UI components
    SetupButton();
    SetupChatView();
    
    // Load saved messages
    LoadMessages();
    
    std::cout << "AssistantButtonController: Created new instance" << std::endl;
}

// Destructor
AssistantButtonController::~AssistantButtonController() {
    // Save messages before destroying
    SaveMessages();
    
    // Clean up views
#if __OBJC__
    UIButton* button = (__bridge UIButton*)m_button;
    if (button) {
        [button removeFromSuperview];
    }
    
    UIView* chatView = (__bridge UIView*)m_chatView;
    if (chatView) {
        [chatView removeFromSuperview];
    }
    
    // Release Objective-C objects
    if (m_button) {
        CFRelease(m_button);
        m_button = nullptr;
    }
    
    if (m_chatView) {
        CFRelease(m_chatView);
        m_chatView = nullptr;
    }
    
    if (m_inputField) {
        CFRelease(m_inputField);
        m_inputField = nullptr;
    }
    
    if (m_panGestureRecognizer) {
        CFRelease(m_panGestureRecognizer);
        m_panGestureRecognizer = nullptr;
    }
#endif
    
    std::cout << "AssistantButtonController: Instance destroyed" << std::endl;
}

// Set button position
void AssistantButtonController::SetPosition(Position position) {
    m_position = position;
    UpdateButtonPosition();
}

// Set button appearance
void AssistantButtonController::SetAppearance(const ButtonAppearance& appearance) {
    m_appearance = appearance;
    ConfigureAppearance();
}

// Set visibility state
void AssistantButtonController::SetVisibilityState(VisibilityState state) {
    if (m_state == state) {
        return;
    }
    
    m_state = state;
    
    switch (m_state) {
        case VisibilityState::Hidden:
            SetButtonHidden(true);
            SetChatViewHidden(true);
            break;
        case VisibilityState::Minimized:
            SetButtonHidden(false);
            SetChatViewHidden(true);
            break;
        case VisibilityState::Visible:
            SetButtonHidden(false);
            SetChatViewHidden(false);
            AnimateChatOpen();
            break;
    }
}

// Get current visibility state
AssistantButtonController::VisibilityState AssistantButtonController::GetVisibilityState() const {
    return m_state;
}

// Set custom message handler
void AssistantButtonController::SetMessageHandler(MessageHandler handler) {
    m_customMessageHandler = handler;
}

// Set assistant model
void AssistantButtonController::SetAssistantModel(std::shared_ptr<AIFeatures::LocalModels::GeneralAssistantModel> model) {
    m_assistantModel = model;
    
    if (m_assistantModel && m_assistantModel->IsInitialized()) {
        // Let the user know the assistant is ready
        SendSystemMessage("AI Assistant is ready to help! Ask me anything about the executor.");
    }
}

// Get assistant model
std::shared_ptr<AIFeatures::LocalModels::GeneralAssistantModel> AssistantButtonController::GetAssistantModel() const {
    return m_assistantModel;
}

// Send system message to chat
void AssistantButtonController::SendSystemMessage(const std::string& message) {
    if (message.empty()) {
        return;
    }
    
    AddMessage(message, MessageType::System);
}

// Send action message to chat
void AssistantButtonController::SendActionMessage(const std::string& message) {
    if (message.empty()) {
        return;
    }
    
    AddMessage(message, MessageType::Action);
}

// Clear chat history
void AssistantButtonController::ClearChatHistory() {
    m_messages.clear();
    UpdateChatView();
    
    // If we have an assistant model, also reset its conversation
    if (m_assistantModel) {
        m_assistantModel->ResetConversation();
    }
}

// Get chat message history
std::vector<AssistantButtonController::ChatMessage> AssistantButtonController::GetChatHistory() const {
    return m_messages;
}

// Handle device orientation change
void AssistantButtonController::HandleOrientationChange() {
    // Update button position for new orientation
    UpdateButtonPosition();
    
    // If chat is visible, update its position as well
    if (m_state == VisibilityState::Visible) {
        AnimateChatOpen();
    }
}

// Update safe area insets
void AssistantButtonController::UpdateSafeAreaInsets(float top, float left, float bottom, float right) {
    m_safeAreaInsets[0] = top;
    m_safeAreaInsets[1] = left;
    m_safeAreaInsets[2] = bottom;
    m_safeAreaInsets[3] = right;
    
    // Update positions with new insets
    UpdateButtonPosition();
}

// Private: Setup button
void AssistantButtonController::SetupButton() {
#if __OBJC__
    UIViewController* viewController = (__bridge UIViewController*)m_viewController;
    if (!viewController) {
        std::cerr << "AssistantButtonController: Invalid view controller" << std::endl;
        return;
    }
    
    // Create the button
    UIButton* button = [UIButton buttonWithType:UIButtonTypeSystem];
    button.frame = CGRectMake(0, 0, m_appearance.size, m_appearance.size);
    button.layer.cornerRadius = m_appearance.cornerRadius;
    button.clipsToBounds = YES;
    button.alpha = m_appearance.alpha;
    
    // Set image if available, otherwise use text
    UIImage* image = [UIImage systemImageNamed:@"message.circle.fill"];
    if (image) {
        [button setImage:image forState:UIControlStateNormal];
    } else {
        [button setTitle:@"AI" forState:UIControlStateNormal];
    }
    
    // Set colors
    button.backgroundColor = [UIColor systemBlueColor];
    button.tintColor = [UIColor whiteColor];
    
    // Add shadow
    button.layer.shadowColor = [UIColor blackColor].CGColor;
    button.layer.shadowOffset = CGSizeMake(0, 2);
    button.layer.shadowRadius = 4;
    button.layer.shadowOpacity = 0.4;
    
    // Create a block to handle button taps
    void (^buttonTapHandler)(UIButton*) = ^(UIButton* sender) {
        self->HandleButtonTap();
    };
    
    // Store block to avoid ARC releasing it
    objc_setAssociatedObject(button, "tapHandler", buttonTapHandler, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    
    // Add target-action for button tap
    [button addTarget:buttonTapHandler 
               action:@selector(invoke:) 
     forControlEvents:UIControlEventTouchUpInside];
    
    // Add pan gesture recognizer
    UIPanGestureRecognizer* panGesture = [[UIPanGestureRecognizer alloc] init];
    
    // Create a block to handle panning
    void (^panHandler)(UIPanGestureRecognizer*) = ^(UIPanGestureRecognizer* gesture) {
        CGPoint translation = [gesture translationInView:button.superview];
        
        if (gesture.state == UIGestureRecognizerStateBegan) {
            self->m_isDragging = true;
        }
        
        if (gesture.state == UIGestureRecognizerStateChanged) {
            button.center = CGPointMake(button.center.x + translation.x, 
                                       button.center.y + translation.y);
            [gesture setTranslation:CGPointZero inView:button.superview];
        }
        
        if (gesture.state == UIGestureRecognizerStateEnded || 
            gesture.state == UIGestureRecognizerStateCancelled) {
            self->m_isDragging = false;
            
            // Snap to nearest edge
            CGRect bounds = button.superview.bounds;
            CGFloat minX = self->m_safeAreaInsets[1];
            CGFloat minY = self->m_safeAreaInsets[0];
            CGFloat maxX = bounds.size.width - button.frame.size.width - self->m_safeAreaInsets[3];
            CGFloat maxY = bounds.size.height - button.frame.size.height - self->m_safeAreaInsets[2];
            
            CGFloat x = button.frame.origin.x;
            CGFloat y = button.frame.origin.y;
            
            // Determine which edge is closest
            bool isCloserToLeft = (button.center.x < bounds.size.width / 2);
            bool isCloserToTop = (button.center.y < bounds.size.height / 2);
            
            x = isCloserToLeft ? minX : maxX;
            y = MAX(minY, MIN(y, maxY));
            
            // Animate to edge
            [UIView animateWithDuration:0.3 animations:^{
                button.frame = CGRectMake(x, y, button.frame.size.width, button.frame.size.height);
            }];
            
            // Update position state based on final position
            if (isCloserToLeft) {
                if (y < bounds.size.height / 3) {
                    self->m_position = Position::TopLeft;
                } else if (y > bounds.size.height * 2 / 3) {
                    self->m_position = Position::BottomLeft;
                } else {
                    self->m_position = Position::Center;
                }
            } else {
                if (y < bounds.size.height / 3) {
                    self->m_position = Position::TopRight;
                } else if (y > bounds.size.height * 2 / 3) {
                    self->m_position = Position::BottomRight;
                } else {
                    self->m_position = Position::Center;
                }
            }
        }
    };
    
    // Store block to avoid ARC releasing it
    objc_setAssociatedObject(panGesture, "panHandler", panHandler, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    
    // Set action for pan gesture
    [panGesture addTarget:panHandler action:@selector(invoke:)];
    [button addGestureRecognizer:panGesture];
    
    // Add to view controller
    [viewController.view addSubview:button];
    
    // Store button
    m_button = (__bridge_retained void*)button;
    m_panGestureRecognizer = (__bridge_retained void*)panGesture;
    
    // Position button
    UpdateButtonPosition();
#endif
}

// Private: Setup chat view
void AssistantButtonController::SetupChatView() {
#if __OBJC__
    UIViewController* viewController = (__bridge UIViewController*)m_viewController;
    if (!viewController) {
        std::cerr << "AssistantButtonController: Invalid view controller" << std::endl;
        return;
    }
    
    // Create the chat view container
    UIView* chatView = [[UIView alloc] initWithFrame:CGRectMake(0, 0, 300, 400)];
    chatView.backgroundColor = [UIColor systemBackgroundColor];
    chatView.layer.cornerRadius = 12;
    chatView.clipsToBounds = YES;
    chatView.layer.shadowColor = [UIColor blackColor].CGColor;
    chatView.layer.shadowOffset = CGSizeMake(0, 4);
    chatView.layer.shadowRadius = 8;
    chatView.layer.shadowOpacity = 0.3;
    chatView.hidden = YES;
    
    // Add title bar
    UIView* titleBar = [[UIView alloc] initWithFrame:CGRectMake(0, 0, chatView.frame.size.width, 44)];
    titleBar.backgroundColor = [UIColor systemBlueColor];
    [chatView addSubview:titleBar];
    
    // Add title label
    UILabel* titleLabel = [[UILabel alloc] initWithFrame:CGRectMake(44, 0, titleBar.frame.size.width - 88, 44)];
    titleLabel.text = @"AI Assistant";
    titleLabel.textColor = [UIColor whiteColor];
    titleLabel.textAlignment = NSTextAlignmentCenter;
    titleLabel.font = [UIFont boldSystemFontOfSize:16];
    [titleBar addSubview:titleLabel];
    
    // Add close button
    UIButton* closeButton = [UIButton buttonWithType:UIButtonTypeSystem];
    closeButton.frame = CGRectMake(titleBar.frame.size.width - 44, 0, 44, 44);
    closeButton.tintColor = [UIColor whiteColor];
    [closeButton setImage:[UIImage systemImageNamed:@"xmark"] forState:UIControlStateNormal];
    [titleBar addSubview:closeButton];
    
    // Create a table view for messages
    UITableView* messagesTable = [[UITableView alloc] initWithFrame:CGRectMake(0, 44, chatView.frame.size.width, chatView.frame.size.height - 88) style:UITableViewStylePlain];
    messagesTable.backgroundColor = [UIColor systemBackgroundColor];
    messagesTable.separatorStyle = UITableViewCellSeparatorStyleNone;
    messagesTable.allowsSelection = NO;
    [chatView addSubview:messagesTable];
    
    // Add input area
    UIView* inputArea = [[UIView alloc] initWithFrame:CGRectMake(0, chatView.frame.size.height - 44, chatView.frame.size.width, 44)];
    inputArea.backgroundColor = [UIColor systemBackgroundColor];
    [chatView addSubview:inputArea];
    
    // Add text field
    UITextField* inputField = [[UITextField alloc] initWithFrame:CGRectMake(8, 7, inputArea.frame.size.width - 52, 30)];
    inputField.placeholder = @"Ask a question...";
    inputField.borderStyle = UITextBorderStyleRoundedRect;
    inputField.backgroundColor = [UIColor systemGray6Color];
    inputField.returnKeyType = UIReturnKeySend;
    [inputArea addSubview:inputField];
    
    // Add send button
    UIButton* sendButton = [UIButton buttonWithType:UIButtonTypeSystem];
    sendButton.frame = CGRectMake(inputArea.frame.size.width - 44, 0, 44, 44);
    [sendButton setImage:[UIImage systemImageNamed:@"arrow.up.circle.fill"] forState:UIControlStateNormal];
    [inputArea addSubview:sendButton];
    
    // Add to view controller
    [viewController.view addSubview:chatView];
    
    // Store chat view and input field
    m_chatView = (__bridge_retained void*)chatView;
    m_inputField = (__bridge_retained void*)inputField;
    
    // Create close button handler
    void (^closeButtonHandler)(UIButton*) = ^(UIButton* sender) {
        self->SetVisibilityState(VisibilityState::Minimized);
    };
    
    // Store block to avoid ARC releasing it
    objc_setAssociatedObject(closeButton, "closeHandler", closeButtonHandler, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    
    // Add target-action for close button
    [closeButton addTarget:closeButtonHandler 
                    action:@selector(invoke:) 
          forControlEvents:UIControlEventTouchUpInside];
    
    // Create send button handler
    void (^sendButtonHandler)(UIButton*) = ^(UIButton* sender) {
        UITextField* field = (__bridge UITextField*)self->m_inputField;
        NSString* text = field.text;
        if (text.length > 0) {
            std::string message = [text UTF8String];
            self->ProcessUserMessage(message);
            field.text = @"";
        }
    };
    
    // Store block to avoid ARC releasing it
    objc_setAssociatedObject(sendButton, "sendHandler", sendButtonHandler, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    
    // Add target-action for send button
    [sendButton addTarget:sendButtonHandler 
                   action:@selector(invoke:) 
         forControlEvents:UIControlEventTouchUpInside];
    
    // Create text field delegate
    id<UITextFieldDelegate> textFieldDelegate = [[NSObject alloc] init];
    
    // Create text field return handler
    BOOL (^textFieldShouldReturn)(UITextField*) = ^BOOL(UITextField* textField) {
        if (textField.text.length > 0) {
            std::string message = [textField.text UTF8String];
            self->ProcessUserMessage(message);
            textField.text = @"";
        }
        return YES;
    };
    
    // Store block to avoid ARC releasing it
    objc_setAssociatedObject(textFieldDelegate, "textFieldShouldReturn", textFieldShouldReturn, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    
    // Use method swizzling to implement delegate method
    class_addMethod([textFieldDelegate class], 
                    @selector(textFieldShouldReturn:), 
                    imp_implementationWithBlock(textFieldShouldReturn), 
                    "c@:@");
    
    // Set delegate
    inputField.delegate = textFieldDelegate;
    
    // Configure table view
    messagesTable.dataSource = [[NSObject alloc] init];
    messagesTable.delegate = [[NSObject alloc] init];
    
    // Create table view data source methods
    NSInteger (^numberOfRowsInSection)(UITableView*, NSInteger) = ^NSInteger(UITableView* tableView, NSInteger section) {
        return self->m_messages.size();
    };
    
    // Store block to avoid ARC releasing it
    objc_setAssociatedObject(messagesTable.dataSource, "numberOfRowsInSection", numberOfRowsInSection, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    
    // Use method swizzling to implement data source method
    class_addMethod([messagesTable.dataSource class], 
                    @selector(tableView:numberOfRowsInSection:), 
                    imp_implementationWithBlock(numberOfRowsInSection), 
                    "i@:@i");
    
    // Create cell for row method
    UITableViewCell* (^cellForRowAtIndexPath)(UITableView*, NSIndexPath*) = ^UITableViewCell*(UITableView* tableView, NSIndexPath* indexPath) {
        static NSString* cellIdentifier = @"MessageCell";
        
        UITableViewCell* cell = [tableView dequeueReusableCellWithIdentifier:cellIdentifier];
        if (cell == nil) {
            cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:cellIdentifier];
            cell.selectionStyle = UITableViewCellSelectionStyleNone;
            
            // Configure message bubble
            UIView* bubbleView = [[UIView alloc] init];
            bubbleView.layer.cornerRadius = 12;
            bubbleView.tag = 101;
            [cell.contentView addSubview:bubbleView];
            
            // Configure message label
            UILabel* messageLabel = [[UILabel alloc] init];
            messageLabel.numberOfLines = 0;
            messageLabel.tag = 102;
            messageLabel.font = [UIFont systemFontOfSize:14];
            [bubbleView addSubview:messageLabel];
        }
        
        // Configure cell based on message type
        if (indexPath.row < self->m_messages.size()) {
            const ChatMessage& message = self->m_messages[indexPath.row];
            UIView* bubbleView = [cell.contentView viewWithTag:101];
            UILabel* messageLabel = (UILabel*)[bubbleView viewWithTag:102];
            
            // Set message text
            messageLabel.text = @(message.text.c_str());
            
            // Layout message and bubble
            CGSize maxSize = CGSizeMake(tableView.frame.size.width * 0.7, CGFLOAT_MAX);
            CGSize messageSize = [messageLabel.text boundingRectWithSize:maxSize
                                                options:NSStringDrawingUsesLineFragmentOrigin
                                                attributes:@{NSFontAttributeName: messageLabel.font}
                                                context:nil].size;
            
            // Add padding
            messageSize.width += 24;
            messageSize.height += 16;
            
            // Position bubble and label based on message type
            CGFloat bubbleX = 8;
            if (message.type == MessageType::User) {
                bubbleX = tableView.frame.size.width - messageSize.width - 8;
                bubbleView.backgroundColor = [UIColor systemBlueColor];
                messageLabel.textColor = [UIColor whiteColor];
            } else if (message.type == MessageType::Assistant) {
                bubbleView.backgroundColor = [UIColor systemGray5Color];
                messageLabel.textColor = [UIColor labelColor];
            } else if (message.type == MessageType::System) {
                bubbleView.backgroundColor = [UIColor systemGray6Color];
                messageLabel.textColor = [UIColor secondaryLabelColor];
                messageLabel.font = [UIFont italicSystemFontOfSize:14];
            } else { // Action
                bubbleView.backgroundColor = [UIColor systemYellowColor];
                messageLabel.textColor = [UIColor labelColor];
                messageLabel.font = [UIFont boldSystemFontOfSize:14];
            }
            
            bubbleView.frame = CGRectMake(bubbleX, 8, messageSize.width, messageSize.height);
            messageLabel.frame = CGRectMake(12, 8, messageSize.width - 24, messageSize.height - 16);
        }
        
        return cell;
    };
    
    // Store block to avoid ARC releasing it
    objc_setAssociatedObject(messagesTable.dataSource, "cellForRowAtIndexPath", cellForRowAtIndexPath, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    
    // Use method swizzling to implement data source method
    class_addMethod([messagesTable.dataSource class], 
                    @selector(tableView:cellForRowAtIndexPath:), 
                    imp_implementationWithBlock(cellForRowAtIndexPath), 
                    "@@:@@");
    
    // Create height for row method
    CGFloat (^heightForRowAtIndexPath)(UITableView*, NSIndexPath*) = ^CGFloat(UITableView* tableView, NSIndexPath* indexPath) {
        if (indexPath.row < self->m_messages.size()) {
            const ChatMessage& message = self->m_messages[indexPath.row];
            
            // Calculate height based on message content
            UIFont* font = [UIFont systemFontOfSize:14];
            CGSize maxSize = CGSizeMake(tableView.frame.size.width * 0.7, CGFLOAT_MAX);
            CGSize messageSize = [@(message.text.c_str()) boundingRectWithSize:maxSize
                                            options:NSStringDrawingUsesLineFragmentOrigin
                                            attributes:@{NSFontAttributeName: font}
                                            context:nil].size;
            
            // Add padding and margins
            return messageSize.height + 32;
        }
        
        return 44; // Default height
    };
    
    // Store block to avoid ARC releasing it
    objc_setAssociatedObject(messagesTable.delegate, "heightForRowAtIndexPath", heightForRowAtIndexPath, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    
    // Use method swizzling to implement delegate method
    class_addMethod([messagesTable.delegate class], 
                    @selector(tableView:heightForRowAtIndexPath:), 
                    imp_implementationWithBlock(heightForRowAtIndexPath), 
                    "f@:@@");
#endif
}

// Private: Configure appearance
void AssistantButtonController::ConfigureAppearance() {
#if __OBJC__
    UIButton* button = (__bridge UIButton*)m_button;
    if (button) {
        // Update button appearance
        button.frame = CGRectMake(button.frame.origin.x, button.frame.origin.y, 
                                 m_appearance.size, m_appearance.size);
        button.layer.cornerRadius = m_appearance.cornerRadius;
        button.alpha = m_appearance.alpha;
        
        // Set image if specified
        if (!m_appearance.iconName.empty()) {
            UIImage* image = [UIImage systemImageNamed:@(m_appearance.iconName.c_str())];
            if (image) {
                [button setImage:image forState:UIControlStateNormal];
            }
        }
        
        // Set colors if specified
        if (!m_appearance.backgroundColor.empty()) {
            unsigned int hexValue = 0;
            NSScanner* scanner = [NSScanner scannerWithString:@(m_appearance.backgroundColor.c_str()+1)];
            [scanner scanHexInt:&hexValue];
            button.backgroundColor = [UIColor colorWithRed:((hexValue & 0xFF0000) >> 16) / 255.0
                                              green:((hexValue & 0x00FF00) >> 8) / 255.0
                                               blue:(hexValue & 0x0000FF) / 255.0
                                              alpha:1.0];
        }
        
        if (!m_appearance.tintColor.empty()) {
            unsigned int hexValue = 0;
            NSScanner* scanner = [NSScanner scannerWithString:@(m_appearance.tintColor.c_str()+1)];
            [scanner scanHexInt:&hexValue];
            button.tintColor = [UIColor colorWithRed:((hexValue & 0xFF0000) >> 16) / 255.0
                                              green:((hexValue & 0x00FF00) >> 8) / 255.0
                                               blue:(hexValue & 0x0000FF) / 255.0
                                              alpha:1.0];
        }
    }
#endif
}

// Private: Update button position
void AssistantButtonController::UpdateButtonPosition() {
#if __OBJC__
    UIButton* button = (__bridge UIButton*)m_button;
    if (!button || m_isDragging) {
        return;
    }
    
    UIView* superview = button.superview;
    if (!superview) {
        return;
    }
    
    CGRect bounds = superview.bounds;
    CGFloat x = 0.0f;
    CGFloat y = 0.0f;
    
    // Calculate position based on enum
    switch (m_position) {
        case Position::TopLeft:
            x = m_safeAreaInsets[1];
            y = m_safeAreaInsets[0];
            break;
        case Position::TopRight:
            x = bounds.size.width - button.frame.size.width - m_safeAreaInsets[3];
            y = m_safeAreaInsets[0];
            break;
        case Position::BottomLeft:
            x = m_safeAreaInsets[1];
            y = bounds.size.height - button.frame.size.height - m_safeAreaInsets[2];
            break;
        case Position::BottomRight:
            x = bounds.size.width - button.frame.size.width - m_safeAreaInsets[3];
            y = bounds.size.height - button.frame.size.height - m_safeAreaInsets[2];
            break;
        case Position::Center:
            x = bounds.size.width / 2 - button.frame.size.width / 2;
            y = bounds.size.height / 2 - button.frame.size.height / 2;
            break;
    }
    
    // Animate to new position
    [UIView animateWithDuration:0.3 animations:^{
        button.frame = CGRectMake(x, y, button.frame.size.width, button.frame.size.height);
    }];
#endif
}

// Private: Handle button tap
void AssistantButtonController::HandleButtonTap() {
    if (m_state == VisibilityState::Minimized) {
        SetVisibilityState(VisibilityState::Visible);
    } else if (m_state == VisibilityState::Visible) {
        SetVisibilityState(VisibilityState::Minimized);
    }
    
    AnimateButtonPress();
}

// Private: Add message
void AssistantButtonController::AddMessage(const std::string& text, MessageType type) {
    if (text.empty()) {
        return;
    }
    
    uint64_t timestamp = static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count()
    );
    
    ChatMessage message(text, type, timestamp);
    m_messages.push_back(message);
    
    // Update UI
    UpdateChatView();
    
    // If adding a user message to the assistant model
    if (type == MessageType::User && m_assistantModel) {
        // Let the user know the assistant is processing
        AddMessage("Thinking...", MessageType::System);
    }
}

// Private: Process user message
void AssistantButtonController::ProcessUserMessage(const std::string& text) {
    if (text.empty()) {
        return;
    }
    
    // Add user message to chat
    AddMessage(text, MessageType::User);
    
    // Process message with assistant model or custom handler
    std::string response;
    
    if (m_customMessageHandler) {
        // Use custom handler
        response = m_customMessageHandler(text);
    } else if (m_assistantModel && m_assistantModel->IsInitialized()) {
        // Use AI model
        response = m_assistantModel->ProcessInput(text);
    } else {
        // Fallback response
        response = "I'm sorry, I can't process your request at the moment. The AI system is not available.";
    }
    
    // Remove "Thinking..." message
    if (!m_messages.empty() && m_messages.back().type == MessageType::System && 
        m_messages.back().text == "Thinking...") {
        m_messages.pop_back();
    }
    
    // Add assistant response to chat
    AddMessage(response, MessageType::Assistant);
}

// Private: Update chat view
void AssistantButtonController::UpdateChatView() {
#if __OBJC__
    UIView* chatView = (__bridge UIView*)m_chatView;
    if (!chatView) {
        return;
    }
    
    // Find table view
    UITableView* tableView = nil;
    for (UIView* subview in chatView.subviews) {
        if ([subview isKindOfClass:[UITableView class]]) {
            tableView = (UITableView*)subview;
            break;
        }
    }
    
    if (tableView) {
        [tableView reloadData];
        
        // Scroll to bottom
        if (m_messages.size() > 0) {
            NSIndexPath* indexPath = [NSIndexPath indexPathForRow:m_messages.size() - 1 inSection:0];
            [tableView scrollToRowAtIndexPath:indexPath atScrollPosition:UITableViewScrollPositionBottom animated:YES];
        }
    }
#endif
}

// Private: Set button hidden
void AssistantButtonController::SetButtonHidden(bool hidden) {
#if __OBJC__
    UIButton* button = (__bridge UIButton*)m_button;
    if (button) {
        button.hidden = hidden;
    }
#endif
}

// Private: Set chat view hidden
void AssistantButtonController::SetChatViewHidden(bool hidden) {
#if __OBJC__
    UIView* chatView = (__bridge UIView*)m_chatView;
    if (chatView) {
        if (hidden) {
            // Hide immediately
            chatView.hidden = YES;
        } else {
            // Position before showing
            UIButton* button = (__bridge UIButton*)m_button;
            if (button) {
                CGFloat x = 0.0f;
                CGFloat y = 0.0f;
                CGFloat width = 300.0f;
                CGFloat height = 400.0f;
                
                // Position based on button position
                switch (m_position) {
                    case Position::TopLeft:
                        x = button.frame.origin.x;
                        y = button.frame.origin.y + button.frame.size.height + 8;
                        break;
                    case Position::TopRight:
                        x = button.frame.origin.x + button.frame.size.width - width;
                        y = button.frame.origin.y + button.frame.size.height + 8;
                        break;
                    case Position::BottomLeft:
                        x = button.frame.origin.x;
                        y = button.frame.origin.y - height - 8;
                        break;
                    case Position::BottomRight:
                        x = button.frame.origin.x + button.frame.size.width - width;
                        y = button.frame.origin.y - height - 8;
                        break;
                    case Position::Center:
                        x = button.frame.origin.x + button.frame.size.width / 2 - width / 2;
                        y = button.frame.origin.y - height - 8;
                        if (y < m_safeAreaInsets[0]) {
                            y = button.frame.origin.y + button.frame.size.height + 8;
                        }
                        break;
                }
                
                // Make sure chat view is fully visible
                UIView* superview = button.superview;
                if (superview) {
                    CGRect bounds = superview.bounds;
                    
                    // Adjust horizontal position
                    if (x < m_safeAreaInsets[1]) {
                        x = m_safeAreaInsets[1];
                    } else if (x + width > bounds.size.width - m_safeAreaInsets[3]) {
                        x = bounds.size.width - width - m_safeAreaInsets[3];
                    }
                    
                    // Adjust vertical position
                    if (y < m_safeAreaInsets[0]) {
                        y = m_safeAreaInsets[0];
                    } else if (y + height > bounds.size.height - m_safeAreaInsets[2]) {
                        y = bounds.size.height - height - m_safeAreaInsets[2];
                    }
                }
                
                chatView.frame = CGRectMake(x, y, width, height);
            }
            
            // Show chat view
            chatView.hidden = NO;
            
            // Update chat view content
            UpdateChatView();
        }
    }
#endif
}

// Private: Animate button press
void AssistantButtonController::AnimateButtonPress() {
#if __OBJC__
    UIButton* button = (__bridge UIButton*)m_button;
    if (button) {
        // Scale down
        [UIView animateWithDuration:0.1 animations:^{
            button.transform = CGAffineTransformMakeScale(0.9, 0.9);
        } completion:^(BOOL finished) {
            // Scale back up
            [UIView animateWithDuration:0.1 animations:^{
                button.transform = CGAffineTransformIdentity;
            }];
        }];
    }
#endif
}

// Private: Animate chat open
void AssistantButtonController::AnimateChatOpen() {
#if __OBJC__
    UIView* chatView = (__bridge UIView*)m_chatView;
    if (chatView && !chatView.hidden) {
        // Start with small scale
        chatView.transform = CGAffineTransformMakeScale(0.8, 0.8);
        chatView.alpha = 0.0;
        
        // Animate to full size
        [UIView animateWithDuration:0.3 animations:^{
            chatView.transform = CGAffineTransformIdentity;
            chatView.alpha = 1.0;
        }];
        
        // Focus on input field
        UITextField* inputField = (__bridge UITextField*)m_inputField;
        if (inputField) {
            [inputField becomeFirstResponder];
        }
    }
#endif
}

// Private: Animate chat close
void AssistantButtonController::AnimateChatClose() {
#if __OBJC__
    UIView* chatView = (__bridge UIView*)m_chatView;
    if (chatView && !chatView.hidden) {
        // Animate to small scale
        [UIView animateWithDuration:0.2 animations:^{
            chatView.transform = CGAffineTransformMakeScale(0.8, 0.8);
            chatView.alpha = 0.0;
        } completion:^(BOOL finished) {
            chatView.hidden = YES;
            chatView.transform = CGAffineTransformIdentity;
        }];
        
        // Resign first responder to dismiss keyboard
        UITextField* inputField = (__bridge UITextField*)m_inputField;
        if (inputField) {
            [inputField resignFirstResponder];
        }
    }
#endif
}

// Private: Save messages
void AssistantButtonController::SaveMessages() {
    // In a production implementation, messages would be saved to disk
    // For this example, we'll just log that we're saving
    std::cout << "AssistantButtonController: Saving " << m_messages.size() << " messages" << std::endl;
}

// Private: Load messages
void AssistantButtonController::LoadMessages() {
    // In a production implementation, messages would be loaded from disk
    // For this example, we'll just log that we're loading and add a welcome message
    std::cout << "AssistantButtonController: Loading messages" << std::endl;
    
    // Add welcome message
    uint64_t timestamp = static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count()
    );
    
    ChatMessage welcome("Welcome to the AI Assistant! How can I help you with the executor today?", 
                       MessageType::System, 
                       timestamp);
    
    m_messages.push_back(welcome);
}

// Private: Adjust for keyboard
void AssistantButtonController::AdjustForKeyboard(bool visible, float keyboardHeight) {
#if __OBJC__
    UIView* chatView = (__bridge UIView*)m_chatView;
    if (chatView && !chatView.hidden) {
        CGRect frame = chatView.frame;
        
        if (visible) {
            // Check if chat view overlaps with keyboard
            CGFloat chatBottom = frame.origin.y + frame.size.height;
            CGFloat screenHeight = chatView.superview.bounds.size.height;
            CGFloat keyboardTop = screenHeight - keyboardHeight;
            
            if (chatBottom > keyboardTop) {
                // Move chat view up
                CGFloat delta = chatBottom - keyboardTop + 8; // 8px padding
                frame.origin.y -= delta;
                
                // Ensure top stays visible
                if (frame.origin.y < m_safeAreaInsets[0]) {
                    // Reduce height instead
                    CGFloat heightReduction = m_safeAreaInsets[0] - frame.origin.y;
                    frame.origin.y = m_safeAreaInsets[0];
                    frame.size.height -= heightReduction;
                }
                
                chatView.frame = frame;
            }
        } else {
            // Restore original position and size
            SetChatViewHidden(false); // This will reposition the chat view
        }
    }
#endif
}

} // namespace UI
} // namespace iOS
