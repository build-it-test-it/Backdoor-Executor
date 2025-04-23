import Foundation
import UIKit

struct App: Codable {
    let bundleId: String
    let name: String
    let iconData: Data?
    
    enum CodingKeys: String, CodingKey {
        case bundleId = "bundle_id"
        case name = "app_name"
        case iconData = "icon_data"
    }
    
    var icon: UIImage? {
        if let iconData = iconData {
            return UIImage(data: iconData)
        }
        return nil
    }
}

// JIT enablement request and response
struct JITEnablementRequest: Codable {
    let bundleId: String
    let iosVersion: String
    let appInfo: [String: String]?
    
    enum CodingKeys: String, CodingKey {
        case bundleId = "bundle_id"
        case iosVersion = "ios_version"
        case appInfo = "app_info"
    }
    
    static func forApp(_ app: App, iosVersion: String) -> JITEnablementRequest {
        return JITEnablementRequest(
            bundleId: app.bundleId,
            iosVersion: iosVersion,
            appInfo: ["name": app.name]
        )
    }
}

struct JITEnablementResponse: Codable {
    let status: String
    let sessionId: String
    let message: String
    let token: String?
    let method: String?
    let instructions: JITInstructions?
    
    enum CodingKeys: String, CodingKey {
        case status
        case sessionId = "session_id"
        case message
        case token
        case method
        case instructions
    }
}

struct JITInstructions: Codable {
    let setCsDebugged: Bool?
    let toggleWxMemory: Bool?
    let memoryRegions: [MemoryRegion]?
    
    enum CodingKeys: String, CodingKey {
        case setCsDebugged = "set_cs_debugged"
        case toggleWxMemory = "toggle_wx_memory"
        case memoryRegions = "memory_regions"
    }
}

struct MemoryRegion: Codable {
    let address: String
    let size: String
    let permissions: String
}