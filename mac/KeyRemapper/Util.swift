import SecurityFoundation
import ServiceManagement
import SystemConfiguration
import AppKit

struct Util {
    static func getPrivilegedHelperAuth() -> AuthorizationRef? {
      var authItem = AuthorizationItem(name: kSMRightBlessPrivilegedHelper, valueLength: 0, value:UnsafeMutableRawPointer(bitPattern: 0), flags: 0)
      var rights = AuthorizationRights(count: 1, items: &authItem)
      var authRef: AuthorizationRef?
      AuthorizationCreate(&rights, nil, [.interactionAllowed, .extendRights, .preAuthorize], &authRef)
    
      return authRef
    }
    
    static func blessHelper(label: String, authRef: AuthorizationRef) -> Bool {
        var error: Unmanaged<CFError>?
        let blessStatus = SMJobBless(kSMDomainSystemLaunchd, label as CFString, authRef, &error)
        
        if !blessStatus {
            NSLog("[SMJBS]: Helper bless failed with error \(error!.takeUnretainedValue())")
        }
        
        return blessStatus
    }
  
    static func fileExists(_ filePath: String) -> Bool {
      let fileManager = FileManager.default
      if fileManager.fileExists(atPath: filePath) {
        return true
      }

      return false
  }
  
  
  static func resizeImage(image: NSImage, w: Int, h: Int) -> NSImage? {
    let frame = NSRect(x: 0, y: 0, width: w, height: h)
    let newSize = NSSize(width: w, height: h)

    guard let representation = image.bestRepresentation(for: frame, context: nil, hints: nil) else {
        return nil
    }
    let image = NSImage(size: newSize, flipped: false, drawingHandler: { (_) -> Bool in
        return representation.draw(in: frame)
    })

    return image
  }
  
  static func getCurLoggedInUserFromRoot() -> String? {
    var uid: uid_t = 0
    var gid: gid_t = 0

    guard let name = SCDynamicStoreCopyConsoleUser(nil, &uid, &gid) else {
      return nil
    }

    return name as String;
  }

  static func getRootPath() -> String? {
    // Since we are running from root, the current user is root so
    // I have to use this to get the actual logged in user
    guard let currentLoggedInUser = getCurLoggedInUserFromRoot() else { return nil }
    return "/Users/\(currentLoggedInUser)/keyRemapperMac"
  }

  static func getConfigName(configPath: String) -> String? {
    do {
        let data = try Data(contentsOf: URL(fileURLWithPath: configPath), options: .mappedIfSafe)
      let jsonResult = try JSONSerialization.jsonObject(with: data, options: .mutableLeaves)
      if let jsonResult = jsonResult as? Dictionary<String, AnyObject>, let name = jsonResult["name"] as? String {
          return name
      }
    } catch {}

    return nil
  }
  
  static func getConfigPath(configIdx: Int) -> String? {
    guard let rootPath = getRootPath() else { return nil }
    let idxStr = configIdx == 0 ? "" : String(configIdx + 1)
    return "\(rootPath)/config\(idxStr).json"
  }
  
  static func getSymbolsPath() -> String? {
    guard let rootPath = getRootPath() else { return nil }
    return "\(rootPath)/symbols.json"
  }
  
  static func copyFile(srcPath filePath: String, to destPath: String) throws {
    let data = try Data(contentsOf: URL(fileURLWithPath: filePath))
    let filemgr = FileManager.default
    filemgr.createFile(atPath: destPath, contents: data, attributes: nil)
}
}
