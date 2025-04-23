import Foundation
import UIKit

struct Device: Codable {
    let id: String?
    let udid: String
    let name: String
    let model: String
    let osVersion: String
    let registeredAt: Date?
    let lastActive: Date?
    
    enum CodingKeys: String, CodingKey {
        case id
        case udid
        case name = "device_name"
        case model = "device_model"
        case osVersion = "ios_version"
        case registeredAt = "registered_at"
        case lastActive = "last_active"
    }
    
    static func current() -> Device {
        let device = UIDevice.current
        let udid = device.identifierForVendor?.uuidString ?? UUID().uuidString
        
        return Device(
            id: nil,
            udid: udid,
            name: device.name,
            model: deviceModel(),
            osVersion: device.systemVersion,
            registeredAt: nil,
            lastActive: nil
        )
    }
    
    private static func deviceModel() -> String {
        var systemInfo = utsname()
        uname(&systemInfo)
        let machineMirror = Mirror(reflecting: systemInfo.machine)
        let identifier = machineMirror.children.reduce("") { identifier, element in
            guard let value = element.value as? Int8, value != 0 else { return identifier }
            return identifier + String(UnicodeScalar(UInt8(value)))
        }
        
        // Map common device identifiers to human-readable names
        switch identifier {
        case "iPhone14,2": return "iPhone 13 Pro"
        case "iPhone14,3": return "iPhone 13 Pro Max"
        case "iPhone14,4": return "iPhone 13 mini"
        case "iPhone14,5": return "iPhone 13"
        case "iPhone15,2": return "iPhone 14 Pro"
        case "iPhone15,3": return "iPhone 14 Pro Max"
        case "iPhone15,4": return "iPhone 14"
        case "iPhone15,5": return "iPhone 14 Plus"
        case "iPhone16,1": return "iPhone 15 Pro"
        case "iPhone16,2": return "iPhone 15 Pro Max"
        case "iPhone16,3": return "iPhone 15"
        case "iPhone16,4": return "iPhone 15 Plus"
        default: return identifier
        }
    }
}

// Registration request and response
struct DeviceRegistrationRequest: Codable {
    let udid: String
    let deviceName: String
    let iosVersion: String
    let deviceModel: String
    
    enum CodingKeys: String, CodingKey {
        case udid
        case deviceName = "device_name"
        case iosVersion = "ios_version"
        case deviceModel = "device_model"
    }
    
    static func fromDevice(_ device: Device) -> DeviceRegistrationRequest {
        return DeviceRegistrationRequest(
            udid: device.udid,
            deviceName: device.name,
            iosVersion: device.osVersion,
            deviceModel: device.model
        )
    }
}

struct DeviceRegistrationResponse: Codable {
    let token: String
    let message: String
}