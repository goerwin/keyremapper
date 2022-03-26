import Foundation
import AppKit

struct Global {
  static let CHECK_CLIENT_INTERVAL = 1.0
    static let VERSION = Bundle.main.infoDictionary?["CFBundleShortVersionString"] as! String
    static var appBridge: AppBridge?
  
  static func setFrontmostAppNameToKeyRemapper() {
    let frontmostApp = NSWorkspace.shared.frontmostApplication
    let bundleId = frontmostApp?.bundleIdentifier
    let localizedName = frontmostApp?.localizedName

    Global.appBridge?.setAppName((bundleId != nil) ? bundleId : (localizedName != nil) ? localizedName : "Unknown")
  }
}
