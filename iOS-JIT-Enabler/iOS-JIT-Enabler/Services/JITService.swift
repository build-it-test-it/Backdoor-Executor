import Foundation
import UIKit

class JITService {
    private let apiService: APIService
    
    init(apiService: APIService) {
        self.apiService = apiService
    }
    
    // Register the device with the backend
    func registerDevice(completion: @escaping (Result<String, Error>) -> Void) {
        let device = Device.current()
        
        apiService.registerDevice(device) { result in
            switch result {
            case .success(let response):
                completion(.success(response.message))
            case .failure(let error):
                completion(.failure(error))
            }
        }
    }
    
    // Enable JIT for a specific app
    func enableJIT(for app: App, completion: @escaping (Result<JITEnablementResponse, Error>) -> Void) {
        apiService.enableJIT(for: app) { result in
            switch result {
            case .success(let response):
                // Process the JIT enablement response
                self.processJITInstructions(response) { success in
                    if success {
                        completion(.success(response))
                    } else {
                        completion(.failure(NSError(domain: "JITService", code: 1001, userInfo: [NSLocalizedDescriptionKey: "Failed to apply JIT instructions"])))
                    }
                }
            case .failure(let error):
                completion(.failure(error))
            }
        }
    }
    
    // Process the JIT enablement instructions from the backend
    private func processJITInstructions(_ response: JITEnablementResponse, completion: @escaping (Bool) -> Void) {
        guard let instructions = response.instructions else {
            completion(false)
            return
        }
        
        // Delegate the actual JIT enablement to the JITEnabler
        JITEnabler.shared.enableJIT(
            method: response.method ?? "generic",
            setCsDebugged: instructions.setCsDebugged ?? false,
            toggleWxMemory: instructions.toggleWxMemory ?? false,
            memoryRegions: instructions.memoryRegions ?? []
        ) { success in
            completion(success)
        }
    }
    
    // Get the status of a JIT session
    func getSessionStatus(sessionId: String, completion: @escaping (Result<Session, Error>) -> Void) {
        apiService.getSessionStatus(sessionId: sessionId) { result in
            switch result {
            case .success(let session):
                completion(.success(session))
            case .failure(let error):
                completion(.failure(error))
            }
        }
    }
    
    // Get all sessions for the current device
    func getDeviceSessions(completion: @escaping (Result<[Session], Error>) -> Void) {
        apiService.getDeviceSessions { result in
            switch result {
            case .success(let sessions):
                completion(.success(sessions))
            case .failure(let error):
                completion(.failure(error))
            }
        }
    }
    
    // Launch an app after enabling JIT
    func launchApp(withBundleId bundleId: String) -> Bool {
        guard let url = URL(string: "\(bundleId)://") else {
            return false
        }
        
        if UIApplication.shared.canOpenURL(url) {
            UIApplication.shared.open(url, options: [:], completionHandler: nil)
            return true
        }
        
        return false
    }
}