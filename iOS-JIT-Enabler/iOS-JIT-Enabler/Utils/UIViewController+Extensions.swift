import UIKit

extension UIViewController {
    // Show an alert with a message
    func showAlert(title: String, message: String, completion: (() -> Void)? = nil) {
        let alertController = UIAlertController(title: title, message: message, preferredStyle: .alert)
        let okAction = UIAlertAction(title: "OK", style: .default) { _ in
            completion?()
        }
        alertController.addAction(okAction)
        present(alertController, animated: true, completion: nil)
    }
    
    // Show an error alert
    func showError(_ error: Error, completion: (() -> Void)? = nil) {
        showAlert(title: "Error", message: error.localizedDescription, completion: completion)
    }
    
    // Show a loading indicator
    func showLoadingIndicator() -> UIAlertController {
        let alert = UIAlertController(title: nil, message: "Please wait...", preferredStyle: .alert)
        
        let loadingIndicator = UIActivityIndicatorView(frame: CGRect(x: 10, y: 5, width: 50, height: 50))
        loadingIndicator.hidesWhenStopped = true
        loadingIndicator.style = .medium
        loadingIndicator.startAnimating()
        
        alert.view.addSubview(loadingIndicator)
        present(alert, animated: true, completion: nil)
        
        return alert
    }
    
    // Hide a loading indicator
    func hideLoadingIndicator(indicator: UIAlertController) {
        indicator.dismiss(animated: true, completion: nil)
    }
    
    // Show a confirmation alert with Yes/No options
    func showConfirmation(title: String, message: String, yesHandler: @escaping () -> Void, noHandler: (() -> Void)? = nil) {
        let alertController = UIAlertController(title: title, message: message, preferredStyle: .alert)
        
        let yesAction = UIAlertAction(title: "Yes", style: .default) { _ in
            yesHandler()
        }
        
        let noAction = UIAlertAction(title: "No", style: .cancel) { _ in
            noHandler?()
        }
        
        alertController.addAction(yesAction)
        alertController.addAction(noAction)
        
        present(alertController, animated: true, completion: nil)
    }
    
    // Show a text input alert
    func showTextInputAlert(title: String, message: String, placeholder: String, defaultText: String? = nil, completion: @escaping (String?) -> Void) {
        let alertController = UIAlertController(title: title, message: message, preferredStyle: .alert)
        
        alertController.addTextField { textField in
            textField.placeholder = placeholder
            textField.text = defaultText
        }
        
        let cancelAction = UIAlertAction(title: "Cancel", style: .cancel) { _ in
            completion(nil)
        }
        
        let saveAction = UIAlertAction(title: "Save", style: .default) { _ in
            let textField = alertController.textFields?.first
            completion(textField?.text)
        }
        
        alertController.addAction(cancelAction)
        alertController.addAction(saveAction)
        
        present(alertController, animated: true, completion: nil)
    }
}