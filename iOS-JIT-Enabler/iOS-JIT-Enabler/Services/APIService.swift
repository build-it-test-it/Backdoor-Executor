import Foundation

enum APIError: Error {
    case invalidURL
    case requestFailed(Error)
    case invalidResponse
    case decodingFailed(Error)
    case serverError(String)
    case unauthorized
    case unknown
    
    var localizedDescription: String {
        switch self {
        case .invalidURL:
            return "Invalid URL"
        case .requestFailed(let error):
            return "Request failed: \(error.localizedDescription)"
        case .invalidResponse:
            return "Invalid response from server"
        case .decodingFailed(let error):
            return "Failed to decode response: \(error.localizedDescription)"
        case .serverError(let message):
            return "Server error: \(message)"
        case .unauthorized:
            return "Unauthorized. Please log in again."
        case .unknown:
            return "An unknown error occurred"
        }
    }
}

class APIService {
    private let baseURL: URL
    private var authToken: String?
    
    init(baseURLString: String) {
        if let url = URL(string: baseURLString) {
            self.baseURL = url
        } else {
            // Fallback to a default URL if the provided one is invalid
            self.baseURL = URL(string: "https://example.com")!
        }
        
        // Load auth token from keychain if available
        self.authToken = KeychainManager.shared.getAuthToken()
    }
    
    func setAuthToken(_ token: String) {
        self.authToken = token
        KeychainManager.shared.saveAuthToken(token)
    }
    
    func clearAuthToken() {
        self.authToken = nil
        KeychainManager.shared.deleteAuthToken()
    }
    
    // MARK: - API Methods
    
    func registerDevice(_ device: Device, completion: @escaping (Result<DeviceRegistrationResponse, APIError>) -> Void) {
        let request = DeviceRegistrationRequest.fromDevice(device)
        
        post(endpoint: "register", body: request, requiresAuth: false) { (result: Result<DeviceRegistrationResponse, APIError>) in
            switch result {
            case .success(let response):
                // Save the auth token
                self.setAuthToken(response.token)
                completion(.success(response))
            case .failure(let error):
                completion(.failure(error))
            }
        }
    }
    
    func enableJIT(for app: App, completion: @escaping (Result<JITEnablementResponse, APIError>) -> Void) {
        let request = JITEnablementRequest.forApp(app, iosVersion: UIDevice.current.systemVersion)
        
        post(endpoint: "enable-jit", body: request, requiresAuth: true) { (result: Result<JITEnablementResponse, APIError>) in
            completion(result)
        }
    }
    
    func getSessionStatus(sessionId: String, completion: @escaping (Result<Session, APIError>) -> Void) {
        get(endpoint: "session/\(sessionId)", requiresAuth: true) { (result: Result<SessionResponse, APIError>) in
            switch result {
            case .success(let response):
                // We need to create a Session from the response
                let session = response.toSession(withId: sessionId, appName: "Unknown App")
                completion(.success(session))
            case .failure(let error):
                completion(.failure(error))
            }
        }
    }
    
    func getDeviceSessions(completion: @escaping (Result<[Session], APIError>) -> Void) {
        get(endpoint: "device/sessions", requiresAuth: true) { (result: Result<SessionsResponse, APIError>) in
            switch result {
            case .success(let response):
                let sessions = response.sessions.map { $0.toSession() }
                completion(.success(sessions))
            case .failure(let error):
                completion(.failure(error))
            }
        }
    }
    
    // MARK: - Helper Methods
    
    private func get<T: Decodable>(endpoint: String, requiresAuth: Bool = true, completion: @escaping (Result<T, APIError>) -> Void) {
        guard let url = URL(string: endpoint, relativeTo: baseURL) else {
            completion(.failure(.invalidURL))
            return
        }
        
        var request = URLRequest(url: url)
        request.httpMethod = "GET"
        
        if requiresAuth {
            guard let token = authToken else {
                completion(.failure(.unauthorized))
                return
            }
            request.addValue("Bearer \(token)", forHTTPHeaderField: "Authorization")
        }
        
        request.addValue("application/json", forHTTPHeaderField: "Content-Type")
        
        performRequest(request, completion: completion)
    }
    
    private func post<T: Encodable, U: Decodable>(endpoint: String, body: T, requiresAuth: Bool = true, completion: @escaping (Result<U, APIError>) -> Void) {
        guard let url = URL(string: endpoint, relativeTo: baseURL) else {
            completion(.failure(.invalidURL))
            return
        }
        
        var request = URLRequest(url: url)
        request.httpMethod = "POST"
        
        if requiresAuth {
            guard let token = authToken else {
                completion(.failure(.unauthorized))
                return
            }
            request.addValue("Bearer \(token)", forHTTPHeaderField: "Authorization")
        }
        
        request.addValue("application/json", forHTTPHeaderField: "Content-Type")
        
        do {
            request.httpBody = try JSONEncoder().encode(body)
        } catch {
            completion(.failure(.requestFailed(error)))
            return
        }
        
        performRequest(request, completion: completion)
    }
    
    private func performRequest<T: Decodable>(_ request: URLRequest, completion: @escaping (Result<T, APIError>) -> Void) {
        let task = URLSession.shared.dataTask(with: request) { data, response, error in
            if let error = error {
                DispatchQueue.main.async {
                    completion(.failure(.requestFailed(error)))
                }
                return
            }
            
            guard let httpResponse = response as? HTTPURLResponse else {
                DispatchQueue.main.async {
                    completion(.failure(.invalidResponse))
                }
                return
            }
            
            // Check for HTTP status code
            switch httpResponse.statusCode {
            case 200...299:
                // Success
                break
            case 401:
                DispatchQueue.main.async {
                    completion(.failure(.unauthorized))
                }
                return
            default:
                if let data = data, let errorMessage = try? JSONDecoder().decode([String: String].self, from: data)["error"] {
                    DispatchQueue.main.async {
                        completion(.failure(.serverError(errorMessage)))
                    }
                } else {
                    DispatchQueue.main.async {
                        completion(.failure(.serverError("HTTP Error: \(httpResponse.statusCode)")))
                    }
                }
                return
            }
            
            guard let data = data else {
                DispatchQueue.main.async {
                    completion(.failure(.invalidResponse))
                }
                return
            }
            
            do {
                let decoder = JSONDecoder()
                let result = try decoder.decode(T.self, from: data)
                DispatchQueue.main.async {
                    completion(.success(result))
                }
            } catch {
                DispatchQueue.main.async {
                    completion(.failure(.decodingFailed(error)))
                }
            }
        }
        
        task.resume()
    }
}