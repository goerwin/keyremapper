import Foundation

struct Constants {
  private static func getValue(forKey key: String) -> String {
    guard let value = Bundle.main.infoDictionary?[key] as? String else {
      fatalError("Missing \(key) in Info.plist")
    }
    return value
  }

  static let MACH_SERVICE_NAME = {
    let type = getValue(forKey: "AppType")
    let key = (type == "Client") ? "DaemonBundleIdentifier" : "CFBundleIdentifier"
    return getValue(forKey: key)
  }()

  static let BUNDLE_NAME = getValue(forKey: "CFBundleName")
  static let VERSION = getValue(forKey: "CFBundleShortVersionString")
  static let OWNER_NAME = getValue(forKey: "Owner")
  static let OWNER_WEBSITE = getValue(forKey: "OwnerWebsite")
}
