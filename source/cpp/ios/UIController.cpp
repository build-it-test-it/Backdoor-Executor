#include "UIController.h"
#include "ui/MainViewController.h"
#include <iostream>

namespace iOS {

// Implementation of the GetMainViewController method
std::shared_ptr<UI::MainViewController> UIController::GetMainViewController() const {
    // Create a new MainViewController instance if needed
    static std::shared_ptr<UI::MainViewController> mainViewController = 
        std::make_shared<UI::MainViewController>();
    
    return mainViewController;
}

} // namespace iOS
