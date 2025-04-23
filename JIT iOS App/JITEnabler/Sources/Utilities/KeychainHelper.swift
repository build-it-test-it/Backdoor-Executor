import Foundation
import Security

class KeychainHelper {
    static let shared = KeychainHelper()
    
    private let tokenKey = "com.jitenabler.jwtToken"
    
    private init() {}
    
    // MARK: - Token Management
    
    func saveToken(_ token: String) {
        let tokenData = Data(token.utf8)
        
        // Create query for keychain
        let query: [String: Any] = [
            kSecClass as String: kSecClassGenericPassword,
            kSecAttrAccount as String: tokenKey,
            kSecValueData as String: tokenData,
            kSecAttrAccessible as String: kSecAttrAccessibleAfterFirstUnlock
        ]
        
        // Delete any existing token
        SecItemDelete(query as CFDictionary)
        
        // Add the new token
        let status = SecItemAdd(query as CFDictionary, nil)
        
        if status != errSecSuccess {
            print("Error saving token to keychain: \(status)")
        }
    }
    
    func getToken() -> String? {
        // Create query for keychain
        let query: [String: Any] = [
            kSecClass as String: kSecClassGenericPassword,
            kSecAttrAccount as String: tokenKey,
            kSecReturnData as String: true,
            kSecMatchLimit as String: kSecMatchLimitOne
        ]
        
        var result: AnyObject?
        let status = SecItemCopyMatching(query as CFDictionary, &result)
        
        guard status == errSecSuccess, let tokenData = result as? Data else {
            return nil
        }
        
        return String(data: tokenData, encoding: .utf8)
    }
    
    func deleteToken() {
        // Create query for keychain
        let query: [String: Any] = [
            kSecClass as String: kSecClassGenericPassword,
            kSecAttrAccount as String: tokenKey
        ]
        
        // Delete the token
        let status = SecItemDelete(query as CFDictionary)
        
        if status != errSecSuccess && status != errSecItemNotFound {
            print("Error deleting token from keychain: \(status)")
        }
    }
}