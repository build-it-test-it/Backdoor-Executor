import Foundation
import UIKit
import MobileCoreServices

class AppManager {
    static let shared = AppManager()
    
    private init() {}
    
    // Get a list of installed apps that might benefit from JIT
    func getJITCompatibleApps() -> [App] {
        // In a real implementation, this would use LSApplicationWorkspace or similar private APIs
        // to get a list of installed apps. Since we can't use private APIs in App Store apps,
        // we'll return a list of common apps that might benefit from JIT.
        
        // This is a simplified implementation. In a real app, you would need to use alternative
        // methods to detect installed apps, such as checking for URL schemes.
        
        return [
            createApp(bundleId: "com.example.emulator1", name: "Emulator 1"),
            createApp(bundleId: "com.example.emulator2", name: "Emulator 2"),
            createApp(bundleId: "com.example.javascript", name: "JavaScript Engine"),
            createApp(bundleId: "com.example.vm", name: "Virtual Machine"),
            createApp(bundleId: "com.example.interpreter", name: "Code Interpreter")
        ]
    }
    
    // Check if an app is installed by its bundle ID
    func isAppInstalled(bundleId: String) -> Bool {
        guard let url = URL(string: "\(bundleId)://") else {
            return false
        }
        
        return UIApplication.shared.canOpenURL(url)
    }
    
    // Create an App object with placeholder icon
    private func createApp(bundleId: String, name: String) -> App {
        // Create a placeholder icon
        let iconSize = CGSize(width: 60, height: 60)
        UIGraphicsBeginImageContextWithOptions(iconSize, false, 0)
        
        let context = UIGraphicsGetCurrentContext()
        context?.setFillColor(UIColor.systemBlue.cgColor)
        context?.fillEllipse(in: CGRect(origin: .zero, size: iconSize))
        
        // Add app initials
        let initials = String(name.prefix(1))
        let attributes: [NSAttributedString.Key: Any] = [
            .font: UIFont.boldSystemFont(ofSize: 30),
            .foregroundColor: UIColor.white
        ]
        
        let textSize = initials.size(withAttributes: attributes)
        let textRect = CGRect(
            x: (iconSize.width - textSize.width) / 2,
            y: (iconSize.height - textSize.height) / 2,
            width: textSize.width,
            height: textSize.height
        )
        
        initials.draw(in: textRect, withAttributes: attributes)
        
        let iconImage = UIGraphicsGetImageFromCurrentImageContext()
        UIGraphicsEndImageContext()
        
        return App(
            bundleId: bundleId,
            name: name,
            iconData: iconImage?.pngData()
        )
    }
    
    // Get recently used apps from UserDefaults
    func getRecentApps() -> [App] {
        guard let recentBundleIds = UserDefaults.standard.stringArray(forKey: "recentApps") else {
            return []
        }
        
        return recentBundleIds.compactMap { bundleId in
            guard let name = UserDefaults.standard.string(forKey: "appName_\(bundleId)") else {
                return nil
            }
            
            return createApp(bundleId: bundleId, name: name)
        }
    }
    
    // Save an app to the recent apps list
    func saveToRecentApps(_ app: App) {
        var recentBundleIds = UserDefaults.standard.stringArray(forKey: "recentApps") ?? []
        
        // Remove if already exists
        if let index = recentBundleIds.firstIndex(of: app.bundleId) {
            recentBundleIds.remove(at: index)
        }
        
        // Add to the beginning
        recentBundleIds.insert(app.bundleId, at: 0)
        
        // Limit to 5 recent apps
        if recentBundleIds.count > 5 {
            recentBundleIds = Array(recentBundleIds.prefix(5))
        }
        
        // Save to UserDefaults
        UserDefaults.standard.set(recentBundleIds, forKey: "recentApps")
        UserDefaults.standard.set(app.name, forKey: "appName_\(app.bundleId)")
        UserDefaults.standard.synchronize()
    }
}