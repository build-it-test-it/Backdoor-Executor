import Foundation

struct Session: Codable {
    let id: String
    let bundleId: String
    let appName: String
    let status: String
    let startedAt: Date
    let completedAt: Date?
    let method: String?
    
    enum CodingKeys: String, CodingKey {
        case id
        case bundleId = "bundle_id"
        case appName = "app_name"
        case status
        case startedAt = "started_at"
        case completedAt = "completed_at"
        case method
    }
    
    var isActive: Bool {
        return status == "active" || status == "completed"
    }
    
    var statusDisplayText: String {
        switch status {
        case "processing":
            return "Processing"
        case "completed":
            return "JIT Enabled"
        case "failed":
            return "Failed"
        case "expired":
            return "Expired"
        case "active":
            return "Active"
        default:
            return status.capitalized
        }
    }
    
    var formattedStartTime: String {
        let formatter = DateFormatter()
        formatter.dateStyle = .short
        formatter.timeStyle = .short
        return formatter.string(from: startedAt)
    }
}

struct SessionResponse: Codable {
    let status: String
    let startedAt: TimeInterval
    let completedAt: TimeInterval?
    let bundleId: String
    let method: String?
    
    enum CodingKeys: String, CodingKey {
        case status
        case startedAt = "started_at"
        case completedAt = "completed_at"
        case bundleId = "bundle_id"
        case method
    }
    
    func toSession(withId id: String, appName: String) -> Session {
        return Session(
            id: id,
            bundleId: bundleId,
            appName: appName,
            status: status,
            startedAt: Date(timeIntervalSince1970: startedAt),
            completedAt: completedAt != nil ? Date(timeIntervalSince1970: completedAt!) : nil,
            method: method
        )
    }
}

struct SessionsResponse: Codable {
    let sessions: [SessionWrapper]
    
    struct SessionWrapper: Codable {
        let id: String
        let bundleId: String
        let appName: String
        let status: String
        let startedAt: TimeInterval
        let completedAt: TimeInterval?
        
        enum CodingKeys: String, CodingKey {
            case id
            case bundleId = "bundle_id"
            case appName = "app_name"
            case status
            case startedAt = "started_at"
            case completedAt = "completed_at"
        }
        
        func toSession() -> Session {
            return Session(
                id: id,
                bundleId: bundleId,
                appName: appName,
                status: status,
                startedAt: Date(timeIntervalSince1970: startedAt),
                completedAt: completedAt != nil ? Date(timeIntervalSince1970: completedAt!) : nil,
                method: nil
            )
        }
    }
}